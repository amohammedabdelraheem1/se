/**
 * Module: MODE
 * Description: Header file for mode definitions
 * Author: Ahmed Mohammed Abdelraheem
 */

#ifndef MODE_H_
#define MODE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
typedef enum {
  IDEAL,
  PROGRAMMING_CARD,
  PROGRAMMING_PASS,
  LIGHT_ONLY,
  ALL_OUTPUTS
} tMode;


void MODE_Init(void);
void MODE_Update(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* MODE_H_ */
