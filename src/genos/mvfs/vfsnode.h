#ifndef GENOS_MVFS_NODE_H
#define GENOS_MVFS_NODE_H

#include <igris/datastruct/dlist.h>
#include <errno.h>

namespace genos 
{
	class file;

	class vfsnode 
	{
	public:
		dlist_head lnk;
		dlist_head childs;
		vfsnode * parent;

		const char * name;
		uint8_t type;

		vfsnode(const char* name, uint8_t type) 
			: name(name), type(type) {}

		virtual int write(const void* data, size_t size, int flags) { return ENOTSUP; }
		virtual int read(void* data, size_t size, int flags) { return ENOTSUP; }

		virtual int release() { return ENOTSUP; }
		virtual int open(genos::file * filp) { return ENOTSUP; }
	};

	int open_node(vfsnode * res);
}

#endif