#ifndef GENOS_KERNEL_SYSTIME_H
#define GENOS_KERNEL_SYSTIME_H

#include <inttypes.h>
#include <sys/cdefs.h>

#if __NOARCH
#else
#include <asm/delay.h> // Аппаратная поддержка малотактных задержек.
#endif

// Обязательно должен быть знаковым.
// Аналог clock_t.
typedef int64_t jiffies_t;

extern volatile jiffies_t __jiffies;

__BEGIN_DECLS

// Частота прерывания таймер
void systime_set_frequency(uint32_t freq);
uint32_t systime_frequency();

// Частота системного таймера
void sysclock_set_frequency(uint32_t freq);
uint32_t sysclock_frequency();

void system_tick();

jiffies_t jiffies();
jiffies_t millis();
jiffies_t micros();

void delay(double d);

jiffies_t ms2jiffies(uint32_t ms);

__END_DECLS


#if __NOARCH
static inline void delayMicroseconds(uint32_t us) { delay(us/1000); }
#else
static inline void delayMicroseconds(uint32_t us) { delay_us(us); }
#endif

#endif