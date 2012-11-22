#include "controller.h"

Screen screen;
TextInput textInput;
SystemStatus shSystem;

void initSystem(){
	shSystem.isQuit = FALSE;
	shSystem.lastState = InDefault;
	shSystem.state = InDefault;
	shSystem.menuIndex = 0;
	shSystem.menuSection = 0;
	memset(shSystem.fileName,0,sizeof(shSystem.fileName));
}

void initCurses(){
	initscr();
	clear();
	noecho();
	//raw();
	cbreak(); 
	keypad(stdscr,TRUE);
	refresh();
	if (!has_colors() || start_color() == ERR){
		printw("Terminal don't support colors!\n");
	}
	initSystem();
}
void destroyCurses(){
	clrtoeol();
	refresh();
	endwin();
}

void setSystemState(enum SystemState state){
	shSystem.lastState = shSystem.state;
	shSystem.state = state;
}
void revertSystemState(){
	setSystemState(shSystem.lastState);
}
