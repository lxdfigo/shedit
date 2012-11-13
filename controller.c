#include "controller.h"
#include <ncurses.h>

TextInput textInput;
SystemStatus shSystem = {0,0,0,0,0,InDefault,InDefault};

void initCurses(){
	initscr();
	clear();
	noecho();
	cbreak(); /*禁用行缓冲，直接传递所有输入*/
	if (!has_colors() || start_color() == ERR){
		printw("Terminal don't support colors!\n");
	}

}
void destroyCurses(){
	clrtoeol();
	refresh();
	endwin();
}