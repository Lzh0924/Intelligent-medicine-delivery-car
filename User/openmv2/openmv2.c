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
//u8 TASK=1;    //���TASK���Դ����openmv����ֵopenmv�ϵ�FindTask������openmvģ��ƥ��Ĳ�ͬģʽ
u16 r1=0,r2=0;//����������ֵ

//char TargetRoom2 = 0;  //A, B, C, D, E, F, G, H;    //��˸��ַ���Ӧ�ŵ�ͼʵ�ʷ��䣬���������3��8�������ӦC-H

//u8  FindStartFlag2;
//u16 FindTimeCount2;

void k210_2_Receive_Data(int16_t com_data)
{
		uint8_t i;
	static uint8_t RxCounter1 = 0;//����
	//static uint16_t RxBuffer1[10]={0};
	static uint16_t RxBuffer1[13] = { 0 };
	static uint8_t RxState = 0;
	//static uint8_t RxFlag1 = 0;

	if (RxState == 0 && com_data == 0x2C)  //0x2c֡ͷ
	{

		RxState = 1;
		RxBuffer1[RxCounter1++] = com_data;
		//HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
	}

	else if (RxState == 1 && com_data == 0x12)  //0x12֡ͷ
	{
		//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
		RxState = 2;
		RxBuffer1[RxCounter1++] = com_data;
	}
	else if (RxState == 2)
	{

		RxBuffer1[RxCounter1++] = com_data;
		if (RxCounter1 >= 8 || com_data == 0x5B)       //RxBuffer1��������,�������ݽ���
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

	else if (RxState == 3)		//����Ƿ���ܵ�������־
	{
		if (RxBuffer1[RxCounter1 - 1] == 0x5B)
		{

			//RxFlag1 = 0;
			RxCounter1 = 0;
			RxState = 0;

		}
		else   //���մ���
		{
			RxState = 0;
			RxCounter1 = 0;
			for (i = 0; i < 10; i++)
			{
				RxBuffer1[i] = 0x00;      //�����������������
			}
		}
	}

	else   //�����쳣
	{
		RxState = 0;
		RxCounter1 = 0;
		for (i = 0; i < 10; i++)
		{
			RxBuffer1[i] = 0x00;      //�����������������
		}
	}
}





//void SendDataToOpenmv2(void)
//{
//	u8 i;
//	//���Ϸ��͸�openmv �����ݵĴ��� (֡ͷ�� ģ��ƥ��ģʽѡ���־λ��ģʽ2����Ҫƥ������֣�֡β)   //����Ҫ�ܸߵķ���Ƶ��
//		
//		
//		for(i = 0; i <= 4; i++)   //��TASK��TargetNum���һ���Է��͸�openmv
//		{
//			sprintf((char *)sendBuf, "*%d%d&", TASK, TargetNum2);
//				HAL_UART_Transmit(&huart3, sendBuf, sizeof(sendBuf), 1000);
//			  //YellowSignal_Toggle;
//		}
//		//greenLED_off;
//}


//�ȴ�openmvʶ��ָ������,������Ŀ�귿�䡣 ֻ���ڸ�λ��
//void SetTargetRoom2(void)
//{
//		//��ѰĿ�겡���ŵĻ��������ݣ���ת����ʼ����   //�������һ�δ�openmv���������ּ�ΪĿ�귿��
//		/*һ��ʼʶ��Ŀ�귿���*/  
//		if(Finded_flag2 == 1)
//		{
//			/**�˲�**/ //���Ե�û��Ҫ
////			if(NumCount == 0)   
////			{
////				LastNum = Num;
////			}
////			
////			else if(NumCount >= 2)  //������������һ���ģ����ж�Ϊʶ��
////			{
////				if(LastNum == Num)
////				{
////					RoomNum = Num;    //�б�Ҫ�ö�һ��������Ҫ�˲����б�Ҫ,����ֱ����Num����
////					LastNum = 0;
////					NumCount = 0;
////				}
////				
////			}
////			NumCount++;
//			
//			 RoomNum2 = Num2;
//			 TASK = 2;    //openmv�Ǳ�û��ȫ��ͼƬ����ȥǰ����Ҫ��TASK��ֵΪ2
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
//	 else if(RoomNum2 >= 3)  //����else if(3 <= Num <= 8)
//	 {
//		 TargetRoom2 = 'E';  //��һ����ò�һ��
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
//	 //ʶ�𵽵�������3-8�� Ĭ���ȸ�  TargetRoom = RoomH
//	 //����openmvʶ�𵽵�����,����ҩ�ĺ����������Ŀ��ֵ��ʵʱ����
//}
