
#ifndef RTC_H
#define RTC_H

// Preprocessor directive to ensure C++ compatibility
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "common_types.h" // Include the common types header file

void RTC_Init(void);
void RTC_Set(tDate date);
void RTC_Update(void);
bool RTC_IsTimeValid(tDate date);
uint32_t UnixTime(tDate* date);
#endif // RTC_H
// End of C++ compatibility block
#ifdef __cplusplus
}
#endif // __cplusplus
