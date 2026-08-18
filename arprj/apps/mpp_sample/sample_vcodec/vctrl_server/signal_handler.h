#ifndef __SIGNAL_HANDLER__
#define __SIGNAL_HANDLER__

void signal_init(void);
void handle_signal(void);
int get_signal_fd();

#endif//__SIGNAL_HANDLER__
