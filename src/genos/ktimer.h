#ifndef GENOS_TIMER_TASKLET_H
#define GENOS_TIMER_TASKLET_H

#include <igris/sync/syslock.h>

#include <systime/systime.h>
#include <genos/ctrobj.h>

struct ktimer;

typedef void(* ktimer_callback_t)(void* arg, struct ktimer * tim);

struct ktimer_base
{
	struct ctrobj ctr;
	
	clock_t start;
	clock_t interval;
};

struct ktimer
{
	struct ktimer_base tim;

	ktimer_callback_t act;
	void * arg;
};

#define KTIMER_DECLARE(name, act, arg, interval) 				\
struct ktimer name = { 											\
	{ 															\
		CTROBJ_DECLARE(name.tim.ctr, CTROBJ_KTIMER_DELEGATE), 	\
		0, 														\
		interval 												\
	}, 															\
	act, 														\
	arg 														\
}

__BEGIN_DECLS

void timer_manager_step();

void ktimer_init_for_milliseconds(struct ktimer * tim, ktimer_callback_t act, void* arg,
        uint32_t ms);
void ktimer_base_init_for_milliseconds(struct ktimer_base * tim, uint32_t interval, uint8_t ctrtype);


void ktimer_base_plan(struct ktimer_base * t);

// Сместить временную отметку начала отсчета с тем, чтобы обеспечить
// срабатывание таймера через равные периоды времени.
void ktimer_swift(struct ktimer * t);

static inline
void ktimer_plan(struct ktimer * t) 
{
	ktimer_base_plan(&t->tim);
}

// Сместить временную отметку и перепланировать таймер.
static inline
void ktimer_replan(struct ktimer * t)
{
	ktimer_swift(t);
	ktimer_plan(t);
}

__END_DECLS

#endif