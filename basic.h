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
	enum SystemState lastState;
	enum SystemState state;
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
	enum WordType type;
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
