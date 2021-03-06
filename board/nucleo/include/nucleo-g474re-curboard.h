#ifndef NUCLEO_BOARDS
#define NUCLEO_BOARDS

#include <sys/cdefs.h>
#include <drivers/gpio/pin.h>
#include <drivers/serial/stm32_usart.h>

#define BOARD_HSE_FREQ 24000000
#define SYSLED_GPIO GPIOA
#define SYSLED_MASK (1<<5)
#define SYSUART LPUART1
#define SYSUART_IRQ LPUART1_IRQn
#define SYSUART_RX_GPIO GPIOA
#define SYSUART_TX_GPIO GPIOA
#define SYSUART_RX_PIN 2
#define SYSUART_TX_PIN 3
#define SYSUART_TX_AF 12
#define SYSUART_RX_AF 12

#endif