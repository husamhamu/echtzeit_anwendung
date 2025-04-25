#include "send_receive.h"

/* custom sendUART void */
void sendUARTSync(USART_TypeDef * const uart, unsigned char c){
		while( (uart->SR & (1u<<7u)) == 0u );
		uart->DR = c & 0xFFu;
}

/* custom receiveUART void */
unsigned char receiveUARTSync(USART_TypeDef * const uart){
		while( !(uart->SR & (1u<<5u)) );
		return (unsigned char) uart->DR & 0xFFu;
}

/* custom sendUART void */
void sendUARTString(USART_TypeDef * const uart, const char * pData){
	if( pData == 0 )
	{
		return;
	}
	while(*pData){
		sendUARTSync(uart, *pData++);
	}
}

/* custom sendSPI void */
void sendSPISync(SPI_TypeDef * const spi, uint16_t pData){
	while( (spi->SR & (1u<<1u)) == 0u );
	spi->DR = pData;
	while( (spi->SR & (1u<<1u)) == 0u );
	while( (spi->SR & (1u<<7u)) != 0u );
}

void sendSPIAsync(SPI_TypeDef * const spi, uint16_t pData) {
	while( (spi->SR & SPI_SR_TXE) == 0u );
	spi->DR = pData;
}

/* custom receiveSPI void */
uint16_t receiveSPISync(SPI_TypeDef * const spi){
	while( (spi->SR & SPI_SR_TXE) == 0u );
	spi->DR = 0xAAAA;
	while( (spi->SR & SPI_SR_RXNE) == 0u);
	return spi->DR & (uint16_t)0xFFFFu;
}