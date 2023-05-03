/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "oled.h"
//#include "bsp_key.h"
#include "bsp_sys.h"
#include "bsp_delay.h"
#include "control.h"
#include "bsp_led.h"
//#include "menu.h"
//#include "Fire_protocol.h"

#include "bsp_pid.h"
#include "openmv.h"
#include "openmv2.h"
#include "bsp_GraySensor.h"

//#include "bsp_BlueTooth.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
	uint8_t usart1_rxbuff;
	uint8_t usart2_rxbuff;
	uint8_t usart3_rxbuff;
	uint8_t uart4_rxbuff;
	
	u8 Do_count = 0, Do2_count = 0;

	u16 showOLEDTime = 0;
	u16 SendTime = 0;
	
	u16 des = 0;

	extern unsigned char BMP3[];
	extern unsigned char BMP4[];
	
	
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	LED_Init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_TIM8_Init();
  MX_USART1_UART_Init();
  MX_UART4_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);   //??TIM3????????
  HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);   //??TIM8????????
  
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);       //Motor 2  
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
	
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);     //Motor1
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);  
	
	HAL_TIM_Base_Start_IT(&htim7);
	HAL_TIM_Base_Start_IT(&htim6);
	
	PID_param_init();
	set_motor2_enable();    //?????pid???????????PWM??
	set_motor_enable();  
	
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);    //??1????,???????,??????
	
  
	//protocol_init();       //????????????????, ????????
	HAL_UART_Receive_IT(&huart4,(void *)&uart4_rxbuff,1);
	HAL_UART_Receive_IT(&huart1,(void *)&usart1_rxbuff,1);
	HAL_UART_Receive_IT(&huart2,(void *)&usart2_rxbuff,1);
	HAL_UART_Receive_IT(&huart3,(void *)&usart3_rxbuff,1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		receiving_process();   //???????????,??????????PID??
		if (SendTime >= 21)
		{
			SendTime = 0;

//			SendDataToOpenmv();   //不能太快，否则会超过openmv的接收数据缓冲区
//			SendDataToOpenmv2();
			
		}

		if (showOLEDTime >= 200)
		{
			showOLEDTime = 0;
			//OLED_Display(Menu_Item);

			//			if(OneLoadFlag == 1)
			//			{
			//				BlueSignal_Toggle;
			//			}
						//printf("123");

						/****判断是否有放药品******/
			LoadOrNot();
		}



		/**************等待识别病房号**************/
		if (TASK == 1)
		{
			SetTargetRoom();

		}


		else if (TASK == 2)
		{
			//加上发送给openmv 的数据
			if (Load_flag == 1)    //运送药物
			{
				//近端病房，靠openmv规划	， 用来测试二车性能
				if (TargetRoom == 1)
				{
					switch (Do_count)
					{
					case 0:
						Do_count++;
						Car_go_new();   //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置

						break;

					case 1:
						if (Stop_Flag == 1)          //直行、转弯函数交替使用时，可以不手动将标志位清零
						{
							Do_count++;
							spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 2:
						if (Spin_succeed_flag == 1)
						{
							Do_count++;
							Car_go_new();      //Stop_Flag再次置1   +3是为了进停车区  
						}
						break;

					case	3:
						if (Stop_Flag == 1)
						{
							RedSignal_on;//点亮红色指示灯
							des = 1;
						}
						break;
					}
				}


				else if (TargetRoom == 2)
				{
					switch (Do_count)
					{
					case 0:
						Do_count++;

						Car_go_new();  //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置

						break;

					case 1:
						if (Stop_Flag == 1)
						{
							Do_count++;
							spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 2:
						if (Spin_succeed_flag == 1)
						{
							Do_count++;
							Car_go_new();;        //Stop_Flag再次置1   +3是为了进停车区  
						}
						break;

					case	3:
						if (Stop_Flag == 1)
						{

							RedSignal_on;
							des = 2;
						}
						break;
					}
				}

//				if (OneLoadFlag == 1)
//				{
//					//去中端送药，靠一车等待目标房号来确定，不用返回
				if (TargetRoom == 3 || 
						TargetRoom == 4 || 
						TargetRoom == 5 || 
						TargetRoom == 6 || 
						TargetRoom == 7 || 
						TargetRoom == 8 )
				{
					switch (Do_count)
					{
					case 0://第一段直线
						Do_count++;
						Car_go_new();    //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置

						break;

					case 1:
						if (Stop_Flag == 1)
						{
							Do_count++;
							Car_go(10);   //此时Spin_succeed_flag== 0 ,完成后自动置1

						}
						break;
						
					case 2://第二段直线
						Do_count++;
						Car_go_new();    //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置

						break;

					case 3:
						if(TargetRoom == l1)
							Do_count = 4;
						else if(TargetRoom == r1)
							Do_count = 7;
						else
							Do_count = 10;
						break;
						
					case 4:
						if (Stop_Flag == 1)          //直行、转弯函数交替使用时，可以不手动将标志位清零
						{
							Do_count++;
							spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 5:
						if (Spin_succeed_flag == 1)
						{
							Do_count++;
							Car_go_new();      //Stop_Flag再次置1   +3是为了进停车区  
						}
						break;

					case 6:
						if (Stop_Flag == 1)
						{
							RedSignal_on;//点亮红色指示灯
							des = 3;
						}
						break;
						
					case 7:
						if (Stop_Flag == 1)
						{
							Do_count++;
							spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 8:
						if (Spin_succeed_flag == 1)
						{
							Do_count++;
							Car_go_new();;        //Stop_Flag再次置1   +3是为了进停车区  
						}
						break;

					case 9:
						if (Stop_Flag == 1)
						{
							RedSignal_on;//点亮红色指示灯
							des = 4;
						}
						break;
					
					case 10:
						if (Stop_Flag == 1)
						{
							Do_count++;
							Car_go(10);   //此时Spin_succeed_flag== 0 ,完成后自动置1

						}
						break;
						
					case 11://第三段直线
						Do_count++;
						Car_go_new();    //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置

						break;	
						
					case 12:
						if(TargetRoom == l1 || TargetRoom == l2)
							Do_count = 13;
						else if(TargetRoom == r1 || TargetRoom == r2)
							Do_count = 22;
						break;
					
					case 13:
						if(Stop_Flag == 1)          //直行、转弯函数交替使用时，可以不手动将标志位清零
						{
							Do_count++;
							spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
					
					case 14://
						if (Spin_succeed_flag == 1)
						{
							Do_count++;
							Car_go_new();;        //Stop_Flag再次置1   +3是为了进停车区  
						}
						break;
						
					case 15:
						if(TargetRoom == l1)
							Do_count = 16;
						else if(TargetRoom == r1)
							Do_count = 19;
						break;
						
					case 16:
						if(Stop_Flag == 1)          //直行、转弯函数交替使用时，可以不手动将标志位清零
						{
							Do_count++;
							spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
						
					case 17:
						if (Spin_succeed_flag == 1)
						{
							Do_count++;
							Car_go_new();;        //Stop_Flag再次置1   +3是为了进停车区  
						}
						break;

					case 18:
						if (Stop_Flag == 1)
						{
							RedSignal_on;//点亮红色指示灯
							des = 5;
						}
						break;
						
					case 19:
						if(Stop_Flag == 1)          //直行、转弯函数交替使用时，可以不手动将标志位清零
						{
							Do_count++;
							spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
						
					case 20:
						if (Spin_succeed_flag == 1)
						{
							Do_count++;
							Car_go_new();;        //Stop_Flag再次置1   +3是为了进停车区  
						}
						break;

					case 21:
						if (Stop_Flag == 1)
						{
							RedSignal_on;//点亮红色指示灯
							des = 6;
						}
						break;
						
					case 22:
						if(Stop_Flag == 1)          //直行、转弯函数交替使用时，可以不手动将标志位清零
						{
							Do_count++;
							spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
					
					case 23://
						if (Spin_succeed_flag == 1)
						{
							Do_count++;
							Car_go_new();;        //Stop_Flag再次置1   +3是为了进停车区  
						}
						break;
						
					case 24:
						if(TargetRoom == l1)
							Do_count = 25;
						else if(TargetRoom == r1)
							Do_count = 28;
						break;
						
					case 25:
						if(Stop_Flag == 1)          //直行、转弯函数交替使用时，可以不手动将标志位清零
						{
							Do_count++;
							spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
						
					case 26:
						if (Spin_succeed_flag == 1)
						{
							Do_count++;
							Car_go_new();;        //Stop_Flag再次置1   +3是为了进停车区  
						}
						break;

					case 27:
						if (Stop_Flag == 1)
						{
							RedSignal_on;//点亮红色指示灯
							des = 7;
						}
						break;
						
					case 28:
						if(Stop_Flag == 1)          //直行、转弯函数交替使用时，可以不手动将标志位清零
						{
							Do_count++;
							spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
						
					case 29:
						if (Spin_succeed_flag == 1)
						{
							Do_count++;
							Car_go_new();;        //Stop_Flag再次置1   +3是为了进停车区  
						}
						break;

					case 30:
						if (Stop_Flag == 1)
						{
							RedSignal_on;//点亮红色指示灯
							des = 8;
						}
						break;

					}
				}


					
				
			}

			else if (Load_flag == 2)       // 只在近端才返回,用来测试二车控制效果  
			{
				if (des == 1)
				{
					switch (Do2_count)
					{
					case 0:
						Do2_count++;

						spin_Turn(back_180);
						RedSignal_off; //关闭红色指示灯

						break;


					case 1:
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();   //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
						}
						break;

					case 2:
						if (Stop_Flag == 1)
						{
							Do2_count++;
							spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 3:
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();   //Stop_Flag再次置1
						}
						break;

					case	4:
						if (Stop_Flag == 1)
						{

							GreenSignal_on; //点亮绿色指示灯
						}
						break;
					}

				}

				else if (des == 2)
				{
					switch (Do2_count)
					{
					case 0:
						Do2_count++;

						spin_Turn(back_180);
						RedSignal_off; //关闭红色指示灯

						break;


					case 1:
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();     //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
						}
						break;

					case 2:
						if (Stop_Flag == 1)
						{
							Do2_count++;
							spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 3:
						if (Spin_succeed_flag == 1)
						{


							Do2_count++;
							Car_go_new();    //Stop_Flag再次置1

						}
						break;

					case	4:
						if (Stop_Flag == 1)
						{

							GreenSignal_on; //点亮绿色指示灯

						}
						break;
					}
				}
			
				else if(des == 3)
				{
					switch (Do2_count)
					{
					case 0:
						Do2_count++;

						spin_Turn(back_180);
						RedSignal_off; //关闭红色指示灯

						break;


					case 1:
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();     //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
						}
						break;

					case 2:
						if (Stop_Flag == 1)
						{
							Do2_count++;
							spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 3:
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();    //Stop_Flag再次置1

						}
						break;

					case 4:
						if (Stop_Flag == 1)
						{
							Do_count++;
							Car_go(10);   //此时Spin_succeed_flag== 0 ,完成后自动置1

						}
						break;
					
					case 5:
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();    //Stop_Flag再次置1

						}
						break;
						
					case	6:
						if (Stop_Flag == 1)
						{
							GreenSignal_on; //点亮绿色指示灯
						}
						break;
					}
				}
				
				
				else if(des == 4)
				{
					switch (Do2_count)
					{
					case 0:
						Do2_count++;

						spin_Turn(back_180);
						RedSignal_off; //关闭红色指示灯

						break;


					case 1:
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();     //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
						}
						break;

					case 2:
						if (Stop_Flag == 1)
						{
							Do2_count++;
							spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 3:
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();    //Stop_Flag再次置1

						}
						break;

					case 4:
						if (Stop_Flag == 1)
						{
							Do_count++;
							Car_go(10);   //此时Spin_succeed_flag== 0 ,完成后自动置1

						}
						break;
					
					case 5:
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();    //Stop_Flag再次置1

						}
						break;
						
					case	6:
						if (Stop_Flag == 1)
						{
							GreenSignal_on; //点亮绿色指示灯
						}
						break;
					}
				}
					
				
			
				
			
			
			
				else if(des == 5)
				{
					switch (Do2_count)
					{
					case 0:
						Do2_count++;

						spin_Turn(back_180);
						RedSignal_off; //关闭红色指示灯

						break;


					case 1://走1
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();     //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
						}
						break;

					case 2:
						if (Stop_Flag == 1)
						{
							Do2_count++;
							spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 3://走2
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();    //Stop_Flag再次置1

						}
						break;

					case 4:
						if (Stop_Flag == 1)
						{
							Do2_count++;
							spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 5://走3
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();    //Stop_Flag再次置1
						}
						break;
						
					case 6:
						if (Stop_Flag == 1)
						{
							Do_count++;
							Car_go(10);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
					
					case 7://走4
							Do2_count++;
							Car_go_new();
						break;
						
					case 8:
						if (Stop_Flag == 1)
						{
							Do_count++;
							Car_go(10);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
					
					case 9://走5
							Do2_count++;
							Car_go_new();
						break;
					
					case 10:
						if (Stop_Flag == 1)
						{
							GreenSignal_on; //点亮绿色指示灯
						}
						break;
					}
				}
				
				else if(des == 6)
				{
					switch (Do2_count)
					{
					case 0:
						Do2_count++;

						spin_Turn(back_180);
						RedSignal_off; //关闭红色指示灯

						break;


					case 1://走1
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();     //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
						}
						break;

					case 2:
						if (Stop_Flag == 1)
						{
							Do2_count++;
							spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 3://走2
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();    //Stop_Flag再次置1

						}
						break;

					case 4:
						if (Stop_Flag == 1)
						{
							Do2_count++;
							spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 5://走3
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();    //Stop_Flag再次置1
						}
						break;
						
					case 6:
						if (Stop_Flag == 1)
						{
							Do_count++;
							Car_go(10);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
					
					case 7://走4
							Do2_count++;
							Car_go_new();
						break;
						
					case 8:
						if (Stop_Flag == 1)
						{
							Do_count++;
							Car_go(10);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
					
					case 9://走5
							Do2_count++;
							Car_go_new();
						break;
					
					case 10:
						if (Stop_Flag == 1)
						{
							GreenSignal_on; //点亮绿色指示灯
						}
						break;
					}
				}
				
				else if(des == 7)
				{
					switch (Do2_count)
					{
					case 0:
						Do2_count++;

						spin_Turn(back_180);
						RedSignal_off; //关闭红色指示灯

						break;


					case 1://走1
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();     //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
						}
						break;

					case 2:
						if (Stop_Flag == 1)
						{
							Do2_count++;
							spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 3://走2
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();    //Stop_Flag再次置1

						}
						break;

					case 4:
						if (Stop_Flag == 1)
						{
							Do2_count++;
							spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 5://走3
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();    //Stop_Flag再次置1
						}
						break;
						
					case 6:
						if (Stop_Flag == 1)
						{
							Do_count++;
							Car_go(10);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
					
					case 7://走4
							Do2_count++;
							Car_go_new();
						break;
						
					case 8:
						if (Stop_Flag == 1)
						{
							Do_count++;
							Car_go(10);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
					
					case 9://走5
							Do2_count++;
							Car_go_new();
						break;
					
					case 10:
						if (Stop_Flag == 1)
						{
							GreenSignal_on; //点亮绿色指示灯
						}
						break;
					}
				}
				
				else if(des == 8)
				{
					switch (Do2_count)
					{
					case 0:
						Do2_count++;

						spin_Turn(back_180);
						RedSignal_off; //关闭红色指示灯

						break;


					case 1://走1
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();     //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
						}
						break;

					case 2:
						if (Stop_Flag == 1)
						{
							Do2_count++;
							spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 3://走2
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();    //Stop_Flag再次置1

						}
						break;

					case 4:
						if (Stop_Flag == 1)
						{
							Do2_count++;
							spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;

					case 5://走3
						if (Spin_succeed_flag == 1)
						{
							Do2_count++;
							Car_go_new();    //Stop_Flag再次置1
						}
						break;
						
					case 6:
						if (Stop_Flag == 1)
						{
							Do_count++;
							Car_go(10);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
					
					case 7://走4
							Do2_count++;
							Car_go_new();
						break;
						
					case 8:
						if (Stop_Flag == 1)
						{
							Do_count++;
							Car_go(10);   //此时Spin_succeed_flag== 0 ,完成后自动置1
						}
						break;
					
					case 9://走5
							Do2_count++;
							Car_go_new();
						break;
					
					case 10:
						if (Stop_Flag == 1)
						{
							GreenSignal_on; //点亮绿色指示灯
						}
						break;
					}
				}
				
			}

		}
		
		
		
		
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  uint16_t tem1;
  if(huart->Instance==UART4)
  {
    tem1=uart4_rxbuff;
    Openmv_Receive_Data(tem1);
  }	
	HAL_UART_Receive_IT(&huart4,(void *)&uart4_rxbuff,1);
	
	uint16_t tem2;
  if(huart->Instance==USART2)
  {
    tem2=usart2_rxbuff;
    k210_1_Receive_Data(tem2);
  }	
	HAL_UART_Receive_IT(&huart2,(void *)&usart2_rxbuff,1);
	
	uint16_t tem3;
  if(huart->Instance==USART3)
  {
    tem3=usart3_rxbuff;
    k210_2_Receive_Data(tem3);
  }	
	HAL_UART_Receive_IT(&huart3,(void *)&usart3_rxbuff,1);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
