#pragma once
#include <stdint.h>
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void sendUARTSync(USART_TypeDef *uart, unsigned char c);
unsigned char receiveUARTSync(USART_TypeDef *uart);
void sendUARTString(USART_TypeDef *uart, const char *pData);
void sendSPISync(SPI_TypeDef *spi, uint16_t pData);
void sendSPIAsync(SPI_TypeDef *spi, uint16_t pData);
uint16_t receiveSPISync(SPI_TypeDef *spi);

#ifdef __cplusplus
}
#endif