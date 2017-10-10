#include <hal/irqs.h>
#include <avr/io.h>
#include <avr/interrupt.h>

namespace hal {
	namespace irqs {
		static inline void enable(void) {
			sei();
		}
		
		static inline uint8_t save(void) {
			irqstate_t save = SREG;
			cli();
			return save;
		}
		
		static inline void restore(uint8_t state) {
			SREG = state;
		}
		
		static inline void disable(void) {
			(void) save();
		}
	}
}