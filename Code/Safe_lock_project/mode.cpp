#include "mode.h"
#include "common_types.h"
#include "unit.h"
#include "rfid.h"
/************************Variables definetions**********************************/
tMode g_mode = IDEAL;

/************************Functions definetions**********************************/
void MODE_Init(void)
{
  g_mode = IDEAL;
}
void MODE_Update(void)
{
//  switch (CHECK_VALIDATION_GetState()) {
//    case INVALID_STATE:
//      switch (g_mode) {
//        case IDEAL:
//          g_mode = IDEAL;
//          Serial.println("INVALEDSTATE_IN_MODE");
//          break;
//        case PROGRAMMING:
//          if ((millis() - g_authorised_data) > (PROGRAMMIN_PERIOD)) {
//            BUZZER_Flush(PROGRAMMING);
//            g_mode = IDEAL;
//            Serial.println("INVALEDSTATE_IN_AUTHORISED");
//          }
//          break;
//        case LIGHT_ONLY:
//          if ((millis() - g_light_only_last_date) > (OFF_DELAY)) {
//            g_mode = IDEAL;
//            Serial.println("INVALEDSTATE_IN_LIGHT_ONLY");
//          }
//          break;
//        case ALL_OUTPUTS:
//          if ((millis() - g_all_outputs_date) > (OFF_DELAY)) {
//            g_mode = IDEAL;
//            Serial.println("INVALEDSTATE_IN_ALL_OUTPUTS");
//          }
//          break;
//        default:
//          g_mode = IDEAL;
//          Serial.println("INVALEDSTATE_IN_DEFAULT");
//          break;
//      }
//      break;
//
//    case VALID_STATE:
//      {
//        Serial.println("VALEDSTATEINMODE");
//        switch (g_mode) {
//          case IDEAL:
//            Serial.println("IDEAL");
//            switch (g_Current_card.Type) {
//              case AUTHORISED:
//                Serial.println("AUTHORISED");
//                g_mode = PROGRAMMING;
//                BUZZER_Flush(AUTHORISED);
//                g_authorised_data = millis();  // --------------------->>
//                Serial.println("PROGRAMMING1");
//                break;
//              //  case MASTER:
//              case EMRGANCY:
//              // case BUILDING:
//              // case FLOOR:
//              case CLIENT:
//                Serial.println("MASTER-EMRGANCY-BUILDING-FLOOR-CLIENT");
//                g_mode = LIGHT_ONLY;
//                g_light_only_start_date = millis();  // --------------------->>
//                break;
//              default:
//                break;
//            }
//            break;
//          case PROGRAMMING:
//            Serial.println("PROGRAMMING2");
//            if ((millis() - g_authorised_data) <= (PROGRAMMIN_PERIOD)) {
//              switch (g_Current_card.Type) {
//                  Serial.println("PROGRAMMING3");
//                case ROOM:
//                  Serial.println("ROOM");
//                  g_unit_location.Room = g_buffer[ROOM_BYTE_NUM];
//                  g_unit_location.Floor = g_buffer[FLOOR_BYTE_NUM];
//                  g_unit_location.Building = g_buffer[BUILDING_BYTE_NUM];
//                  UNIT_Update();
//                  BUZZER_Flush(ROOM);
//                  g_mode = IDEAL;
//                  break;
//                case TIMESYNC:
//                  Serial.println("TIMESYNC");
//                  g_sync_date.Minute = (g_buffer[SYNC_MINET_BYTE_NUM] & 0x7f);
//                  g_sync_date.Hour = (((g_buffer[SYNC_MINET_BYTE_NUM] & 0x80) >> 3) | (g_buffer[SYNC_HOUR_BYTE_NUM] & 0x0f));
//                  g_sync_date.Day = (g_buffer[SYNC_MONTH_DAY_BYTE_NUM] & 0x0f) | ((g_buffer[SYNC_YEAR_BYTE_NUM] & 0x80) >> 3);
//                  g_sync_date.Month = (g_buffer[SYNC_MONTH_DAY_BYTE_NUM] >> 4);
//                  g_sync_date.Year = (g_buffer[SYNC_YEAR_BYTE_NUM] & 0x7f);
//                  RTC_Set();
//                  BUZZER_Flush(TIMESYNC);
//                  g_mode = IDEAL;
//                  break;
//                default:
//                  break;
//              }
//            } else {
//              Serial.println("PROGRAMMING4");
//              BUZZER_Flush(PROGRAMMING);
//              g_mode = IDEAL;
//            }
//            break;
//          case LIGHT_ONLY:
//            Serial.println("LIGHT_ONLY");
//            g_light_only_last_date = millis();  // --------------------->>
//            switch (g_Current_card.Type) {
//              case CLIENT:
//              case EMRGANCY:
//                // case MASTER:
//                Serial.println("CLIENT-EMRGANCY-MASTER");
//
//                if ((millis() - g_light_only_start_date) >= (AC_DELAY)) {
//                  g_mode = ALL_OUTPUTS;
//                }
//                break;
//              default:
//                break;
//            }
//            break;
//          case ALL_OUTPUTS:
//            switch (g_Current_card.Type) {
//              case CLIENT:
//              case EMRGANCY:
//                //case MASTER:
//                g_all_outputs_date = millis();  //------------------------------------>
//                Serial.println("ALLOUTPUTS");
//                break;
//              default:
//                g_mode = IDEAL;
//            }
//            break;
//          default:
//            g_mode = IDEAL;
//            break;
//        }
//      }
//      break;
//    default:
//      g_mode = IDEAL;
//      break;
//  }
}
