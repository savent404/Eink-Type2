#ifndef _EPD_W21_H_
#define _EPD_W21_H_

#include "stm32f1xx_hal.h"

/* Usr define func */
#define SDA_H  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);	  //PD.10
#define SDA_L  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);	  //PD.10

#define SCLK_H HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);	  //PD.9
#define SCLK_L HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);

#define nCS_H  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET);	  //PD.8
#define nCS_L  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);

#define nDC_H  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET); //PE.15
#define nDC_L  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);

#define nRST_H HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_SET);   //PE.14
#define nRST_L HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_RESET);

#define nBUSY  HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13) //PE.13

#define nBS_H  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET);	 	//PE.11
#define nBS_L  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET);

#define VPP_H	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);			//PA4
#define VPP_L	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);			//PA4

static void Usr_SPI_Write(unsigned char C_0_D_1, unsigned char data);
static void Usr_SPI_OpraDelay(void);
extern void Delay_ms(int);
static void EPD_W21_CheckStatus(void);

/* BSP func */
static void SPI4W_WRITECOM(unsigned char CMD);
static void SPI4W_WRITEDATA(unsigned char DATA);
static void lut(void);
static void Load2Chip(const unsigned char *src);

/* public func */
/**
  * @Brief: Init IO
	*/
void EPD_W21_Init(void);

/**
  * @Para: \SleepEnable @1 there will be more option to shutdown EPD_W21, spend more time, less power.
	* @Para: \src_black   pointer to black picture
	* @Para: \src_red     pointer to red   picture
	*/
void EPD_W21_Display(const unsigned char *src_black, const unsigned char *src_red, unsigned char SleepEnable);
#endif
