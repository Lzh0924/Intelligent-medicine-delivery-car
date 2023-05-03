#include "bsp_GraySensor.h"
#include "main.h"
#include "adc.h"
#include "control.h"
#include "usart.h"

//����Ѳ�ߵ� '�Ҷȴ�����' �� ��������Ƿ����ҩƷ�� '���⴫����' �Ĵ��붼�������.c�ļ�����

//u8 L2_Val, C3, C4, C5, R2_Val;
u8 C1=0,C2=0,C3=0,C4=0,C5=0,C6=0,C7=0;
int Line_Num;


u16 ADC_Value;
float ADC_Volt;
u8 Load_flag =0;  // 0��ʾ��ûת��ҩƷ����ʾת����ϣ�-1��ʾҩƷ��

u8 LoadCount,NotLoadCount;

/*******************�Ҷȴ�����Ѳ��*************/
//ֻҪ���ٶȻ����ƺã�����ֵ����ȷ����  ��ͬ�ٶ�ֵ��Ҫ��ͬ�Ĳ���ֵ��  ���Ժ�һ����õġ�  ��ͬ��ת��Ҫ��Ӧ��ͬ�Ĳ���ϵ������ֵ
void Light_GoStraight_control(void)   //�Ҷ�Ѳ��ֱ��, ��Ҫ�и��ж���Ҫֱ�ж೤����ĺ���    //����Ҫ���м�Ĵ������⵽�ص�����Ϊֹ
{
	  //Get_Light_TTL();  
	
	//���Ҫ���Ӿ�׼�Ŀ��ƣ�ֻҪ����Ѳ�߻�������Ҫ��λ�ý��в�����  �ɷ�ֱ�ӽ� g_lMotorPulseSigma  ��g_lMotor2PulseSigma ������ֵ�Ӽ�����
  
	//���Ѱֱ���õĴ��������Զ�ȡģ�����Ļ�������������������Ѳֱ��Ѱͦ�ȵ�


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
		if (RxCounter1 >= 13 || com_data == 0x5B)       //RxBuffer1��������,�������ݽ���
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

//�ѻҶȴ���������ֻ����ߵ͵�ƽ�� �ߵ�ƽ��ʶ�𵽺����ˡ�
//void Get_Light_TTL(void)  
//{
//	if(HAL_GPIO_ReadPin(L2_GPIO_Port, L2_Pin) == GPIO_PIN_SET){L2_Val = 1;} else {L2_Val = 0;}
//	if(HAL_GPIO_ReadPin(L1_GPIO_Port, L1_Pin) == GPIO_PIN_SET){C3 = 1;} else {C3 = 0;}
//	if(HAL_GPIO_ReadPin(M_GPIO_Port, M_Pin) == GPIO_PIN_SET)  {C4 = 1;} else {C4 = 0;}
//	if(HAL_GPIO_ReadPin(R1_GPIO_Port, R1_Pin) == GPIO_PIN_SET){C5 = 1;} else {C5 = 0;}
//	if(HAL_GPIO_ReadPin(R2_GPIO_Port, R2_Pin) == GPIO_PIN_SET){R2_Val = 1;} else {R2_Val = 0;}
//}



/********����Ƿ�ת��ҩƷ*************/
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
				if(LoadCount > 3) //3�γ���ִ�е���֮���ٴ��ж�
				{
					Load_flag = 1;
				}
			}
			else if(ADC_Volt >  1.6)
			{
				LoadCount = 0;
				
				if(Load_flag == 1)    //������װ�ع�ҩƷ������²����ж��Ƿ�����ҩƷ
				{
					NotLoadCount++;
					if(NotLoadCount > 3 )  //3�γ���ִ�е���֮���ٴ��ж�
					{
						Load_flag = 2;
					}
			  }
			}
}
