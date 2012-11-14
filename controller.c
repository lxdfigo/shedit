#include "controller.h"
#include <ncurses.h>

TextInput textInput;
SystemStatus shSystem = {0,0,0,0,FALSE,InDefault,InDefault};

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