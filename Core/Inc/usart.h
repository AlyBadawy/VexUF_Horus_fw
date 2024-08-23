
#ifndef __USART_H__
#define __USART_H__

#include "main.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

void MX_USART1_UART_Init(void);
void MX_USART6_UART_Init(void);

#endif /* __USART_H__ */

