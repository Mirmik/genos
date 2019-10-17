#include <drivers/serial/stm32_usart.h>
#include <igris/util/retype.h>

int stm32_usart_device::enable(int en)
{
	stm32_usart_enable(regs, en);
	return 0;
}

int stm32_usart_device::ctrirqs(uint8_t cmd)
{	
	switch (cmd)
	{
		case UART_CTRIRQS_TXON:
			stm32_usart_txirq_enable(regs, true);
			return 0;

		case UART_CTRIRQS_TXOFF:
			stm32_usart_txirq_enable(regs, false);
			return 0;
		
		case UART_CTRIRQS_RXON:
			stm32_usart_rxirq_enable(regs, true);
			return 0;
		
		case UART_CTRIRQS_RXOFF:
			stm32_usart_rxirq_enable(regs, false);
			return 0;
		
		case UART_CTRIRQS_TCON:
			stm32_usart_tcirq_enable(regs, true);
			return 0;
		
		case UART_CTRIRQS_TCOFF:
			stm32_usart_tcirq_enable(regs, false);
			return 0;
	}

	return -1;

}

int stm32_usart_device::recvbyte()
{
	return stm32_usart_getc(regs);

}

int stm32_usart_device::sendbyte(int symbol)
{
	return stm32_usart_putc(regs, symbol);
}

int stm32_usart_device::cantx()
{
	return stm32_usart_room(regs);
}

int stm32_usart_device::hasrx()
{
	return stm32_usart_avail(regs);
}

static void _irqhandler(void* priv)
{
	struct stm32_usart_device * dev = (struct stm32_usart_device *) priv;
	struct usart_regs * regs = dev->regs;

	if (stm32_rxirq_status(regs)){
		dev->handler(dev->handarg, UART_IRQCODE_RX);
	}

	else if (stm32_txirq_status(regs)){
		dev->handler(dev->handarg, UART_IRQCODE_TX);
	}

	else if (stm32_tcirq_status(regs)) {
		//dev.handler(dev.handarg, UART_IRQCODE_TC);
		stm32_usart_debug_print(regs);
		BUG();
	}
}

void stm32_usart_device::irqinit()
{
	irqtable_set_handler(irqno, &_irqhandler, (void*) this);
}

int stm32_usart_device::setup(int32_t baud, char parity,
                             uint8_t databits, uint8_t stopbits)
{
	//STM32 count parity bit as one of databit.
	if (parity != 'n')
		databits += 1;

	stm32_usart_setup(regs, baud, parity, databits, stopbits);
	irqinit();
	nvic_enable_irq(irqno);

	return 0;
}