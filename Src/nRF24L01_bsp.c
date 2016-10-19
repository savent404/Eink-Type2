#include "stm32f1xx_hal.h"

extern SPI_HandleTypeDef hspi2;

#define NRF_CSN_GPIO_Port nRF24L01_CSN_GPIO_Port
#define NRF_CSN_Pin       nRF24L01_CSN_Pin
#define NRF_CE_GPIO_Port  nRF24L01_CE_GPIO_Port
#define NRF_CE_Pin        nRF24L01_CE_Pin
//#define NRF_CSN_GPIO_Port GPIOB
//#define NRF_CSN_Pin       GPIO_PIN_0
//#define NRF_CE_GPIO_Port  GPIOA
//#define NRF_CE_Pin        GPIO_PIN_4
#define NRF_IRQ_GPIO_Port nRF24L01_IRQ_GPIO_Port
#define NRF_IRQ_Pin       nRF24L01_IRQ_Pin

void Delay_ms(int ms){
    HAL_Delay(ms);
}

/* SPI 使能操作 */
void nRF24L01_CEN_opra(unsigned char set_1_reset_0){
    if(set_1_reset_0)
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(NRF_CSN_GPIO_Port, NRF_CSN_Pin, GPIO_PIN_RESET);
}

/* 发送使能操作 */
void nRF24L01_CE_opra(unsigned char set_1_reset_0){
    if(set_1_reset_0)
            HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(NRF_CE_GPIO_Port, NRF_CE_Pin, GPIO_PIN_RESET);
}
unsigned char nRF24L01_IRQ_read(void){
    return HAL_GPIO_ReadPin(NRF_IRQ_GPIO_Port, NRF_IRQ_Pin);
}
unsigned char nRF24L01_opra_IO(unsigned char val){
    unsigned char tx[1],rx[1];
    tx[0] = val;
    HAL_SPI_TransmitReceive(&hspi2, tx, rx, 1, 10);
    return *rx;
}
