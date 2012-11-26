//
//  ui.c
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
#include "ui.h"
#include "controller.h"

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

WINDOW *inputWin;
ColorSultion colorsln;
CMenu menu = {80,1,8,4,{
	{8,4,"FILE",  {
		"New",
			"Load",
			"Save",
			"Quit"}},
		{8,4,"EDIT",{
			//"Undo",
				"Cut",
				"Copy",
				"Paste",
				"Search"}},
			{20,3,"DEBUG", {
				"Run",
					"Step",
					"Set Breakpoint"}},
				{8,2,"HELP",{
					"Manual",
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
	init_pair(++initnum, COLOR_CYAN, COLOR_BLACK);
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

	screen.width = COLS;
	screen.height = LINES;
	screen.mrx = 0;
	screen.mry = 0;
	screen.mrex = screen.width - 1;
	screen.mrey = screen.mry;
	screen.erx = 0;
	screen.ery = screen.mrey + 1;
	screen.erex = screen.width-1;
	screen.erey = screen.height-2;
	screen.srx = 0;
	screen.sry = screen.height - 1;
	screen.srex = screen.width - 1;
	screen.srey = screen.sry;
	screen.curX = screen.erx;
	screen.curY = screen.ery+1;
}

void destroyUIModule(){
	if (inputWin != NULL){
		delwin(inputWin);
		inputWin = NULL;
	}
}

void resetView(){
	drawMenu();
	drawEditRoom();
	drawStatusRoom();
	setCurser(screen.curX,screen.curY);
	curs_set(2);
	refresh();
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
	for(i = x; i <= ex; ++i){
		gotoxy(i, y);
		printw("-");
	}
}

void drawMenuList(int index,int selSec){
	int i;
	int begin = index * menu.itemWidth;
	int bg,tc1,tc2;

	drawNoTopWindow(begin,screen.mrey + 1,begin + menu.items[index].width,screen.mrey + 1 + menu.items[index].num,
		colorsln.mr_tc_1);

	for(i = 0; i < menu.items[index].num; ++i){
		if (selSec == i){
			tc1 = colorsln.mr_s_tc_1; tc2 = colorsln.mr_s_tc_2;
		}else{
			tc1 = colorsln.mr_tc_1; tc2 = colorsln.mr_tc_2;
		}
		printBg(begin + 1,screen.mrey + 1 + i,begin + menu.items[index].width - 1,screen.mrey + 1 + i,tc1);

		drawItem(begin + 2, screen.mrey + 1 + i,menu.items[index].section[i],tc2,tc1);
	}
}

void drawSelectMenu(int index,int selSec){
	drawItem(2 + screen.mrx + index * menu.itemWidth,screen.mry,
		menu.items[index].name,colorsln.mr_s_tc_2,colorsln.mr_s_tc_1);
	drawMenuList(index,selSec);
}

void drawMenu(){
	int i = 0;

	printBg(screen.mrx,screen.mry,screen.mrex,screen.mrey,colorsln.mr_tc_1);

	for(i = 0; i < menu.num; ++i){
		drawItem(MENUAHEAD + screen.mrx + i * menu.itemWidth,screen.mry,
			menu.items[i].name,colorsln.mr_tc_2,colorsln.mr_tc_1);
	}
}

void drawEditFrame(){
	int i;
	setcolor(colorsln.er_tc);
	for(i = screen.erx; i < screen.erex; ++i){
		gotoxy(i,screen.ery);
		printw("=");
		//printw("%c",205);
	}
	for(i = screen.erx; i < screen.erex; ++i){
		gotoxy(i,screen.erey);
		printw("-");
		//printw("%c",196);
	}
	gotoxy((screen.erex-screen.erx-6)/2 + screen.erx,screen.ery);
	printw("shedit");
}
void setCurser(int x,int y){
	gotoxy(x,y);
}

void drawElement(Element *el){
	if (el == NULL || el->father == NULL){
		printw("There is no father of element or no element!\n");
		return;
	}
	int i = 0;

	switch(el->father->type){
		case NORMAL:
		case SEPARATE:
		case NEWLINE:
			setcolor(colorsln.er_tc);
			break;
		case FUNCTION:
			setcolor(colorsln.er_f_ktc);
			break;
		case OPERATOR:
			setcolor(colorsln.er_o_ktc);
			break;
		case STRING:
			setcolor(colorsln.er_c_tc_1);
			break;
		case STRING_DOT:
			setcolor(colorsln.er_c_tc_2);
			break;
		case EXPLAIN:
			setcolor(colorsln.er_e_tc);
			break;
		case KEYWORD:
			setcolor(colorsln.er_v_ktc);
			break;
	}
	if (el->isSelected == TRUE)
		setcolor(colorsln.er_s_tc_1);

	printw("%c",el->c);
	if (el == textInput.curElement){
		getyx(stdscr,screen.curY,screen.curX);
	}
}


void drawEditRoom(){

	printBg(screen.erx,screen.ery,screen.erex,screen.erey,colorsln.er_tc);

	screen.curY = screen.ery + 1;
	screen.curX = screen.erx;
	gotoxy(screen.erx,screen.ery+1);
	setcolor(colorsln.er_tc);
	Element *el = textInput.screenBeginElement;
	while(el != textInput.screenEndElement){
	  	drawElement(el);
		el = el->next;
	}
	Point *p = textInput.breakpoints;
	while (p != NULL){
		gotoxy(screen.width-1,p->ln-1);
		setcolor(colorsln.er_w_tc);
		printw("*");
		p = p->next;
	}
	drawEditFrame();
}

void drawStatusRoom(){
	printBg(screen.srx,screen.sry,screen.srex,screen.srey,colorsln.sr_tc_1);


	if (isSystemState(InDebug)){
		setcolor(colorsln.sr_tc_1);
		gotoxy(screen.srx + 1,screen.sry);
		printw("%s",shSystem.status);
	}else{
		setcolor(colorsln.sr_tc_1);
		gotoxy(screen.srx + 1,screen.sry);
		printw("Menu - F1");
		setcolor(colorsln.sr_tc_2);
		gotoxy(screen.srex * 0.6,screen.sry);
		printw("Ln");
		setcolor(colorsln.sr_tc_3);
		gotoxy(screen.srex * 0.6 + 3,screen.sry);
		printw("%d",textInput.curLn);
		setcolor(colorsln.sr_tc_2);
		gotoxy(screen.srex * 0.7,screen.sry);
		printw("Col ");
		setcolor(colorsln.sr_tc_3);
		gotoxy(screen.srex * 0.7 + 4,screen.sry);
		printw("%d",textInput.curCol);
	}
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
	storeWindow(0,0,screen.width,20);

	drawSelectMenu(shSystem.menuIndex,shSystem.menuSection);

}

void drawInputDialog(char *str,int len,int w,int h){
	int x,y,ew,i;
	x = (screen.width - w) / 2;
	y = (screen.height - h) / 2 - 1;
	ew = w * 0.618;

	storeWindow(x,y,w+1,h+1);
	drawWindow(x,y,w+x,h+y,colorsln.mr_tc_1);

	setcolor(colorsln.mr_tc_1);
	mvprintw(y + 1,x + (w - len)/2,"%s",str);

	x += (w - ew)/2;
	y += h/2;
	mvprintw(y, x - 5,"Path:");
	printBg(x, y, x + ew, y,colorsln.er_tc);

	i = textInput.tmpCur - ew + 1;
	i = i > 0 ? i : 0;
	mvprintw(y,x,"%s",textInput.tmpStr + i);
	getyx(stdscr,screen.curY,screen.curX);
}

void drawMessageDialog(char *title,int tlen,char *str,int len,int w,int h){
	int x,y;
	x = (screen.width - w) / 2;
	y = (screen.height - h) / 2 - 1;

	storeWindow(x,y,w+1,h+1);
	drawWindow(x,y,w+x,h+y,colorsln.mr_tc_1);

	setcolor(colorsln.mr_tc_1);
	mvprintw(y + 1,x + (w - tlen)/2,"%s",title);

	mvprintw(y + 3,x + len,"%s",str);
}
void drawSaveDialog(){
	drawInputDialog("Save",4,35,7);
}

void drawLoadDialog(){
	drawInputDialog("Load",4,35,7);
}
void drawManualDialog(){
	drawMessageDialog("Manual",6,
		"Menu - F1\nRun - F5\nStep Over - F10\nSet Breakpoint - F9\nSelect - Ctrl+L\nSelectAll - Ctrl+A\nCut - Ctrl+X\nCopy - Ctrl+C\nPaste - Ctrl+V\nSave - Ctrl+S",3,35,15);
}
void drawAboutDialog(){
	drawMessageDialog("Author",6,
		"lxdfigo@gmail.com",7,25,5);
}

void drawMenuStaff(){
	switch(shSystem.subState){
		case InDefault:
			curs_set(0);
			updateMenu();
			break;
		case InSave:
			drawSaveDialog();
			break;
		case InLoad:
			drawLoadDialog();
			break;
		case InAbout:
			curs_set(0);
			drawAboutDialog();
			break;
		case InManual:
			curs_set(0);
			drawManualDialog();
			break;
	}
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
			drawMenuStaff();
			break;
		case InTextEdit:
			drawEditRoom();
			drawStatusRoom();
			break;
	}
	setCurser(screen.curX,screen.curY);
	refresh();
}
