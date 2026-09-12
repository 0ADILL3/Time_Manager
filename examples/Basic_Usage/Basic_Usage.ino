#include <Arduino.h>
#include <Time_Manager.h>
#include <esp_task_wdt.h>

// Set batas waktu toleransi Watchdog Timer (misal 10 detik)
#define WDT_TIMEOUT 10

Time_Manager rtc;

void setup() {
  Serial.begin(115200);
  delay(1000); 
  
  // Menginisialisasi sistem waktu (memulihkan dari RTC RAM atau NVS jika ada)
  rtc.begin();

  // Mendaftarkan Watchdog Timer untuk melindungi sistem dari macet/hang
  esp_task_wdt_init(WDT_TIMEOUT, true); 
  esp_task_wdt_add(NULL);               
}

void loop() {
  // Mereset hitungan mundur WDT di setiap perulangan
  esp_task_wdt_reset();

  // Wajib dipanggil untuk mendengarkan instruksi sinkronisasi dari Python
  rtc.serial_feed_handler();

  // Membaca dan menampilkan waktu setiap 3 detik
  static unsigned long last_print = 0;
  if (millis() - last_print > 1000) {
    last_print = millis();
    
    // Cek apakah sistem sudah pernah disinkronkan minimal satu kali
    if (rtc.is_ready()) {
      Serial.print("Waktu Saat Ini: ");
      Serial.println(rtc.get_time_f());
      
      // Contoh mengambil nilai individual
      // Serial.printf("Jam: %02d:%02d\n", rtc.get_hour(), rtc.get_minute());
    } else {
      Serial.println("Menunggu sinkronisasi dari skrip Python...");
    }
  }
}