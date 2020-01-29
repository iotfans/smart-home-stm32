#ifndef __OLED_H_
#define __OLED_H_

#include "stm32l4xx_hal.h"
#include "main.h"

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据    

#define OLED_DC_Clr()	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET)//DC
#define OLED_DC_Set()	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET)

#define OLED_RST_Clr()	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_RESET)//RES
#define OLED_RST_Set()	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_SET)

//OLED控制用函数
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Draw(uint8_t byte);

void OLED_ShowChar(uint16_t x,uint16_t y, uint8_t num, uint8_t size);
void OLED_ShowString(uint16_t x,uint16_t y,char *p, uint8_t size);
void OLED_ShowInt32Num(uint16_t x,uint16_t y, int32_t num, uint8_t len, uint8_t size);
void OLED_DrawFont16(uint16_t x, uint16_t y, char *s);
void OLED_DrawFont32(uint16_t x, uint16_t y, char *s);
void OLED_Show_Str(uint16_t x, uint16_t y, char *str,uint8_t size);

#endif

