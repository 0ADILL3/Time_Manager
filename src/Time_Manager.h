#pragma once

#include <Arduino.h>
#include <sys/time.h>
#include <time.h>
#include <Preferences.h>

#define TIME_FILTER 1000000000UL
#define NVS_SAVE_INTERVAL 10*60*1000UL

/**
 * @class Time_Manager
 * @brief Sistem manajemen waktu offline ringan untuk ESP32 dengan perlindungan memori ganda (NVS & RTC RAM).
 */
class Time_Manager
{
  private:
    Preferences prefs_;
    unsigned long last_saved_millis = 0;

    void update_interval_time_(uint32_t epoch);

    struct tm get_time_struct_();

  public:
    /**
     * @brief Konstruktor
     */
    Time_Manager();

    /**
     * @brief Menginisialisasi sistem waktu dan memulihkan cadangan.
     * 
     * Wajib dipanggil di dalam setup(). Fungsi ini mengatur zona waktu, 
     * membuka namespace NVS, dan mencoba memulihkan waktu secara otomatis 
     * dari RTC RAM (jika terjadi soft-restart) atau dari NVS (jika terjadi cold-boot).
     */
    void begin();

    /**
     * @brief Pendengar (listener) serial untuk menangkap umpan waktu dari skrip Python.
     * 
     * Wajib dipanggil secara konstan di dalam loop() agar ESP32 siap merespons
     * instruksi sinkronisasi dan mengirimkan status balasan (acknowledgement).
     */
    void serial_feed_handler(String serial_data = "NULL");

    /**
     * @brief Mendapatkan waktu UNIX Epoch saat ini.
     * 
     * Fungsi ini bertindak sebagai "Smart Getter". Selain mengambil waktu, 
     * fungsi ini juga memicu pengamanan data terbaru ke RTC RAM dan NVS (sesuai interval).
     * 
     * @return time_t Jumlah detik sejak 1 Januari 1970.
     */
    time_t get_epoch();

    /**
     * @brief Mendapatkan waktu dalam format string teks.
     * 
     * @param format C-String format specifier standar strftime (Default: "%Y-%m-%d %H:%M:%S").
     * @return String berisi waktu yang telah diformat. Jika gagal/belum sinkron, mengembalikan "Unsynchronized".
     */
    String get_time_f(const char* format = "%Y-%m-%d %H:%M:%S");

    /**
     * @brief Mengambil informasi Tahun.
     * @return Angka tahun (contoh: 2026).
     */
    uint16_t get_year();

    /**
     * @brief Mengambil informasi Bulan.
     * @return Angka bulan dalam rentang 1 - 12.
     */
    uint8_t get_month();

    /**
     * @brief Mengambil indeks Hari dalam seminggu.
     * @return Angka indeks hari (0 = Minggu, 1 = Senin, dst).
     */
    uint8_t get_weekday();

    /**
     * @brief Mengambil informasi Tanggal pada bulan terkait.
     * @return Angka tanggal dalam rentang 1 - 31.
     */
    uint8_t get_day();

    /**
     * @brief Mengambil informasi Jam.
     * @return Angka jam dalam format 24-jam (0 - 23).
     */
    uint8_t get_hour();

    /**
     * @brief Mengambil informasi Menit.
     * @return Angka menit (0 - 59).
     */
    uint8_t get_minute();

    /**
     * @brief Mengambil informasi Detik.
     * @return Angka detik (0 - 59).
     */
    uint8_t get_second();

    /**
     * @brief Mengecek apakah ESP32 sudah memiliki waktu yang tervalidasi.
     * 
     * Waktu dinyatakan valid apabila melewati ambang batas definisi TIME_FILTER (> tahun 2001).
     * 
     * @return true Jika waktu berhasil disinkronkan atau dipulihkan dari memori.
     * @return false Jika ESP32 masih tertahan di tahun 1970.
     */
    bool is_ready();
};