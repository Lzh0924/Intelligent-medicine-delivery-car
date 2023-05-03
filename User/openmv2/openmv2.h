#ifndef __OPENMV2_H
#define __OPENMV2_H


#include "bsp_sys.h"

//extern u8 str_buff_FPX2[64];
//extern  uint8_t  Num2, LoR2, Finded_flag2, FindTask2;
//extern u8 RoomNum2, TargetNum2 ,TASK2;
//extern char TargetRoom2;
//extern int GetOpenmvDataCount2;

//extern u8 FindStartFlag2;
//extern u16 FindTimeCount2;

//extern uint8_t usart3_rxbuff;

extern u16 r1,r2;

void k210_2_Receive_Data(int16_t com_data);
//void SetTargetRoom2(void);
//void SendDataToOpenmv2(void);

#endif
