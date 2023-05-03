#include "bsp_GraySensor.h"
#include "main.h"
#include "adc.h"
#include "control.h"
#include "usart.h"

//用来巡线的 '灰度传感器' 和 用来检测是否放上药品的 '红外传感器' 的代码都放在这个.c文件里了

//u8 L2_Val, C3, C4, C5, R2_Val;
u8 C1=0,C2=0,C3=0,C4=0,C5=0,C6=0,C7=0;
int Line_Num;


u16 ADC_Value;
float ADC_Volt;
u8 Load_flag =0;  // 0表示还没转载药品，表示转载完毕，-1表示药品送

u8 LoadCount,NotLoadCount;

/*******************灰度传感器巡线*************/
//只要把速度环控制好，补偿值即可确定。  不同速度值需要不同的补偿值。  测试好一个最好的。  不同的转速要对应不同的补偿系数或数值
void Light_GoStraight_control(void)   //灰度巡线直行, 需要有个判断需要直行多长距离的函数    //补偿要到中间的传感器测到回到线上为止
{
	  //Get_Light_TTL();  
	
	//如果要更加精准的控制，只要有了巡线环，都需要对位置进行补偿。  可否直接将 g_lMotorPulseSigma  和g_lMotor2PulseSigma 进行数值加减？？
  
	//如果寻直线用的传感器可以读取模拟量的话，三个传感器都可以巡直线寻挺稳的


		if(C3 == 0 &&  C4 == 1 && C5 == 0 )  Line_Num =  0;
	  if(C3 == 0 &&  C4 == 1 && C5 == 1) Line_Num =  400;
	  if(C3 == 0 &&  C4 == 0 && C5 == 1 )  Line_Num =  500;
	  if(C3 == 1 &&  C4 == 0 && C5 == 0) Line_Num = -500;   
    if(C3 == 1 &&  C4 == 1 && C5 == 0) Line_Num = -400;	
	
//	 if(LineNumToggleFlag == 1)
//	 {
//		 Line_Num = - Line_Num;
//	 }
	
}

void   Openmv_Receive_Data(int16_t com_data)
{

	uint8_t i;
	static uint8_t RxCounter1 = 0;//计数
	//static uint16_t RxBuffer1[10]={0};
	static uint16_t RxBuffer1[13] = { 0 };
	static uint8_t RxState = 0;
	//static uint8_t RxFlag1 = 0;

	if (RxState == 0 && com_data == 0x2C)  //0x2c帧头
	{

		RxState = 1;
		RxBuffer1[RxCounter1++] = com_data;
		//HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
	}

	else if (RxState == 1 && com_data == 0x12)  //0x12帧头
	{
		//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
		RxState = 2;
		RxBuffer1[RxCounter1++] = com_data;
	}
	else if (RxState == 2)
	{

		RxBuffer1[RxCounter1++] = com_data;
		if (RxCounter1 >= 13 || com_data == 0x5B)       //RxBuffer1接受满了,接收数据结束
		{
			RxState = 3;
			//RxFlag1 = 1;

			C1 = RxBuffer1[RxCounter1 - 8];
			C2 = RxBuffer1[RxCounter1 - 7];
			C3 = RxBuffer1[RxCounter1 - 6];
			C4 = RxBuffer1[RxCounter1 - 5];
			C5 = RxBuffer1[RxCounter1 - 4];
			C6 = RxBuffer1[RxCounter1 - 3];
			C7 = RxBuffer1[RxCounter1 - 2];
			//               if(RxState==1)
			//          {
			//            HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
			//          }
			//          else if(RxState!=1&&RxState!=0)
			//          {
			//            HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
			//          }
		}
	}

	else if (RxState == 3)		//检测是否接受到结束标志
	{
		if (RxBuffer1[RxCounter1 - 1] == 0x5B)
		{

			//RxFlag1 = 0;
			RxCounter1 = 0;
			RxState = 0;

		}
		else   //接收错误
		{
			RxState = 0;
			RxCounter1 = 0;
			for (i = 0; i < 10; i++)
			{
				RxBuffer1[i] = 0x00;      //将存放数据数组清零
			}
		}
	}

	else   //接收异常
	{
		RxState = 0;
		RxCounter1 = 0;
		for (i = 0; i < 10; i++)
		{
			RxBuffer1[i] = 0x00;      //将存放数据数组清零
		}
	}
}

//把灰度传感器当作只输出高低电平。 高电平是识别到红线了。
//void Get_Light_TTL(void)  
//{
//	if(HAL_GPIO_ReadPin(L2_GPIO_Port, L2_Pin) == GPIO_PIN_SET){L2_Val = 1;} else {L2_Val = 0;}
//	if(HAL_GPIO_ReadPin(L1_GPIO_Port, L1_Pin) == GPIO_PIN_SET){C3 = 1;} else {C3 = 0;}
//	if(HAL_GPIO_ReadPin(M_GPIO_Port, M_Pin) == GPIO_PIN_SET)  {C4 = 1;} else {C4 = 0;}
//	if(HAL_GPIO_ReadPin(R1_GPIO_Port, R1_Pin) == GPIO_PIN_SET){C5 = 1;} else {C5 = 0;}
//	if(HAL_GPIO_ReadPin(R2_GPIO_Port, R2_Pin) == GPIO_PIN_SET){R2_Val = 1;} else {R2_Val = 0;}
//}



/********检测是否转载药品*************/
void LoadOrNot()
{
			HAL_ADC_Start(&hadc1);      
			if(HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
			{
				ADC_Value = HAL_ADC_GetValue(&hadc1);
				ADC_Volt =  (float)ADC_Value*3.3/4096.0;
			}
			
			if(ADC_Volt <= 1.6)
			{
				NotLoadCount = 0;
				
				LoadCount++;
				if(LoadCount > 3) //3次程序执行到这之后再次判断
				{
					Load_flag = 1;
				}
			}
			else if(ADC_Volt >  1.6)
			{
				LoadCount = 0;
				
				if(Load_flag == 1)    //必须先装载过药品的情况下才能判断是否拿走药品
				{
					NotLoadCount++;
					if(NotLoadCount > 3 )  //3次程序执行到这之后再次判断
					{
						Load_flag = 2;
					}
			  }
			}
}
