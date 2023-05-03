#ifndef __GRAYSENSOR_H
#define __GRAYSENSOR_H
#include "bsp_sys.h"
#include "stm32f1xx.h"

//extern u8 L2_Val, L1_Val, M_Val, R1_Val, R2_Val;
extern u8 C1,C2,C3,C4,C5,C6,C7;

extern u16 ADC_Value;
extern float ADC_Volt;
extern u8  Load_flag;


extern int Line_Num;


void LoadOrNot(void);

//void Get_Light_TTL(void);
void  Openmv_Receive_Data(int16_t data);
void Light_GoStraight_control(void);


#endif
