//
//  controller.c
//
//  Created by LIU XiaoDan on 7/11/2012.
//

/***********************************************************************************
*
* All code (C) LIU XiaoDan (lxdfigo@gmail.com), 2012
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
************************************************************************************/
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

BOOL isSystemState(enum SystemState state){
	if (shSystem.state == state)
		return TRUE;
	return FALSE;
};