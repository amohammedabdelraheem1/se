
#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "std_types.h" // Include the standard types header file
typedef struct
{
  uint8_t Minute;
  uint8_t Hour;
  uint8_t Day;
  uint8_t Month;
  uint8_t Year;
} tDate;  //CurrentDate ,SyncDate
typedef enum {
  INVALID_STATE,
  VALID_STATE
} tUserState;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* COMMON_TYPES_H_ */
