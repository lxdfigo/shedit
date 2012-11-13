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
struct winsize ttySize;  
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

	//init menu color
	csln->mr_bg = COLOR_WHITE;
	csln->mr_s_bg = COLOR_BLUE;
	csln->mr_tc_1 = COLOR_BLUE;
	csln->mr_tc_2 = COLOR_CYAN;
	csln->mr_s_tc_1 = COLOR_WHITE;
	csln->mr_s_tc_2 = COLOR_YELLOW;

	//init edit color
	csln->er_bg = COLOR_BLACK;
	csln->er_s_bg = COLOR_WHITE;
	csln->er_tc = COLOR_WHITE;
	csln->er_s_tc_1 = COLOR_BLACK;
	csln->er_s_tc_2 = COLOR_CYAN;
	csln->er_s_tc_3 = COLOR_MAGENTA; 
	csln->er_f_ktc = COLOR_GREEN;   
	csln->er_o_ktc = COLOR_YELLOW;  
	csln->er_v_ktc = COLOR_BLUE;   
	csln->er_e_tc = COLOR_BLACK;     
	csln->er_c_tc_1 = COLOR_MAGENTA;    
	csln->er_c_tc_2 = COLOR_CYAN;  
	csln->er_w_bg = COLOR_BLACK;   
	csln->er_w_tc = COLOR_RED;   


	//init status color
	csln->sr_bg = COLOR_WHITE;
	csln->sr_tc_1 = COLOR_WHITE;
	csln->sr_tc_2 = COLOR_RED;
	csln->sr_tc_3 = COLOR_BLUE;
}


//函数set_disp_mode用于控制是否开启输入回显功能  
//如果option为0，则关闭回显，为1则打开回显  
int set_disp_mode(int fd,int option) {  
	int err;  
	struct termios term;  
	if(tcgetattr(fd,&term)==-1){  
		perror("Cannot get the attribution of the terminal");  
		return 1;  
	}  
	if(option)  
		term.c_lflag|=ECHOFLAGS;  
	else  
		term.c_lflag &=~ECHOFLAGS;  
	err=tcsetattr(fd,TCSAFLUSH,&term);  
	if(err==-1 && err==EINTR){  
		perror("Cannot set the attribution of the terminal");  
		return 1;  
	}  
	return 0;  
}  


void initUIModule(){
	if (isatty(STDOUT_FILENO) == 0)  
		exit(1);  
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ttySize) < 0) {   
		perror("ioctl TIOCGWINSZ error");    
		exit(1);  
	}  
	//printf("%d rows, %d columns\n", ttySize.ws_row, ttySize.ws_col);  
	set_disp_mode(STDIN_FILENO,0);

	initColorSln(&colorsln);
}

void destroyUIModule(){
	//printf("\033[0m");
	set_disp_mode(STDIN_FILENO,1);
	if (inputWin != NULL){
		delwin(inputWin);
		inputWin = NULL;
		endwin();
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
	//printf("\033[%d;%dH",y,x);
}
void setbgtc(int bg,int tc){
	static int initnum = 1;
	init_pair(initnum, tc, bg);
	attron(COLOR_PAIR(initnum));

	if (initnum > 1000) initnum = 1;

	//printf("\033[%d;%dm\033[%d;%dm",bg,tc);

}
void drawItem(char *itemname,int c1,int c2,int bg){
	int i = 0;
	setbgtc(bg,c1);
	printw("%c",itemname[0]);
	setbgtc(bg,c2);
	printw("%s",itemname+1);
}

void printBg(int x,int y,int ex,int ey){
	int i,j;
	for(i = y; i <= ey; ++i){
		gotoxy(x,i);
		for(j = x; j <= ex; ++j){
			printw(" ");
		}
	}
}
void drawWindow(int x,int y,int ex,int ey,int bg,int tc){
	int i;
	setbgtc(bg,tc);
	printBg(x,y,ex,ey);
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

void drawMenuList(int index,int selSec){
	int i;
	int begin = index * menu.itemWidth;
	int bg,tc1,tc2;

	drawWindow(begin,mrey + 1,begin + menu.items[index].width,mrey + 1 + menu.items[index].num,
		colorsln.mr_bg,colorsln.mr_tc_1);

	for(i = 0; i < menu.items[index].num; ++i){
		if (selSec == i){
			bg = colorsln.mr_s_bg; tc1 = colorsln.mr_s_tc_1; tc2 = colorsln.mr_s_tc_2;
		}else{
			bg = colorsln.mr_bg; tc1 = colorsln.mr_tc_1; tc2 = colorsln.mr_tc_2;
		}
		setbgtc(bg,tc1);
		printBg(begin + 1,mrey + 1 + i,begin + menu.items[index].width - 1,mrey + 1 + i);
		gotoxy(begin + 2, mrey + 1 + i);
		drawItem(menu.items[index].section[i],tc2,tc1,bg);
	}
}

void drawSelectMenu(int index,int selSec){
	setbgtc(colorsln.mr_s_bg,colorsln.mr_s_tc_1);
	gotoxy(2 + mrx + index * menu.itemWidth,mry);
	drawItem(menu.items[index].name,colorsln.mr_s_tc_2,colorsln.mr_s_tc_1,colorsln.mr_s_bg);
	drawMenuList(index,selSec);
}

void drawMenu(){
	int i = 0;
	mrx = 0;
	mry = 0;
	mrex = ttySize.ws_col - 1;
	mrey = mry;
	setbgtc(colorsln.mr_bg,colorsln.mr_tc_1);
	printBg(mrx,mry,mrex,mrey);

	for(i = 0; i < menu.num; ++i){
		gotoxy(MENUAHEAD + mrx + i * menu.itemWidth,mry);
		drawItem(menu.items[i].name,colorsln.mr_tc_2,colorsln.mr_tc_1,colorsln.mr_bg);
	}
}

void drawEditFrame(){
	int i;
	erx = 0;
	ery = mrey + 1;
	erex = ttySize.ws_col-1;
	erey = ttySize.ws_row-2;

	setbgtc(colorsln.er_bg,colorsln.er_tc);
	printBg(erx,ery,erex,erey);

	setbgtc(colorsln.er_bg,colorsln.er_tc);
	for(i = erx; i < erex; ++i){
		gotoxy(i,ery);
		printw("=");
	}
	for(i = erx; i < erex; ++i){
		gotoxy(i,erey);
		printw("-");
	}
	gotoxy((erex-erx-6)/2 + erx,ery);
	printw("shedit");
}
void drawStatusFrame(){
	srx = 0;
	sry = ttySize.ws_row - 1;
	srex = ttySize.ws_col - 1;
	srey = sry;

	setbgtc(colorsln.sr_bg,colorsln.sr_tc_1);
	printBg(srx,sry,srex,srey);

}

void setCurser(int x,int y){
	gotoxy(erx + x,ery + 1 + y);
}

void drawWord(Word *w){
	int i = 0;
	if (w->type == NORMAL){
		setbgtc(colorsln.er_bg,colorsln.er_tc);
	}else if (w->type == FUNCTION){
		setbgtc(colorsln.er_bg,colorsln.er_f_ktc);
	}else if (w->type == OPERATOR){
		setbgtc(colorsln.er_bg,colorsln.er_o_ktc);
	}
	for(i = w->begin; i < w->end; ++i){
		printw("%c",textInput.buffer[i]);
	}
}

void drawEditRoom(){
	gotoxy(erx,ery+1);
	setbgtc(colorsln.er_bg,colorsln.er_tc);
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
	box(inputWin,0,0);
	overwrite(stdscr,inputWin);
}

void updateView(){
	overwrite(inputWin, stdscr);
	if (shSystem.state == InMenu){
		shSystem.menuIndex = shSystem.menuIndex < 0 ? 0 : shSystem.menuIndex;
		shSystem.menuIndex = shSystem.menuIndex >= menu.num ? menu.num - 1 : shSystem.menuIndex;

		shSystem.menuSection = shSystem.menuSection < 0 ? 0 : shSystem.menuSection;
		shSystem.menuSection = shSystem.menuSection >= menu.items[shSystem.menuIndex].num ? 
			menu.items[shSystem.menuIndex].num - 1 : shSystem.menuIndex;

		storeWindow(shSystem.menuIndex * menu.width,1,menu.items[shSystem.menuIndex].width + 1,menu.items[shSystem.menuIndex].num + 1);
		drawSelectMenu(shSystem.menuIndex,shSystem.menuSection);
	}
	if (shSystem.state == InDefault){
		drawEditRoom();
		drawStatusRoom();
		//setCurser(shSystem.textX,shSystem.textY);
	}
	refresh();
}