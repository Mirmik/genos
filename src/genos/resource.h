#ifndef GENOS_RESOURCE_H
#define GENOS_RESOURCE_H

#include <string.h>
#include <errno.h>

#include <igris/datastruct/dlist.h>
#include <igris/dprint.h>

#include <limits.h>

#define GENOS_RESOURCE_FILE 1
#define GENOS_RESOURCE_DIRECTORY 1

namespace genos
{
	class openres;

	class resource
	{
	public:
		virtual int release() { return ENOTSUP; }
		virtual int open(openres * ores) { return ENOTSUP; }
		virtual int gettype() { return 0; }
	};


	struct node : public resource
	{
	public:
		virtual int write(const void* data, size_t size, int flags) { return ENOTSUP; }
		virtual int read(void* data, size_t size, int flags) { return ENOTSUP; }
		virtual int room() { return INT_MAX; }
		virtual int avail() { return INT_MAX; }
		//virtual int write(const void* data, size_t size, genos::openres* onode) { return ENOTSUP; }
		//virtual int read(void* data, size_t size, genos::openres* onode) { return ENOTSUP; }

		int print(const char * str, int flags) { return write(str, strlen(str), flags); }
	
		int gettype() override { return GENOS_RESOURCE_FILE; } 
	};

//	int open_node(genos::node * res, genos::openres * ores);
//	int open_node(genos::node * res, int flags);
	int open_resource(genos::resource * res, genos::openres * ores);
	int open_resource(genos::resource * res, int16_t flags);

	
	class directory : public resource
	{
	public:
		virtual int iterate(char* buffer, size_t maxsz, genos::openres* onode) { return ENOTSUP; }
		virtual int mknode(const char* childname, genos::openres* onode) { return ENOTSUP; }
		virtual int rmnode(const char* childname, genos::openres* onode) { return ENOTSUP; }
	
		int gettype() override { return GENOS_RESOURCE_DIRECTORY; } 
	};

	int open_directory(genos::directory * res);
	int open_directory(genos::directory * res, genos::openres * ores);

	
	class openres
	{
	public:
		union {
			genos::resource * res;
			genos::node * node;
			genos::directory * dir;
		};
		int16_t flags;
	};

#define RESTBL_SIZE 4

	struct restbl
	{
//		genos::openres * tbl;
//		uint8_t tblsz;

		genos::openres tbl[RESTBL_SIZE];
	
	public:
		restbl() 
		{
			memset(tbl, 0, sizeof(tbl));
		}

		int size() 
		{
			return RESTBL_SIZE;
		}

/*		void set_table(genos::openres * tbl, uint8_t tblsz)
		{
			this->tbl = tbl;
			this->tblsz = tblsz;

			memset(tbl, 0, sizeof(genos::openres) * tblsz);
		}*/

		genos::openres * operator[](int i) 
		{
			if (i >= RESTBL_SIZE) 
				return nullptr;

			return &tbl[i];
		}

		int get_available_fd();
	};


	class named_node : public node
	{
	public:
		dlist_head lnk;
		const char* name;

	public:
		named_node() {}
		named_node(const char* name) : lnk(DLIST_HEAD_INIT(lnk)), name(name) {}
	};

	class named_node_list : public directory
	{
	public:
		dlist_head list = DLIST_HEAD_INIT(list);

		int iterate(char* buffer, size_t maxsz, genos::openres* onode) override
		{
		/*	if (onode->dh == nullptr)
				onode->dh = list.next;
			else
				onode->dh = onode->dh->next;

			if (onode->dh == &list)
			{
				buffer[0] = 0;
				return 0;
			}

			strncpy(buffer, dlist_entry(onode->dh, named_node, lnk)->name, maxsz);
		*/	return 0;
		}
	};
}

//#define RESOURCE_TABLE(name, size) genos::openres name[size]

#endif