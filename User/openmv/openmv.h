#ifndef __OPENMV_H
#define __OPENMV_H


#include "bsp_sys.h"

//extern u8 str_buff_FPX[64];
//extern  uint8_t  Num, LoR, Finded_flag, FindTask;
//extern u8 RoomNum, TargetNum ,TASK;
//extern char TargetRoom;
//extern int GetOpenmvDataCount;

//extern u8 FindStartFlag;
//extern u16 FindTimeCount;

//extern uint8_t usart2_rxbuff;

extern u16 TargetRoom;
extern u8 TASK;
extern u16 l1,l2;

void k210_1_Receive_Data(int16_t com_data);
void SetTargetRoom(void);
//void SendDataToOpenmv(void);

#endif
