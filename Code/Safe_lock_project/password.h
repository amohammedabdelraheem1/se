#ifndef PASSWORD_H_
#define PASSWORD_H_

#ifdef __cplusplus // To adapt to C and C++ disable mangling naming
extern "C" {
#endif   //__cplusplus
//header code

#include "std_types.h"

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
extern uint8 eg_savedPass[PASSWORD_SIZE];
extern uint8 eg_masterPass[PASSWORD_SIZE];
extern uint8 eg_userPass[PASSWORD_SIZE];
/************************ functions declaration *****************************/
extern void Password_Init(void);
extern tPassword_state Password_getPass(uint8* ptrTopass);
extern tPassword_state Password_setNew(uint8* ptrTopass);
extern tCompare_state Password_compare( uint8* ptrTopass1, uint8* ptrTopass2);

#ifdef __cplusplus
}
#endif   //__cplusplus
#endif  //PASSWORD_H_
