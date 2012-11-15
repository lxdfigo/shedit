#include "ui.h"
#include "controller.h"
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/ioctl.h> 
#include <termios.h>  
#include <unistd.h>  
#include <errno.h>  
#include <ncurses.h>

#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL)  

#define MENUAHEAD 2
#define MAXITEM 10


typedef struct {
	int width;
	int num;
	char *name;
	char *section[MAXITEM];
}MenuItem;

typedef struct{
	int width;
	int height;
	int itemWidth;
	int num;
	MenuItem items[MAXITEM];
}CMenu;


//The color of each item in the ui window, mr: menu room, er: edit room, sr: status room
//The value is the control with the color
//
typedef struct{
	//the background(bg) and textcolor(tc) in menu room when it is selected(s) or not
	int mr_bg;
	int mr_s_bg;
	int mr_tc_1;
	int mr_tc_2;
	int mr_s_tc_1;
	int mr_s_tc_2;

	//the background(bg) and textcolor(tc) of key words(k), normal word and warning(w) in edit room when it is selected(s) or not
	int er_bg;
	int er_s_bg;
	int er_tc;
	int er_s_tc_1;
	int er_s_tc_2;
	int er_s_tc_3;
	int er_f_ktc;  // the color of functions
	int er_o_ktc;  // the color of operators
	int er_v_ktc;  // the color of variance
	int er_e_tc;   // the color of explain
	int er_c_tc_1;   // the color of string 1
	int er_c_tc_2;   // the color of string 2
	int er_w_tc;   // the color of warning
	int er_w_bg;   // the color of warning background


	//the background(bg) and textcolor(tc) in status room
	int sr_bg;
	int sr_tc_1;
	int sr_tc_2;
	int sr_tc_3;

}ColorSultion;

typedef struct{
	int x;
	int y;
}Point;

WINDOW *inputWin;
int erx,erex,ery,erey,mrx,mrex,mry,mrey,srx,srex,sry,srey;
ColorSultion colorsln;
CMenu menu = {80,1,8,4,{
	{8,4,"FILE",  {
		"New",
			"Load",
			"Save",
			"Quit"}},
		{8,5,"EDIT",{
			"Undo",
				"Copy",
				"Cut",
				"Paste",
				"Search"}},
			{20,3,"DEBUG", {
				"Run",
					"Set Breakpoint",
					"Del Breakpoint"}},
				{8,1,"HELP",{
					"About"}}
}
};
void initColorSln(ColorSultion *csln){

	int initnum = 0;
	//init menu color
	init_pair(++initnum, COLOR_BLUE, COLOR_WHITE);
	csln->mr_tc_1 = initnum;
	init_pair(++initnum, COLOR_CYAN, COLOR_WHITE);
	csln->mr_tc_2 = initnum;
	init_pair(++initnum, COLOR_WHITE, COLOR_BLUE);
	csln->mr_s_tc_1 = initnum;
	init_pair(++initnum, COLOR_YELLOW, COLOR_BLUE);
	csln->mr_s_tc_2 = initnum;

	//init edit color
	init_pair(++initnum, COLOR_WHITE, COLOR_BLACK);
	csln->er_tc = initnum;
	init_pair(++initnum, COLOR_BLACK, COLOR_WHITE);
	csln->er_s_tc_1 = initnum;
	init_pair(++initnum, COLOR_CYAN, COLOR_WHITE);
	csln->er_s_tc_2 = initnum;
	init_pair(++initnum, COLOR_MAGENTA, COLOR_WHITE);
	csln->er_s_tc_3 = initnum; 
	init_pair(++initnum, COLOR_GREEN, COLOR_BLACK);
	csln->er_f_ktc = initnum;   
	init_pair(++initnum, COLOR_YELLOW, COLOR_BLACK);
	csln->er_o_ktc = initnum;  
	init_pair(++initnum, COLOR_BLUE, COLOR_BLACK);
	csln->er_v_ktc = initnum;   
	init_pair(++initnum, COLOR_BLACK, COLOR_BLACK);
	csln->er_e_tc = initnum;     
	init_pair(++initnum, COLOR_MAGENTA, COLOR_BLACK);
	csln->er_c_tc_1 = initnum;   
	init_pair(++initnum, COLOR_CYAN, COLOR_BLACK); 
	csln->er_c_tc_2 = initnum;  
	init_pair(++initnum, COLOR_RED, COLOR_BLACK);
	csln->er_w_tc = initnum;   


	//init status color
	init_pair(++initnum, COLOR_BLACK, COLOR_WHITE);
	csln->sr_tc_1 = initnum;
	init_pair(++initnum, COLOR_RED, COLOR_WHITE);
	csln->sr_tc_2 = initnum;
	init_pair(++initnum, COLOR_BLUE, COLOR_WHITE);
	csln->sr_tc_3 = initnum;
}

void initUIModule(){
	initColorSln(&colorsln);
}

void destroyUIModule(){
	if (inputWin != NULL){
		delwin(inputWin);
		inputWin = NULL;
	}
}

void resetView(){
	drawMenu();
	drawEditFrame();
	drawStatusFrame();
	drawStatusRoom();
	drawEditRoom();
	setCurser(shSystem.textX,shSystem.textY);
}

void gotoxy(int x,int y){
	move(y,x);
}
void setcolor(int pairnum){
	attron(COLOR_PAIR(pairnum));

}
void drawItem(int x,int y,char *itemname,int c1,int c2){
	int i = 0;
	gotoxy(x,y);
	setcolor(c1);
	printw("%c",itemname[0]);
	setcolor(c2);
	printw("%s",itemname+1);
}

void printBg(int x,int y,int ex,int ey,int tc){
	int i,j;
	setcolor(tc);
	for(i = y; i <= ey; ++i){
		gotoxy(x,i);
		for(j = x; j <= ex; ++j){
			printw(" ");
		}
	}
}
void drawNoTopWindow(int x,int y,int ex,int ey,int tc){
	int i;
	printBg(x,y,ex,ey,tc);

	for(i = y; i <= ey; ++i){
		gotoxy(x, i);
		printw("|");
		gotoxy(ex,i);
		printw("|");
	}
	for(i = x; i <= ex; ++i){
		gotoxy(i, ey);
		printw("-");
	}
}
void drawWindow(int x,int y,int ex,int ey,int tc){
	int i;
	drawNoTopWindow(x,y,ex,ey,tc);
	printBg(x,y,ex,ey,tc);

	for(i = x; i <= ex; ++i){
		gotoxy(i, y);
		printw("-");
	}
}

void drawMenuList(int index,int selSec){
	int i;
	int begin = index * menu.itemWidth;
	int bg,tc1,tc2;

	drawNoTopWindow(begin,mrey + 1,begin + menu.items[index].width,mrey + 1 + menu.items[index].num,
		colorsln.mr_tc_1);

	for(i = 0; i < menu.items[index].num; ++i){
		if (selSec == i){
			tc1 = colorsln.mr_s_tc_1; tc2 = colorsln.mr_s_tc_2;
		}else{
			tc1 = colorsln.mr_tc_1; tc2 = colorsln.mr_tc_2;
		}
		printBg(begin + 1,mrey + 1 + i,begin + menu.items[index].width - 1,mrey + 1 + i,tc1);

		drawItem(begin + 2, mrey + 1 + i,menu.items[index].section[i],tc2,tc1);
	}
}

void drawSelectMenu(int index,int selSec){
	drawItem(2 + mrx + index * menu.itemWidth,mry,
		menu.items[index].name,colorsln.mr_s_tc_2,colorsln.mr_s_tc_1);
	drawMenuList(index,selSec);
}

void drawMenu(){
	int i = 0;
	mrx = 0;
	mry = 0;
	mrex = COLS - 1;
	mrey = mry;

	printBg(mrx,mry,mrex,mrey,colorsln.mr_tc_1);

	for(i = 0; i < menu.num; ++i){
		drawItem(MENUAHEAD + mrx + i * menu.itemWidth,mry,
			menu.items[i].name,colorsln.mr_tc_2,colorsln.mr_tc_1);
	}
}

void drawEditFrame(){
	int i;
	erx = 0;
	ery = mrey + 1;
	erex = COLS-1;
	erey = LINES-2;

	printBg(erx,ery,erex,erey,colorsln.er_tc);

	setcolor(colorsln.er_tc);
	for(i = erx; i < erex; ++i){
		gotoxy(i,ery);
		printw("=");
		//printw("%c",205);
	}
	for(i = erx; i < erex; ++i){
		gotoxy(i,erey);
		printw("-");
		//printw("%c",196);
	}
	gotoxy((erex-erx-6)/2 + erx,ery);
	printw("shedit");
}
void drawStatusFrame(){
	srx = 0;
	sry = LINES - 1;
	srex = COLS - 1;
	srey = sry;

	printBg(srx,sry,srex,srey,colorsln.sr_tc_1);

}

void setCurser(int x,int y){
	gotoxy(erx + x,ery + 1 + y);
}

void drawWord(Word *w){
	int i = 0;
	if (w->type == NORMAL){
		setcolor(colorsln.er_tc);
	}else if (w->type == FUNCTION){
		setcolor(colorsln.er_f_ktc);
	}else if (w->type == OPERATOR){
		setcolor(colorsln.er_o_ktc);
	}else if (w->type == KEYWORD){
		setcolor(colorsln.er_v_ktc);
	}else if (w->type == STRING){
		setcolor(colorsln.er_c_tc_1);
	}else if (w->type == EXPLAIN){
		setcolor(colorsln.er_e_tc);
	}
	for(i = w->begin; i < w->end; ++i){
		printw("%c",textInput.buffer[i]);
	}
}

void drawEditRoom(){
	gotoxy(erx,ery+1);
	setcolor(colorsln.er_tc);
	Word *w = textInput.head;
	while(w != NULL){
		drawWord(w);
		w = w->next;
	}
}

void drawStatusRoom(){

}

void storeWindow(int x,int y,int wid,int hei){
	if (inputWin != NULL){
		delwin(inputWin);
		inputWin = NULL;
	}
	inputWin = newwin(hei,wid,y,x);
	overwrite(stdscr,inputWin);
}

void updateMenu(){
	shSystem.menuIndex = shSystem.menuIndex < 0 ? 0 : shSystem.menuIndex;
	shSystem.menuIndex = shSystem.menuIndex >= menu.num ? menu.num - 1 : shSystem.menuIndex;

	shSystem.menuSection = shSystem.menuSection < 0 ? 0 : shSystem.menuSection;
	shSystem.menuSection = shSystem.menuSection >= menu.items[shSystem.menuIndex].num ? 
		menu.items[shSystem.menuIndex].num - 1 : shSystem.menuSection;

	//storeWindow(shSystem.menuIndex * menu.width,0,5,12);
	storeWindow(0,0,COLS,20);

	drawSelectMenu(shSystem.menuIndex,shSystem.menuSection);

}

void drawSaveDialog(){
	int x,y,w,h;
	w = 40;
	h = 10;
	x = (COLS - w) / 2;
	y = (LINES - h) / 2 - 1;

	storeWindow(x,y,w,h);
	drawWindow(x,y,w+x,h+y,colorsln.mr_tc_1);

	x += 5; y += h/2 - 1;
	printBg(x, y, x + w - 10, y,colorsln.er_tc);
	mvprintw(y,x,"%s",textInput.tmpstr);
}

void drawLoadDialog(){

}
void updateView(){
	if (inputWin != NULL){
		overwrite(inputWin, stdscr);
		delwin(inputWin);
		inputWin = NULL;
	}
	curs_set(2);
	switch(shSystem.state){
		case InMenu:
			curs_set(0);
			updateMenu();
			break;
		case InSave:
			drawSaveDialog();
			break;
		case InLoad:
			drawLoadDialog();
			break;
		case InDefault:
			drawEditRoom();
			drawStatusRoom();
			break;
	}
	refresh();
}
