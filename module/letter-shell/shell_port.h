#ifndef __SHELL_PORT_H__
#define __SHELL_PORT_H__

#include "shell.h"
#include "ringbuffer.h"

extern Shell shell_user;
extern rbuffer_t rbuffer_user;

void userShellInit(void);

#endif
