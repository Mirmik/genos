#ifndef GENOS_DRIVERS_AVR_TIMER_H
#define GENOS_DRIVERS_AVR_TIMER_H

#include <periph/regs/timer.h>
#include <periph/irqdefs.h>

#include <hal/irqtable.h>
#include <igris/util/bits.h>

#include <assert.h>

namespace genos
{
	namespace avr
	{
		struct timer_irqs
		{
			uint8_t ovf;
			uint8_t compa;
			uint8_t compb;
			uint8_t compc;
		};

		static uint8_t tc_divider_code(uint16_t divider)
		{
			switch (divider)
			{
				case 0 :
					return 0;

				case 1 :
					return 1;

				case 8 :
					return 2;

				case 64 :
					return 3;

				case 256 :
					return 4;
			}

			return -1;
		}

		template<class T, class R>
		struct timer
		{
			R  							regs;
			struct timer_irqs	 		irqs;
			volatile uint8_t* 			timsk;

			/// Read value
			T value()
			{
				return regs->tcnt;
			}

			void set_value(T val)
			{
				regs->tcnt = val;
			}

			void set_compare_a(T val)
			{
				regs->ocr_a = val;
			}

			void set_compare_b(T val)
			{
				regs->ocr_b = val;
			}

			/**
				Активировать таймер.
				@param div Устанавливаемый делитель. (см. доступные делители в tc_divider_code)
				@return Возвращает -1, если делитель неверен. Иначе 0.
			*/
			int set_divider(uint16_t div)
			{
				int divcode = tc_divider_code(div);

				if (divcode < 0)
					return -1;

				/// Set divider to CSX[0:2]
				bits_mask_assign(regs->tccr_b, 7, divcode);
				return 0;
			}

			int set_divcode(uint8_t code) 
			{
				bits_mask_assign(regs->tccr_b, 7, code);
				return 0;
			}

			/// Enable overflow irq.
			int irq_ovf_enable(bool en)
			{
				bits_mask_assign(*timsk, 0x1, en);
			}

			//Set overflow interrupt handler to irqtable.
			void set_ovf_handler(void(*handler)(void*), void* arg)
			{
				genos::irqtable::set_handler(irqs.ovf, handler, arg);
			}
		};

		struct timer8 : public timer<uint8_t, timer_8bit_regs*>
		{
			using parent = timer<uint8_t, timer_8bit_regs*>;

			timer8(timer_8bit_regs* a, struct timer_irqs b, volatile uint8_t* c) : parent { a, b, c }
			{}
		};

		struct timer16 : public timer<uint16_t, timer_16bit_regs*>
		{
			using parent = timer<uint16_t, timer_16bit_regs*>;

			timer16(timer_16bit_regs* a, struct timer_irqs b, volatile uint8_t* c) : parent { a, b, c }
			{}

			void set_compare_c(uint16_t val)
			{
				regs->ocr_c = val;
			}

			/*
				Output modes:
				0b00 - disconnect
				0b01 - toggle
				0b10 - clear on compare, set on downlevel 
				0b10 - set on compare, clear on downlevel
			*/

			void set_output_a_mode(uint8_t mode)
			{
				assert(mode <= 3);
				bits_mask_assign_bias(regs->tccr_a, 0b11, mode, 6);
			}

			void set_output_b_mode(uint8_t mode)
			{
				assert(mode <= 3);
				bits_mask_assign_bias(regs->tccr_a, 0b11, mode, 4);
			}

			void set_output_c_mode(uint8_t mode)
			{
				assert(mode <= 3);
				bits_mask_assign_bias(regs->tccr_a, 0b11, mode, 2);
			}

			/*
				WGM modes:

				Value 	Timer mode         TOP

				 0000  	Clock mode         0xFFFF
				 0001  	PWM  8bit          0x00FF
				 0010  	PWM  9bit          0x01FF
				 0011  	PWM 10bit          0x03FF
				 0100  	CTC                OCRnA
				 0101  	Fast PWM  8bit     0x00FF
				 0110  	Fast PWM  9bit     0x01FF
				 0111  	Fast PWM 10bit     0x03FF

				 1000
				 #TODO					
			*/

			enum class TimerMode : uint8_t
			{
				Clock     = 0b0000,
				PWM8      = 0b0001,
				PWM9      = 0b0010,
				PWM10     = 0b0011, 
				CTC       = 0b0100,
				FastPWM8  = 0b0101,
				FastPWM9  = 0b0110,
				FastPWM10 = 0b0111
			};

			void set_mode(TimerMode mode)
			{
				uint8_t aval = 0b11 & (uint8_t)mode;
				uint8_t bval = 0b11 & ((uint8_t)mode >> 2);
				bits_mask_assign(		regs->tccr_a, 0b11, aval);
				bits_mask_assign_bias(	regs->tccr_b, 0b11, bval, 3);
			}

			void irq_overflow_enable(bool en)
			{
				bits_lvl(*timsk, 1 << 0, en); //TOIE
			}

			void irq_compare_a_enable(bool en)
			{
				bits_lvl(*timsk, 1 << 1, en); //OCIEA
			}

			void irq_compare_b_enable(bool en)
			{
				bits_lvl(*timsk, 1 << 2, en); //OCIEB
			}

			void debug_print_regs()
			{
				dpr("TCCR_A: "); dprptr(&regs->tccr_a); dpr(" "); dprbinln(regs->tccr_a);
				dpr("TCCR_B: "); dprptr(&regs->tccr_b); dpr(" "); dprbinln(regs->tccr_b);
				dpr("TCCR_C: "); dprptr(&regs->tccr_c); dpr(" "); dprbinln(regs->tccr_c);
				dpr("TCNT__: "); dprptr(&regs->tcnt); dpr(" "); dprbinln(regs->tcnt);
				dpr("ICR___: "); dprptr(&regs->icr); dpr(" "); dprbinln(regs->icr);
				dpr("OCR_A_: "); dprptr(&regs->ocr_a); dpr(" "); dprbinln(regs->ocr_a);
				dpr("OCR_B_: "); dprptr(&regs->ocr_b); dpr(" "); dprbinln(regs->ocr_b);
				dpr("OCR_C_: "); dprptr(&regs->ocr_c); dpr(" "); dprbinln(regs->ocr_c);
				dpr("TIMSK_: "); dprptr(timsk); dpr(" "); dprbinln(*timsk);
			}
		};
	}
}

namespace periph
{
#if defined (CHIP_ATMEGA2560)
	extern genos::avr::timer8 timer0;
	extern genos::avr::timer8 timer2;
	extern genos::avr::timer16 timer1;
	extern genos::avr::timer16 timer3;
	extern genos::avr::timer16 timer4;
	extern genos::avr::timer16 timer5;
#elif defined (CHIP_ATMEGA328P)
	extern genos::avr::timer8 timer0;
	extern genos::avr::timer8 timer2;
	extern genos::avr::timer16 timer1;
#endif
}

#endif