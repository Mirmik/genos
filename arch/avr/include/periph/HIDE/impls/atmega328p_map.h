#ifndef GENOS_AVR_MAP_ATMEGA2560_H
#define GENOS_AVR_MAP_ATMEGA2560_H

#include <periph/regs/usart.h>
#include <periph/regs/gpio.h>
#include <periph/regs/twi.h>
#include <periph/irqdefs.h>

#define GPIOB ((struct gpio_regs *)(0x23))
#define GPIOC ((struct gpio_regs *)(0x26))
#define GPIOD ((struct gpio_regs *)(0x29))

//#define TWI ((struct twi_regs*)(0xBD))

#define USART0 ((struct usart_regs *)(0xC0))

/*struct usart_data {
	struct usart_regs* regs;
	int8_t irqbase;
};

static const struct usart_data usart0_data = { USART0, ATMEGA_IRQ_U0RX };
static const struct usart_data usart1_data = { USART1, ATMEGA_IRQ_U1RX };
static const struct usart_data usart2_data = { USART2, ATMEGA_IRQ_U2RX };
*/

#endif