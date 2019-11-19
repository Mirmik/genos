#include "contty.h"

#include <fcntl.h>
#include <unistd.h>

#include <igris/defs/vt100.h>

void genos::contty::newline()
{
	int ans;
	char line[48];
	int len = readline_linecpy(&rl, line, 48);

	if (ex)
		ans = ex->execute(line, len, SH_INTERNAL_SPLIT, nullptr);

	if (ans == EXECUTOR_PROCESS_STARTED)
		state = 5;
}

void genos::contty::execute()
{
	char c;
	int ret;

	switch (state)
	{
		case 0:
			if (debug_mode)
				dprln("contty: state 0");

			//ret = open_resource(outside, (int)0);

			/*if (ret)
			{
				dprln("contty:open_resource", ret);
				schedee_exit();
			}*/

			readline_init(&rl, buffer, CONTTY_LINE_LENGTH);
			readline_history_init(&rl, hbuffer, CONTTY_HISTORY_SIZE);
			state = 1;

		case 1:
			if (debug_mode)
				dprln("contty: state 1");

			readline_newline_reset(&rl);
			state = 2;
			if (echo)
				outside->write(prefix_string, strlen(prefix_string), 0);

		case 2:
			if (debug_mode)
				dprln("contty: state 2");

			state = 3;
			outside->read(&c, 0, 0); //Неблокирующий wait для автомата.
			break;

		case 3:
			if (debug_mode)
				dprln("contty: state 3");

			ret = outside->read(&c, 1, 0);

			if (debug_mode)
			{
				dpr("contty: read char ");
				dprhex(c);
				dln();
				state = 2;
				return;
			}

			ret = readline_putchar(&rl, c);

			switch (ret)
			{
				case READLINE_ECHOCHAR:
					{
						if (echo)
							ret = outside->write(&c, 1, 0);

						if (!sline_in_rightpos(&rl.line))
						{
							char buf[16];

							if (echo)
							{
								outside->write(sline_rightpart(&rl.line), sline_rightsize(&rl.line), 0);
								ret = vt100_left(buf, sline_rightsize(&rl.line));
								outside->write(buf, ret, 0);
							}
						}
					}
					break;

				case READLINE_NEWLINE:
					state = 1;

					if (echo)
						outside->write("\r\n", 2, 0);

					newline();
					return;

				case READLINE_BACKSPACE:
					{
						if (echo)
						{
							outside->write(VT100_LEFT, 3, 0);
							outside->write(VT100_ERASE_LINE_AFTER_CURSOR, 3, 0);
						}
						if (!sline_in_rightpos(&rl.line))
						{
							char buf[16];

							if (echo)
							{
								outside->write(sline_rightpart(&rl.line), sline_rightsize(&rl.line), 0);
								ret = vt100_left(buf, sline_rightsize(&rl.line));
								outside->write(buf, ret, 0);
							}
						}

						break;
					}


				case READLINE_RIGHT:
					if (echo)
						outside->write(VT100_RIGHT, 3, 0);
					break;

				case READLINE_LEFT:
					if (echo)
						outside->write(VT100_LEFT, 3, 0);
					break;

				case READLINE_NOTHING:
					break;

				case READLINE_UPDATELINE:
					{
						char buf[16];

						if (rl.lastsize)
						{
							if (echo)
							{
								ret = vt100_left(buf, rl.lastsize);
								outside->write(buf, ret, 0);
								outside->write(VT100_ERASE_LINE_AFTER_CURSOR, 3, 0);
							}
						}

						if (rl.line.len)
							if (echo)
								outside->write(rl.line.buf, rl.line.len, 0);

						break;
					}

				default:
					dprln("retcode:", ret);
					BUG();
			}

			state = 2;
			break;

		case 5:
			dprln("state5");
			state = 1;
			return;

		default:
			BUG();
	}
}