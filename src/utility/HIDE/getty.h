#ifndef GENOS_UTILITY_GETTY_H
#define GENOS_UTILITY_GETTY_H

#include <sys/cdefs.h>
#include <igris/datastruct/sline.h>

#define GETTY_LINE_LENGTH 32

struct char_device;

struct getty_context {
	struct sline line;
	struct char_device * cdev;
	char last;

	char buffer[GETTY_LINE_LENGTH];
};

__BEGIN_DECLS

void * nologin_getty(void * arg, int * state);

__END_DECLS

#endif