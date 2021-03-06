#include <hal/board.h>
#include <hal/irq.h>
#include <drivers/gpio/gpio.h>
#include <drivers/gpio/pin.h>

#include <gxx/debug/dprint.h>
#include <gxx/debug/delay.h>

#include <sched/sched.h>
#include <sched/timer.h>
#include <sched/api.h>

//#include <crow/tower.h>

//#include <drivers/crow/uartgate.h>

#include <periph/irqdefs.h>
#include <drivers/serial/avr_usart.h>
#include <drivers/serial/uartring.h>
#include <drivers/spi/avr_spi.h>

#include <drivers/bdev/atmel_dataflash.h>

struct avr_usart uart0;
struct uartring_device serial0;

GPIO_PIN(dataflash_select, GPIOB, 0)

struct avr_spi_device spi;
AT45DB041 dataflash(&spi, &dataflash_select);

void * maintask(void *) {
	dprln("maintask");
	dataflash.reset();

	msleep(10);

	dataflash.lock();
	int ret = dataflash.get_status();
	dataflash.unlock();

	dprln(ret);
}

int main() {
	board_init();
	schedee_manager_init();

	gpio_settings(RED_LED_GPIO, RED_LED_MASK, GPIO_MODE_OUTPUT);
	gpio_settings(GREEN_LED_GPIO, GREEN_LED_MASK, GPIO_MODE_OUTPUT);

	gpio_write(RED_LED_GPIO, RED_LED_MASK, 1);
	gpio_write(GREEN_LED_GPIO, GREEN_LED_MASK, 1);

	gpio_pin_settings(&dataflash_select, GPIO_MODE_OUTPUT);
	gpio_pin_write(&dataflash_select, 1);

	struct uart_params uparams = { 115200, UART_PARITY_NONE, 1, 8 };

	uart0.init(USART0, ATMEGA_IRQ_U0RX);
	uart0.setup(&uparams);
	uart0.enable(true);
	serial0.init(&uart0, "ttyS0");
	
	spi.init("spi");
	//spi.enable(true);

	schedee_run(create_cooperative_schedee(maintask, NULL, 512));

	irqs_enable();
	__schedule__();
}

void __schedule__() {
	while(1) {
		timer_manager();
		schedee_manager();
	}
}


uint16_t crow_millis() {
	return millis();
}