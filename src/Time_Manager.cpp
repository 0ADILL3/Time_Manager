#include "Time_Manager.h"

RTC_NOINIT_ATTR uint32_t rtc_saved_epoch;
RTC_NOINIT_ATTR uint32_t rtc_magic_marker;

Time_Manager::Time_Manager() {}

void Time_Manager::update_interval_time_(uint32_t epoch)
{
  struct timeval tv;
  tv.tv_sec = epoch;
  tv.tv_usec = 0;
  settimeofday(&tv, NULL);
}

struct tm Time_Manager::get_time_struct_()
{
  time_t now = get_epoch();
  struct tm timeinfo = {0};
  if (now > TIME_FILTER) {
    localtime_r(&now, &timeinfo);
  }
  return timeinfo;
}

void Time_Manager::begin()
{
  setenv("TZ", "WIB-7", 1);
  tzset();

  prefs_.begin("Time_Manager", false);
  
  if (rtc_magic_marker == 0x55AA55AA && rtc_saved_epoch > TIME_FILTER)
  {
    update_interval_time_(rtc_saved_epoch + 3); 
    Serial.printf("[Time_Manager] Waktu dipulihkan dari RTC RAM: %s\n", get_time_f().c_str());
  }
  else
  {
    uint32_t last_epoch_nvs = prefs_.getUInt("last_epoch", 0);
    if (last_epoch_nvs > TIME_FILTER)
    {
      update_interval_time_(last_epoch_nvs);
      Serial.printf("[Time_Manager] Waktu dipulihkan dari NVS: %s\n", get_time_f().c_str());
    }
  }
}

void Time_Manager::serial_feed_handler(String serial_data)
{
  if (serial_data == "NULL")
  {
    if (Serial.available())
    {
      serial_data = Serial.readStringUntil('\n');
      
      if (serial_data.startsWith("[Time_Manager]"))
      {
        uint32_t epoch_time = serial_data.substring(14).toInt();
        
        update_interval_time_(epoch_time);
        prefs_.putUInt("last_feed", epoch_time);
        prefs_.putUInt("last_epoch", epoch_time);
        last_saved_millis = millis(); 
        
        Serial.printf("[Time_Manager] Sinkronisasi via Serial sukses! epoch:%u\n", epoch_time);
      }
    }
  }
  else
  {
    if (serial_data.startsWith("[Time_Manager]"))
    {
      uint32_t epoch_time = serial_data.substring(14).toInt();
      
      update_interval_time_(epoch_time);
      prefs_.putUInt("last_feed", epoch_time);
      prefs_.putUInt("last_epoch", epoch_time);
      last_saved_millis = millis(); 
      
      Serial.printf("[Time_Manager] Sinkronisasi via Serial sukses! epoch:%u\n", epoch_time);
    }
  }
}

time_t Time_Manager::get_epoch()
{
  time_t now = time(NULL);
  
  if (now > TIME_FILTER)
  {
    rtc_saved_epoch = now;
    rtc_magic_marker = 0x55AA55AA;

    if (millis() - last_saved_millis >= NVS_SAVE_INTERVAL)
    {
      last_saved_millis = millis();
      prefs_.putUInt("last_epoch", now);
    }
  }
  return now;
}

String Time_Manager::get_time_f(const char* format)
{
  if (!is_ready()) {return "Unsynchronized";}

  struct tm timeinfo = get_time_struct_();
  char buffer[64]; 
  strftime(buffer, sizeof(buffer), format, &timeinfo);
  
  return String(buffer);
}

uint16_t Time_Manager::get_year()   {return get_time_struct_().tm_year + 1900;}
uint8_t Time_Manager::get_month()   {return get_time_struct_().tm_mon + 1;}
uint8_t Time_Manager::get_weekday() {return get_time_struct_().tm_wday;}
uint8_t Time_Manager::get_day()     {return get_time_struct_().tm_mday;}
uint8_t Time_Manager::get_hour()    {return get_time_struct_().tm_hour;}
uint8_t Time_Manager::get_minute()  {return get_time_struct_().tm_min;}
uint8_t Time_Manager::get_second()  {return get_time_struct_().tm_sec;}

bool Time_Manager::is_ready() {return time(NULL) > TIME_FILTER;}