#include "openmv2.h"
#include "stm32f1xx.h"
#include "stdio.h"
#include "usart.h"
#include "bsp_led.h"
#include "adc.h"
#include "bsp_sys.h"

//int GetOpenmvDataCount2  = 0;
//uint8_t  Num2=0, LoR2 =0, Finded_flag2 = 0, FindTask2 = 0;     //()
//u8 LastNum;
//u8 NumCount = 0;

//u8 sendBuf[4];


//u8 RoomNum2, TargetNum2;
//u8 TASK=1;    //这个TASK可以传输给openmv，赋值openmv上的FindTask来控制openmv模板匹配的不同模式
u16 r1=0,r2=0;//输入两个数值

//char TargetRoom2 = 0;  //A, B, C, D, E, F, G, H;    //这八个字符对应着地图实际房间，里面的数字3—8会随机对应C-H

//u8  FindStartFlag2;
//u16 FindTimeCount2;

void k210_2_Receive_Data(int16_t com_data)
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
		if (RxCounter1 >= 8 || com_data == 0x5B)       //RxBuffer1接受满了,接收数据结束
		{
			RxState = 3;
			//RxFlag1 = 1;
			
			r1 = RxBuffer1[RxCounter1 - 3];
			r2 = RxBuffer1[RxCounter1 - 2];
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





//void SendDataToOpenmv2(void)
//{
//	u8 i;
//	//加上发送给openmv 的数据的代码 (帧头， 模板匹配模式选择标志位，模式2所需要匹配的数字，帧尾)   //不需要很高的发送频率
//		
//		
//		for(i = 0; i <= 4; i++)   //将TASK和TargetNum打包一次性发送给openmv
//		{
//			sprintf((char *)sendBuf, "*%d%d&", TASK, TargetNum2);
//				HAL_UART_Transmit(&huart3, sendBuf, sizeof(sendBuf), 1000);
//			  //YellowSignal_Toggle;
//		}
//		//greenLED_off;
//}


//等待openmv识别指定数字,并设置目标房间。 只有在复位后
//void SetTargetRoom2(void)
//{
//		//查寻目标病房号的缓冲区数据，跳转任务开始函数   //重启后第一次从openmv传来的数字即为目标房号
//		/*一开始识别目标房间号*/  
//		if(Finded_flag2 == 1)
//		{
//			/**滤波**/ //可以但没必要
////			if(NumCount == 0)   
////			{
////				LastNum = Num;
////			}
////			
////			else if(NumCount >= 2)  //至少连续两次一样的，才判断为识别到
////			{
////				if(LastNum == Num)
////				{
////					RoomNum = Num;    //有必要用多一个变量吗？要滤波才有必要,否则直接用Num就行
////					LastNum = 0;
////					NumCount = 0;
////				}
////				
////			}
////			NumCount++;
//			
//			 RoomNum2 = Num2;
//			 TASK = 2;    //openmv那边没把全部图片放上去前，不要给TASK赋值为2
//			
//		}
//		
//		
//		else if(Finded_flag2 == 0)
//		{
//			RoomNum2 = 0;   
//			LastNum = 0;
//			NumCount = 0;
//		}
//	
//		
//	 if(RoomNum2 ==  1) 
//	 {
//		 TargetRoom2 = 'A';
//		 
//		 
//	 }
//	 else if(RoomNum2 == 2)
//	 {
//		 TargetRoom2 = 'B';
//		 
//			
//	 }
//	 else if(RoomNum2 >= 3)  //不能else if(3 <= Num <= 8)
//	 {
//		 TargetRoom2 = 'E';  //与一车最好不一样
//		 
//	 }
//	 
//   switch(RoomNum2)
//		{
//			case 1:
//				TargetNum2 = 1;
//			break;
//			
//			case 2:
//				TargetNum2 = 2;
//			break;
//			
//			case 3:
//				TargetNum2 = 3;
//			break;
//			
//			case 4:
//				TargetNum2 = 4;
//			break;
//			
//			case 5:
//				TargetNum2 = 5;
//			break;
//			
//			case 6:
//				TargetNum2 = 6;
//			break;

//			case 7:
//				TargetNum2 = 7;
//			break;
//			
//			case 8:
//				TargetNum2 = 8;
//			break;	 		
//		}
//	 //识别到的数字是3-8， 默认先给  TargetRoom = RoomH
//	 //根据openmv识别到的数据,在送药的函数里面进行目标值的实时更改
//}
