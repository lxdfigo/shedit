//
//  basic.h
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
#ifndef _BASIC_SHEDIT
#define _BASIC_SHEDIT

#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BOOL char
//#define TRUE 1
//#define FALSE 0

enum SystemState{
	InMenu,
	InTextEdit,
	InDebug,
	InQuit,
	InLoad,
	InSave,
	InAbout,
	InManual,
	InSelect,
	InDefault
};

typedef struct _Point{
	int ln;
	int col;
	struct _Point *next;
	struct _Point *previous;
}Point;

typedef struct {
	int menuIndex;
	int menuSection;
	BOOL isQuit;
	char fileName[1024];
	int lastState;
	int state;
}SystemStatus;

enum WordType{
	EMPTY = 0,
	NORMAL = 1,
	FUNCTION = 2,
	KEYWORD = 3,
	OPERATOR = 4,
	STRING = 11,
	STRING_DOT = 12,
	EXPLAIN = 13,
	NEWLINE = 21,
	SEPARATE = 22
};

struct _Word;

typedef struct _Element{
	char c;
	BOOL isSelected;
	struct _Element *next;
	struct _Element *previous;
	struct _Word *father;
}Element;

typedef struct _Word{
	int type;
	Element *begin;
	Element *end;
	int count;
	struct _Word *next;
	struct _Word *previous;
}Word;

typedef struct{
	char tmpStr[1024];
	short tmpCur;
	Word *headWord;
	int curLn,curCol;
	Point *breakpoints;
	Element *screenBeginElement;
	Element *curElement;
	Element *selected_begin,* selected_end, *selected_center;
	Element *copy_begin,* copy_end;
}TextInput; 

typedef struct{
	int erx,erex,ery,erey,mrx,mrex,mry,mrey,srx,srex,sry,srey;
	int curX,curY;
	int width;
	int height;
}Screen;

extern Screen screen;
extern TextInput textInput;
extern SystemStatus shSystem;

#endif
