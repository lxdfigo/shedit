#ifndef _UI_SHEDIT
#define _UI_SHEDIT
#include "basic.h"

void initUIModule();
void destroyUIModule();

void drawMenu();
void drawEditFrame();
void drawStatusFrame();

void drawSelectMenu(int index,int selSec);
void drawEditRoom();
void drawStatusRoom();
void setCurser();

void resetView();
void updateView();

#endif
