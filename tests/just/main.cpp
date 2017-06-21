#include <hal/arch.h>
#include <gxx/debug/dprint.h>
#include <sched/Scheduler.h>
#include <sched/AutomSchedee.h>

#include <resource/calls.h>

#include <thread>
#include <chrono>

#include <dev/devnull.h>
#include <dev/devzero.h>

genos::devnull null;
genos::devzero zero;

genos::test_resource tres;

void func(void*) {
	dprln("KEKEKE");
}

Genos::AutomSchedee aut(func);

void func2(void*) {
	dprln("KEKEKEgfdasfadf");
	int no = genos::open_test_resource(tres);
	int no2 = genos::openptr(&null, 0);
	dprln(no2);
	int ret = genos::close(no);
	if (ret) {
		dprf("close error");
	}

	aut.stop();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

Genos::AutomSchedee aut2(func2);

int main() {
	arch_init();
	dprln("HelloWorld");

	Genos::Glue::registerSchedee(&aut);
	Genos::Glue::registerSchedee(&aut2);

	aut.run();
	aut2.run();

	while(1) genos::sched::schedule();
}