#ifndef __VCTRL_HANDLER__
#define __VCTRL_HANDLER__

void vctrl_init(void);
void start_vctrl_service(void);
void handle_vctrl_set_fd(unsigned int u32CoreClock, unsigned int u32BpuClock, unsigned int u32JpegCoreClock);
int get_vctrl_set_fd();
extern void handle_control_message(const char* name, const char* value);

#endif//__VCTRL_HANDLER__
