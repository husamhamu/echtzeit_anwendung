/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <usb_device.h>
#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_cortex.h>
#include <stm32f429xx.h>

// ----------------------------------------------------------------------------

// The external clock frequency is specified as a preprocessor definition
// passed to the compiler via a command line option (see the 'C/C++ General' ->
// 'Paths and Symbols' -> the 'Symbols' tab, if you want to change it).
// The value selected during project creation was HSE_VALUE=8000000.
//
// The code to set the clock is at the end.
//
// Note1: The default clock settings assume that the HSE_VALUE is a multiple
// of 1MHz, and try to reach the maximum speed available for the
// board. It does NOT guarantee that the required USB clock of 48MHz is
// available. If you need this, please update the settings of PLL_M, PLL_N,
// PLL_P, PLL_Q to match your needs.
//
// Note2: The external memory controllers are not enabled. If needed, you
// have to define DATA_IN_ExtSRAM or DATA_IN_ExtSDRAM and to configure
// the memory banks in system/src/cmsis/system_stm32f4xx.c to match your needs.

// ----------------------------------------------------------------------------

// Forward declarations.

void
__initialize_hardware(void);

void
SystemClock_Config(void);

void waitForFullControl();
void setupClockTree();
void setupGPIOs();
void setupUART2();
void setupSPI3();
void setupSPI5();
void setupTIM1();
void setupTIM2();
void setupTIM8();

void setupAdditionalHardware() {
	HAL_Delay(10);

	setupGPIOs();
	setupUART2();
	setupSPI3();
	setupSPI5();
    MX_USB_DEVICE_Init(); // Activate usb driver
	setupTIM1(); // commented out, because it sets up interrupts
//	setupTIM8();

//	MX_USB_DEVICE_Init(); // commented out, because it will read initialized data before measurement_logger is implemented
}

void setupClockTree() {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	// enable SPI3 clock
	__HAL_RCC_SPI3_CLK_ENABLE();
	// enable USART2 clock
	__HAL_RCC_USART2_CLK_ENABLE();
	// enable SPI5 clock
	__HAL_RCC_SPI5_CLK_ENABLE();
	// enable TIM1 clock
	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();
}

void setupGPIOs() {
	GPIOG -> MODER |= GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0;
	GPIOD -> MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0 | GPIO_MODER_MODER12_0;
	GPIOD -> PUPDR |= GPIO_PUPDR_PUPDR5_0 | GPIO_PUPDR_PUPDR6_0;
	GPIOD -> AFR[0] |= GPIO_AF7_USART2 << 20 | GPIO_AF7_USART2 << 24;
	// SPI3
	GPIOC -> MODER |= GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_0;
	GPIOC -> AFR[1] |= GPIO_AF6_SPI3 << 8 | GPIO_AF6_SPI3 << 12 | GPIO_AF6_SPI3 << 16;
	// SPI5
	GPIOF -> MODER |= GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_1 | GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1;
	GPIOF -> AFR[0] |= GPIO_AF5_SPI5 << 28;
	GPIOF -> AFR[1] |= GPIO_AF5_SPI5 | GPIO_AF5_SPI5 << 4;
	// TIM1
	GPIOE->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1 | GPIO_MODER_MODER12_1 |GPIO_MODER_MODER13_1;
	GPIOE->AFR[1] |= GPIO_AF1_TIM1 | GPIO_AF1_TIM1<<4 | GPIO_AF1_TIM1 <<8 | GPIO_AF1_TIM1 <<12 | GPIO_AF1_TIM1<<16 |GPIO_AF1_TIM1<<20;
}

void setupUART2() {
	USART2 -> BRR = 365; //168.000.000/ (4*115200) ~ 365
	USART2 -> CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
	GPIOD->BSRR = GPIO_BSRR_BR_8;
}

void setupSPI3() {
	SPI3 -> CR1 |= SPI_CR1_MSTR | SPI_CR1_BR_0  | SPI_CR1_BR_1 | SPI_CR1_SPE | SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_DFF;
	GPIOD->BSRR = GPIO_BSRR_BS_12;
	GPIOC->BSRR = GPIO_BSRR_BS_13;
	GPIOD->BSRR = GPIO_BSRR_BS_9;

}

void setupSPI5() {
	SPI5 -> CR1 |= SPI_CR1_MSTR | SPI_CR1_BR_0  | SPI_CR1_BR_1 | SPI_CR1_SPE | SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_DFF;
	GPIOF->BSRR = GPIO_BSRR_BS_6;
}

void setupTIM1() { // needs to be commented in above, when timer is properly configured!

	NVIC->ISER[0] |= 1 << TIM1_UP_TIM10_IRQn;
	TIM1->CR1 |= TIM_CR1_CEN| TIM_CR1_CMS_0; // Enable and center aligned mode 1
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2; // Set OC1M to PWM mode 1 for Channel // Set OC2M to PWM mode 1 for Channel 2
	TIM1->DIER |= TIM_DIER_UIE; // Enable the update interrupt event 
	TIM1->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2; // Set OC3M to PWM mode 1 for Channel 3
	TIM1->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC1NE | // Channel 1
			   TIM_CCER_CC2E | TIM_CCER_CC2NE | // Channel 2
			   TIM_CCER_CC3E | TIM_CCER_CC3NE); // Channel 3
	TIM1->PSC = 0;
	TIM1->ARR = 21000;
	TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_DTG_7 | TIM_BDTR_DTG_6 | TIM_BDTR_DTG_3 | TIM_BDTR_DTG_1; // Enable Main Output
			   //(0b110 << 5) | // Set DTG[7:5] = 110
			   //0b1010);           // Set DTG[4:0] = 1010
}


void setupTIM8() {

}

// ----------------------------------------------------------------------------

// This is the application hardware initialisation routine,
// redefined to add more inits.
//
// Called early from _start(), right after data & bss init, before
// constructors.
//
// After Reset the Cortex-M processor is in Thread mode,
// priority is Privileged, and the Stack is set to Main.
//
// Warning: The HAL requires the system timer, running at 1000 Hz
// and calling HAL_IncTick().

void
__initialize_hardware(void) {
	// Initialise the HAL Library; it must be the first function
	// to be executed before the call of any HAL function.
	HAL_Init();

	// Enable HSE Oscillator and activate PLL with HSE as source
	SystemClock_Config();

	// Call the CSMSIS system clock routine to store the clock frequency
	// in the SystemCoreClock global RAM location.
	SystemCoreClockUpdate();

	setupClockTree();

	waitForFullControl();

	setupAdditionalHardware();
}

// ----------------------------------------------------------------------------

/**
 * @brief  System Clock Configuration
 * @param  None
 * @retval None
 */
void
__attribute__((weak))
SystemClock_Config(void) {
	// Enable Power Control clock
			__PWR_CLK_ENABLE();

	// The voltage scaling allows optimizing the power consumption when the
	// device is clocked below the maximum system frequency, to update the
	// voltage scaling value regarding system frequency refer to product
	// datasheet.
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

//#warning "Please check if the SystemClock_Config() settings match your board!"
	// Comment out the warning after checking and updating.

	RCC_OscInitTypeDef RCC_OscInitStruct;

#if defined(HSE_VALUE) && (HSE_VALUE != 0)
	// Enable HSE Oscillator and activate PLL with HSE as source.
	// This is tuned for STM32F4-DISCOVERY; update it for your board.
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	// This assumes the HSE_VALUE is a multiple of 1 MHz. If this is not
	// your case, you have to recompute these PLL constants.
	RCC_OscInitStruct.PLL.PLLM = (HSE_VALUE / 1000000u);
#else
	// Use HSI and activate PLL with HSI as source.
	// This is tuned for NUCLEO-F411; update it for your board.
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	// 16 is the average calibration value, adjust for your own board.
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	// This assumes the HSI_VALUE is a multiple of 1 MHz. If this is not
	// your case, you have to recompute these PLL constants.
	RCC_OscInitStruct.PLL.PLLM = (HSI_VALUE/1000000u);
#endif

	RCC_OscInitStruct.PLL.PLLN = 336;
#if defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE)
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4; /* 84 MHz */
#elif defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx)
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; /* 168 MHz */
#elif defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx)
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; /* 168 MHz */
#elif defined(STM32F446xx)
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; /* 168 MHz */
#else
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4; /* 84 MHz, conservative */
#endif
	RCC_OscInitStruct.PLL.PLLQ = 7; /* To make USB work. */
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	// Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	// clocks dividers
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
								   | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
#if defined(STM32F401xC) || defined(STM32F401xE) || defined(STM32F411xE)
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
#else
	// This is expected to work for most large cores.
	// Check and update it for your own configuration.
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
#endif

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

void waitForFullControl() {
	GPIOG->MODER = GPIO_MODER_MODER4_0 // LED3
				   | GPIO_MODER_MODER3_0 // LED2
				   | GPIO_MODER_MODER2_0; // LED1


	long inner = 0;
	unsigned int outer = 0;

	while ((GPIOD->IDR & GPIO_IDR_IDR_15) == 0) {
		if (inner++ > 100000) {
			inner = 0;
			outer = (outer + 1) % 8;
		}

		GPIOG->ODR = (GPIOG->ODR & ~0x1Cu) | ((0x7u & ~outer) << 2u);
	}

	GPIOG->BSRR = GPIO_BSRR_BS_4 | GPIO_BSRR_BS_3 | GPIO_BSRR_BS_2;
}

// ----------------------------------------------------------------------------
