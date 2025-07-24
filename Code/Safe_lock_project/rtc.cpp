#include"rtc.h"
#include "RTClib.h"
RTC_DS1307 rtc;
/**************************************************************/
#define SECONDS 0
#define DAYOFWEEK 1
/**************************** variables **********************************/
#define SECONDS_FROM_1970_TO_2000 946684800
const uint8_t daysInMonth[11] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30};
static uint32_t g_current_unixtime;
tDate g_current_date = {0, 0, 0, 0, 0};
/**************************Function Decleration*************************************/
uint16_t Date2Days(uint16_t y, uint8_t m, uint8_t d);
uint32_t Time2Ulong(uint16_t days, uint8_t h, uint8_t m);
/**************************Function Definitions*************************************/

void RTC_Init(void)
{

  if (!rtc.begin())
  {
    /**@note
       need to add buzzer for RTC not found
    */
    Serial.println("Couldn't find RTC");
    Serial.flush();
    g_current_unixtime = 0;
    g_current_date.Minute = 0;
    g_current_date.Hour = 0;
    g_current_date.Day = 0;
    g_current_date.Month = 0;
    g_current_date.Year = 0;
  }

  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running, let's set the time!");
  }
  RTC_Update();
}
void RTC_Set(tDate* date)
{

  rtc.adjust(DateTime(date->Year, date->Month, date->Day, date->Hour, date->Minute, 0));
}
void RTC_Update(void)
{
  DateTime now = rtc.now();
  g_current_unixtime = now.unixtime();
  g_current_date.Minute = now.minute();
  g_current_date.Hour = now.hour();
  g_current_date.Day = now.day();
  g_current_date.Month = now.month();
  g_current_date.Year = now.year() - 2000;
}

uint32_t UnixTime(tDate* date)
{
  uint32_t t;
  uint16_t days = Date2Days(date->Year, date->Month, date->Day);
  t = Time2Ulong(days, date->Hour, date->Minute);
  t += SECONDS_FROM_1970_TO_2000; // seconds from 1970 to 2000

  return t;
}
uint16_t Date2Days(uint16_t y, uint8_t m, uint8_t d)
{
  if (y >= 2000U)
    y -= 2000U;
  uint16_t days = d;
  for (uint8_t i = 1; i < m; ++i)
  {
    days += daysInMonth[i - 1];
  }
  if (m > 2 && y % 4 == 0) // if current year is leap
    ++days;
  return days + 365 * y + (y + 3) / 4 - 1; // num of previce leap years
}
uint32_t Time2Ulong(uint16_t days, uint8_t h, uint8_t m)
{
  return ((days * 24UL + h) * 60 + m) * 60;
}
bool RTC_IsTimeValid(tDate* date)
{
  if (UnixTime(date) > g_current_unixtime)
  {
    return true; // The date is valid if it is in the future
  }
  return false; // The date is invalid if it is not in the future
}
void RealTimeDisplay(void) {
  Serial.print("Current Time = ");
  if (g_current_date.Hour < 10) Serial.print("0");
  Serial.print(g_current_date.Hour);
  Serial.print(":");
  if (g_current_date.Minute < 10) Serial.print("0");
  Serial.print(g_current_date.Minute);

  Serial.print(" | Date = ");
  if (g_current_date.Day < 10) Serial.print("0");
  Serial.print(g_current_date.Day);
  Serial.print("-");
  if (g_current_date.Month < 10) Serial.print("0");
  Serial.print(g_current_date.Month);
  Serial.print("-");
  Serial.print(g_current_date.Year + 2000); // Convert back to full year

  Serial.print(" | Unix Time = ");
  Serial.println(g_current_unixtime);
}
