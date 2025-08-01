#ifndef PASSWORD_H_
#define PASSWORD_H_

#include <Arduino.h>
#ifdef __cplusplus // To adapt to C and C++ disable mangling naming
extern "C" {
#endif   //__cplusplus
//header code



/************************    defines configurations    *****************************/
#define PASSWORD_SIZE                4
#define MAX_ENTRY_PASS_TIME          20000 // by ms
#define EEPROM_CLIENT_PASS_FIRST_BYTR       0      //    0
#define EEPROM_CLIENT_PASS_SECOND_BYTR      1       //    0
#define EEPROM_CLIENT_PASS_THIRD_BYTR       2       //    0
#define EEPROM_CLIENT_PASS_FOURTH_BYTR      3       //    0
#define EEPROM_MASTER_PASS_FIRST_BYTR       4       //    1
#define EEPROM_MASTER_PASS_SECOND_BYTR      5       //    0
#define EEPROM_MASTER_PASS_THIRD_BYTR       6       //    1
#define EEPROM_MASTER_PASS_FOURTH_BYTR      7       //    0



/************************ typedef *****************************/
typedef enum {

  INVAILD_PASSWORD ,
  VAILD_PASSWORD
}
tPassword_state;
typedef enum
{

  INVAILD_COMPARE ,
  VAILD_COMPARE
}
tCompare_state;

/************************ variables *****************************/
extern uint8_t eg_savedPass[PASSWORD_SIZE];
extern uint8_t eg_masterPass[PASSWORD_SIZE];
extern uint8_t eg_userPass[PASSWORD_SIZE];
/****************************************************************/

static const uint8_t sg_defaultUserPass[PASSWORD_SIZE] = {0, 0, 0, 0};
static const uint8_t sg_defaultMasterPass[PASSWORD_SIZE] = {1, 0, 1, 0};
/************************ functions declaration *****************************/
void Password_Init(void);
void Password_Reset(void);
void Password_Update(uint8_t* ptrTopass, uint8_t l_eeprom_first_byte) ;
tPassword_state Password_getPass(uint8_t* ptrTopass);
tPassword_state Password_setNew(uint8_t* ptrTopass);
tCompare_state Password_compare( uint8_t* ptrTopass1, uint8_t* ptrTopass2);
void Password_Display(uint8_t* ptrTopass);

#ifdef __cplusplus
}
#endif   //__cplusplus
#endif  //PASSWORD_H_
