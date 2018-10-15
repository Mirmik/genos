#include <utility/getty.h>
#include <drivers/cdev/cdev.h>

#include <gxx/panic.h>

static int newline(struct getty_context * cntxt) {
	dpr("line_state: "); debug_write(cntxt->line.buf, cntxt->line.len); dprln();
}

void * nologin_getty(void * arg, int * state) 
{
	char c;
	int ret;

	struct getty_context * cntxt = (struct getty_context *) arg;
	struct char_device * cdev = cntxt->cdev;
	struct sline * line = &cntxt->line;

	switch (*state) 
	{
		case 0:
			cntxt->last = '\n';
			sline_setbuf(line, cntxt->buffer, GETTY_LINE_LENGTH);
			*state = 1;

		case 1: 
			sline_reset(line);
			cdev->write("$ ", 2);
			*state = 2;

		case 2:
			//dpr("line_state: "); debug_write(line->buf, line->len); dprln();
			cdev->waitread(); //Неблокирующий wait для автомата.
			*state = 3;
			break;

		case 3:
			while ((ret = cdev->read(&c, 1))) 
			{
				switch (c) {
					case '\r': 
					case '\n':
						if (cntxt->last == '\r') 
							break;
						cntxt->last = '\n';
						cdev->write("\r\n", 2);
						newline(cntxt);
						*state = 1;
						return NULL;
					default:
						cdev->write(&c, 1);
						sline_putchar(line, c);
						break;
				}
				cntxt->last = c;
			}
			*state = 2;
			break;

		default:
			panic("state");
	}

	return NULL;
}