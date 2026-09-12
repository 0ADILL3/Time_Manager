# Time_Manager

Sebuah pustaka (*library*) ringan untuk mengelola pewaktuan internal ESP32 tanpa memerlukan modul RTC eksternal (seperti DS3231) atau koneksi internet (NTP). Sistem ini dirancang untuk beroperasi secara *offline* dengan tingkat ketahanan tinggi terhadap *crash* maupun kegagalan daya.

Pustaka ini menggunakan skrip Python untuk melakukan sinkronisasi awal melalui koneksi Serial. Setelah disinkronkan, ESP32 akan menghitung waktu secara mandiri menggunakan perangkat keras pengatur waktu (*hardware timer*) internalnya.

## Fitur Utama
* **Tanpa Koneksi Internet & RTC Eksternal**: Sangat cocok untuk *data logger* di area terpencil.
* **Tahan Banting terhadap Restart (RTC RAM)**: Jika ESP32 mengalami *hang* atau direstart oleh Watchdog Timer (WDT), waktu akan langsung dipulihkan secara presisi dari memori RTC RAM (terkalibrasi dengan jeda kompensasi 3 detik saat *booting*).
* **Cadangan Mati Listrik (NVS)**: Waktu dicadangkan ke dalam *flash memory* (Non-Volatile Storage) secara otomatis setiap 10 menit. Jika daya mati total, sistem tidak akan tereset ke tahun 1970, melainkan melanjutkan dari titik penyimpanan terakhir.
* **Sinkronisasi Cerdas via Python**: Skrip pengumpan (*feeder*) otomatis memindai port COM ESP32 dan melakukan *handshake* konfirmasi untuk memastikan waktu berhasil terkirim.

## Instalasi
1. Buka Arduino IDE.
2. Masuk ke menu **Sketch > Include Library > Add .ZIP Library...**
3. Pilih *file* `.zip` dari repositori ini, atau cari **TimeManager** di Library Manager (jika sudah terindeks).

## Cara Penggunaan

### 1. Sisi ESP32 (C++)
Masukkan pustaka ke dalam kode Anda dan panggil `serial_feed_handler()` di dalam `loop()` utama. Lihat folder `examples/` untuk referensi lengkap yang menyertakan Watchdog Timer (WDT).

### 2. Sisi Komputer (Python)
Skrip sinkronisasi berada di dalam folder `tools/time_feeder.py`.
Pastikan Anda telah menginstal pustaka `pyserial`:
```bash
pip install pyserial
```
Lalu jalankan skrip saat ESP32 terhubung ke USB komputer:
```bash
python tools/time_feeder.py
```