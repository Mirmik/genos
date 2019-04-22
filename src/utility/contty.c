#include <utility/contty.h>

#include <drivers/cdev/cdev.h>

#include <igris/util/bug.h>

//#include <sched/sched.h>
//#include <sched/schedee.h>
//#include <mvfs/file.h>
//#include <mvfs/compat.h>

//#include <utility/sh.h>

static int newline(struct contty_context * cntxt) {
//	struct schedee * sch = create_autom_schedee(sh_utility, NULL);
//	schedee_run(sch);
//	__schedee_wait_for(current_schedee(), sch);
//	__displace__();
}

void * contty_automate(void * arg, int * state) 
{
	struct file * filp;
	char c;
	int ret;

	struct contty_context * cntxt = (struct contty_context *) arg;
	struct char_device * cdev = cntxt->cdev;
	struct sline * line = &cntxt->line;

	switch (*state) 
	{
		case 0:
			//filp = vfs_file_alloc();
			//cdev->open(filp);
			//vfs_open_node(cdev, &filp);
			//schedee_setfd(current_schedee(), filp, 0); //input
			//schedee_setfd(current_schedee(), filp, 1); //output

			cntxt->last = '\n';
			sline_setbuf(line, cntxt->buffer, CONTTY_LINE_LENGTH);
			*state = 1;

		case 1: 
			sline_reset(line);
			cdev->c_ops->write(cdev, "$ ", 2, 0);
			*state = 2;

		case 2:
			//dpr("line_state: "); debug_write(line->buf, line->len); dprln();
			cdev->c_ops->waitread(cdev); //Неблокирующий wait для автомата.
			*state = 3;
			break;

		case 3:
			while ((ret = cdev->c_ops->read(cdev, &c, 1, 0))) 
			{
				switch (c) {
					case '\r': 
					case '\n':
						if (cntxt->last == '\r') 
							break;
						cntxt->last = '\n';
						cdev->c_ops->write(cdev, "\r\n", 2, 0);
						newline(cntxt);
						*state = 1;
						return NULL;
					default:
						cdev->c_ops->write(cdev, &c, 1, 0);
						sline_putchar(line, c);
						break;
				}
				cntxt->last = c;
			}
			*state = 2;
			break;

		default:
			BUG();
	}

	return NULL;
}