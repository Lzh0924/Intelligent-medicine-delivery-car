#include "bsp_led.h"
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOA_CLK_ENABLE();           	//??GPIOA/E??
	__HAL_RCC_GPIOB_CLK_ENABLE(); 
	__HAL_RCC_GPIOE_CLK_ENABLE();  	
	
    GPIO_Initure.Pin=GPIO_PIN_5; 				//PB5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//????
    GPIO_Initure.Pull=GPIO_PULLUP;          	//??
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    //??
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_5; 				//PE5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//????
    GPIO_Initure.Pull=GPIO_PULLUP;          	//??
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    //??
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
	
	GPIO_Initure.Pin=GPIO_PIN_11 | GPIO_PIN_12;
	GPIO_Initure.Pull=GPIO_PULLDOWN; 
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	  GPIO_Initure.Pin=GPIO_PIN_15;
	  GPIO_Initure.Pull=GPIO_PULLDOWN; 
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);	//PB5?0,????????
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET);	//PE5?0,????????
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);
}
