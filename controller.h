#ifndef _CONTROLLER_SHEDIT
#define _CONTROLLER_SHEDIT
#include "basic.h"

void initCurses();
void destroyCurses();

void setSystemState(enum SystemState state);
void revertSystemState();
#endif