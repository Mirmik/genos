#ifndef GENOS_SCHEDEE_H
#define GENOS_SCHEDEE_H

#include <stdint.h>
#include <gxx/dlist.h>
//#include <gxx/datastruct/dlist_head.h>
#include <gxx/datastruct/hlist_head.h>
#include <sched/AbstractSchedeeManager.h>

#include <sched/id.h>
#include <resource/descriptor.h>

//#include <kernel/resources/SchedeeResource.h>
//#include <kernel/resources/File.h>
#include <gxx/vector.h>
//#include <gxx/ByteArray.h>

#include <sync/csection.h>
//#include <kernel/event/OnceEvent.h>

#include <gxx/util/setget.h>

#ifndef PRIORITY_TOTAL
#	define PRIORITY_TOTAL 4
#endif

//#define SCHEDULE_RESUME 1
//#define SCHEDULE_REPEAT 0

static const uint8_t SCHEDEE_STATE_INIT = 0x00;
static const uint8_t SCHEDEE_STATE_RUN = 0x01;
static const uint8_t SCHEDEE_STATE_STOP = 0x07;

static const uint8_t SCHEDEE_STATE_WAIT = 0x02;
static const uint8_t SCHEDEE_BLOCKED_SEND = 0x03;
static const uint8_t SCHEDEE_BLOCKED_RECEIVE = 0x04;

static const uint8_t SCHEDEE_STATE_FINAL = 0x05;
static const uint8_t SCHEDEE_STATE_ZOMBIE = 0x06;

static const uint8_t SCHEDEE_STATE_MASK = 0x0F;

//static const uint8_t SCHEDEE_FINAL_DEALLOCATE = 0x80;
//static const uint8_t SCHEDEE_FINAL_RELEASE = 0x40;

#define CLONE_DESCRIPTORS 0x01
#define LINK_AS_RESOURCE 0x01

namespace Genos {

	class Schedee;

	namespace Glue {
		genos::pid_t registerSchedee(Schedee*);
	}

	class Schedee {//: public SchedeeResource 
	public:
		struct dlist_head schlnk;	//линк к спискам планировщика
		struct hlist_node hlnk;		//линк в хештаблицу процессов

		struct dlist_head chldlnk;	//линк к списку потомков
		gxx::dlist<Schedee, &Schedee::chldlnk> childs;
		gxx::vector<genos::descriptor> descriptors;

		uint16_t signals = 0;

		uint8_t prio;
		genos::pid_t pid; 
		
		struct {
			uint8_t state : 4;
			uint8_t subst : 1;
		};

		//SchedeeResourceList resources;
		//gxx::vector<File*> files;

		//OnceEvent finalWaiterHead;

	public:
		Schedee() : 
			prio(PRIORITY_TOTAL - 1), 
			state(SCHEDEE_STATE_INIT),
			subst(0)
		{
			dlist_init(&schlnk);
			hlist_node_init(&hlnk);
		};

		Schedee(uint8_t prio) : 
			prio(prio), 
			state(SCHEDEE_STATE_INIT),
			subst(0)
		{
			dlist_init(&schlnk);
			hlist_node_init(&hlnk);
		};

		Schedee(const Schedee&) = delete;

		bool is_subst() {
			return subst;
		}

		bool can_subst() {
			return subst;
		}

		void run() {
			Glue::systemSchedeeManager().runSchedee(*this);
		}

		void stop() {
			Glue::systemSchedeeManager().stopSchedee(*this);
		}

		void wait() {
			Glue::systemSchedeeManager().waitSchedee(*this, SCHEDEE_STATE_WAIT);
		}

		void wait(uint8_t state) {
			Glue::systemSchedeeManager().waitSchedee(*this, state);
		}

		void zombie() {
			Glue::systemSchedeeManager().zombieSchedee(*this);
			//if (resources.empty()) schedee_destroy();
		}

		void unwait() {
			Glue::systemSchedeeManager().unwaitSchedee(*this, SCHEDEE_STATE_WAIT);
		}

		void unwait(uint8_t state) {
			Glue::systemSchedeeManager().unwaitSchedee(*this, state);
		}

		void final() {
			Glue::systemSchedeeManager().finalSchedee(*this);
		}

		void setPrio(uint8_t prio) {
			assert(prio < PRIORITY_TOTAL);
			this->prio = prio;
		}

		void finalizeResources() {
			gxx::for_each(descriptors.begin(), descriptors.end(), [](genos::descriptor& desc){
				genos::close_descriptor(desc);
			});

			//finalWaiterHead.invoke();
			//unbindFromParent();
		}

		void finalizeSchedee() {
			atomic_section_enter();
			finalizeResources();
			zombie();
			atomic_section_leave();
		}

		virtual void execute() = 0;
		virtual void displace() = 0;
		virtual void finalize() = 0;
	
		/*void addResource(SchedeeResource& res) {
			resources.move_back(res);
			res.parent = this;
		}

		void unbindResource(SchedeeResource* res) {
			dlist_del_init(&res->reslnk);
			res->parent = nullptr;
			if (state == SCHEDEE_STATE_ZOMBIE && resources.empty()) {
				schedee_destroy();
			}
		}

		void releaseResource() override {
			final();
		}*/

		void schedee_destroy() {
			hlist_del(&hlnk);
			dlist_del(&schlnk);
			dlist_del(&chldlnk);
			destroy();
		}

		/*void initFromParent(Schedee* parent, uint8_t flags) {
			if (parent == nullptr) return;

			if (flags & CLONE_DESCRIPTORS) {
				files = parent->files;
				for (auto fptr : files) {
					fptr->open();
				}
			}

			if (flags & LINK_AS_RESOURCE) {
				setParent(parent);
			}
		}*/

		virtual void destroy() = 0;

		static genos::pid_t& getkey(Schedee& sch) { return sch.pid; }
	
		const char* m_name;
		ACCESSOR(name, m_name);
	};

	using SchedeeList = gxx::dlist<Schedee, &Schedee::schlnk>;

	Schedee* currentSchedee();
	void currentSchedee(Schedee* sch);

//	void finalizeSchedee(Schedee* sch);

	int8_t run(pid_t pid);
	int8_t stop(pid_t pid);
	int8_t final(pid_t pid);
	int8_t assign_name(pid_t pid, const char* name);

	Genos::Schedee* raw(pid_t pid);
}

namespace genos {
	Genos::Schedee* current_schedee();
	void current_schedee(Genos::Schedee* sch);
}

#endif