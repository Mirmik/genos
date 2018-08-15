#include <hal/board.h>
#include <arch/irqs.h>

#include <genos/time/systime.h>
#include <gxx/debug/dprint.h>

namespace board {
	arch::gpio::pin led(GPIOB, 7);
}

void board_init() {
	arch_init();
	systime::frequency = 16000000 / 256 / 64;

	board::led.mode(genos::hal::gpio::output);
	board::led.set();
}

void board_shutdown(arch_shutdown_mode_t mode) {
	emergency_stop();
	switch(mode)
	{
		case ARCH_SHUTDOWN_MODE_HALT:
		break;
		case ARCH_SHUTDOWN_MODE_REBOOT:
		break;
		case ARCH_SHUTDOWN_MODE_ABORT:
			arch::irqs::disable();
			board::led.mode(genos::hal::gpio::output);
			
			dprln("arch_shutdown");
			while(1) {
				board::led.tgl();
				systime::delay(100);
			}
		break;
	};
	arch_shutdown(mode);	
}

__attribute__((weak)) void emergency_stop() {}