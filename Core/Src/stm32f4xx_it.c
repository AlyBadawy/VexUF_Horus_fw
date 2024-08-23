#include "main.h"
#include "stm32f4xx_it.h"

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;
extern RTC_HandleTypeDef hrtc;
extern DMA_HandleTypeDef hdma_sdio;
extern SD_HandleTypeDef hsd;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim10;
extern TIM_HandleTypeDef htim11;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart6_rx;
extern DMA_HandleTypeDef hdma_usart6_tx;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;
extern TIM_HandleTypeDef htim1;

void NMI_Handler(void) {
  HAL_RCC_NMI_IRQHandler();
   while(1);
}

void HardFault_Handler(void) {
  while(1);
}

void MemManage_Handler(void) {
  while(1);
}

void BusFault_Handler(void) {
  while(1);
}

void UsageFault_Handler(void) {
  while(1);
}


void SVC_Handler(void) { }
void DebugMon_Handler(void) { }
void PendSV_Handler(void) {}
void SysTick_Handler(void) { }

void TAMP_STAMP_IRQHandler(void) {
  HAL_RTCEx_TamperTimeStampIRQHandler(&hrtc);
}

void RCC_IRQHandler(void) { }

void ADC_IRQHandler(void) {
  HAL_ADC_IRQHandler(&hadc1);
}

void TIM1_BRK_TIM9_IRQHandler(void) {
  HAL_TIM_IRQHandler(&htim9);
}

void TIM1_UP_TIM10_IRQHandler(void) {
  HAL_TIM_IRQHandler(&htim1);
  HAL_TIM_IRQHandler(&htim10);
}

void TIM1_TRG_COM_TIM11_IRQHandler(void) {
  HAL_TIM_IRQHandler(&htim11);
}

void TIM3_IRQHandler(void) {
  HAL_TIM_IRQHandler(&htim3);
}

void TIM4_IRQHandler(void) {
  HAL_TIM_IRQHandler(&htim4);
}


void SPI1_IRQHandler(void) {
  HAL_SPI_IRQHandler(&hspi1);
}

void USART1_IRQHandler(void) {
  HAL_UART_IRQHandler(&huart1);
}

void RTC_Alarm_IRQHandler(void) {
  HAL_RTC_AlarmIRQHandler(&hrtc);
}

void SDIO_IRQHandler(void) {
  HAL_SD_IRQHandler(&hsd);
}

void TIM5_IRQHandler(void) {
  HAL_TIM_IRQHandler(&htim5);
}

void DMA2_Stream1_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_usart6_rx);
}

void DMA2_Stream2_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
}


void DMA2_Stream3_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_sdio);
}


void DMA2_Stream4_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_adc1);
}


void OTG_FS_IRQHandler(void) {
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}


void DMA2_Stream6_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_usart6_tx);
}


void DMA2_Stream7_IRQHandler(void) {
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

void USART6_IRQHandler(void) {
  HAL_UART_IRQHandler(&huart6);
}


void FPU_IRQHandler(void) { }