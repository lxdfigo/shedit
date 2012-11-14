#include "edit.h"
#include "controller.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ncurses.h>

#define KEYNUM     14
#define TEXTLENSIZE 1024
const char * tmpfilename = "/tmp/shedit.tmp";
FILE* tmpFile;
KeyWord keywords[KEYNUM] = {{2,"if"},{2,"do"},{4,"done"},{5,"while"},{4,"case"},{4,"then"},{4,"else"},{4,"elif"}
,{2,"fi"},{4,"esac"},{4,"echo"},{2,"ls"},{4,"more"},{8,"function"}
};

Word* addNewWord(){
	Word *w = malloc(sizeof(Word));
	w->begin = textInput.cur;
	w->end = w->begin;
	w->next = NULL;
	w->previous = textInput.current;
	w->type = NORMAL;
	textInput.current = w;
	return w;
}

void initEditModule(){
	tmpFile = fopen(tmpfilename,"rw");
	textInput.cur = 0;
	textInput.tmpstr = "";
	textInput.printOffset = 0;
	textInput.length = TEXTLENSIZE;
	textInput.buffer = malloc(TEXTLENSIZE);
	textInput.head = addNewWord();

}

void destroyEditModule(){
	fclose(tmpFile);
	free(textInput.buffer);
}
BOOL isSeparate(char input){
	Word *w = textInput.current;
	if (input == '\"' || input == '\''){
		if (w->type != EXPLAIN)
			return TRUE;
	}
	if (input == '\n'){
		if (w->type != STRING)
			return TRUE;
	}
	if (input == ' ' || input == '\t'){
		if (w->type != STRING && w->type != EXPLAIN)
			return TRUE;
	}
	if (input == '#'){
		if (w->type != STRING)
			return TRUE;
	}
	return FALSE;
}

BOOL isFunction(char *str,int b,int e){
	return FALSE;
}
BOOL isOperator(char *str,int b,int e){
	if (b == e - 1 && (
		str[b] == '*' || 
		str[b] == '-' ||
		str[b] == '+' ||
		str[b] == '=' ||
		str[b] == '/' ||
		str[b] == '|' ||
		str[b] == '&'
		)){
			return TRUE;
	}
	return FALSE;
}
BOOL isKeyWord(char *str,int b,int e){
	int i,j;
	for(i = 0; i < KEYNUM; ++i){
		BOOL sign = TRUE;
		if (e - b == keywords[i].length){
			for(j = b; j < e; ++j){
				if (keywords[i].str[j - b] != str[j]){
					sign = FALSE;
					break;
				}
			}
			if (sign == TRUE) return TRUE;
		}
	}
	return FALSE;
}
BOOL isString(char *str,int b,int e){
	if (str[b] == '\"' || str[b] == '\'')
		return TRUE;
	return FALSE;
}
BOOL isExplain(char *str,int b,int e){
	if (str[b] == '#')
		return TRUE;
	return FALSE;
}

void checkWord(Word * word){
	if (isFunction(textInput.buffer,word->begin,word->end)){
		word->type = FUNCTION;
	}else if (isOperator(textInput.buffer,word->begin,word->end)){
		word->type = OPERATOR;
	}else if (isString(textInput.buffer,word->begin,word->end)){
		word->type = STRING;
	}else if (isKeyWord(textInput.buffer,word->begin,word->end)){
		word->type = KEYWORD;
	}else if (isExplain(textInput.buffer,word->begin,word->end)){
		word->type = EXPLAIN;
	}else{
		word->type = NORMAL;
	}
}
void checkChar(char input){
	if (isSeparate(input)){
		Word * w = addNewWord();
		w->end++;
	}else{
		textInput.current->end++;
		checkWord(textInput.current);
	}
}

void addchar(char input){
	textInput.buffer[textInput.cur] = input;
	textInput.cur++;
	if (textInput.cur >= textInput.length){
		char *tmp = malloc(textInput.length + TEXTLENSIZE);
		(tmp,textInput.buffer,textInput.length);
		free(textInput.buffer);
		textInput.length += TEXTLENSIZE;
		textInput.buffer = tmp;
	}
	checkChar(input);
}

void doNewFile(){

}

void doSave(){
	shSystem.state = shSystem.lastState;
	shSystem.state = InSave;
	memset(shSystem.tmpstr,0,sizeof(shSystem.tmpstr));
}

void doLoad(){
	shSystem.state = shSystem.lastState;
	shSystem.state = InLoad;
}
void doEdit(){

}
void doDebug(){

}
void doHelp(){

}

void doFile(){
	switch(shSystem.menuSection){
		case 0:
			doNewFile();
			break;
		case 1:
			doLoad();
			break;
		case 2:
			doSave();
			break;
		case 3:
			doExit();
			break;
	}
}



void doMenu(){
	switch (shSystem.menuIndex){
		case 0:
			doFile();
			break;
		case 1:
			doEdit();
			break;
		case 2:
			doDebug();
			break;
		case 3:
			doHelp();
			break;
	}
}

void doExit(){
	shSystem.state= InQuit;
	shSystem.isQuit = TRUE;
}

void saveFile(){
	int i = 0;
	FILE *file = NULL;
	if (textInput.tmpstr[0] == 0){
		return;
	}
	file = fopen(textInput.tmpstr,"w+");
	if (file == NULL) return;
	fwrite(textInput.buffer,sizeof(char),textInput.cur,file);
	fclose(file);
}

void loadFile(){
	int i = 0;
	FILE *file = NULL;
	if (textInput.tmpstr[0] == 0){
		return;
	}
	file = fopen(textInput.tmpstr,"r+");
	if (file == NULL) return;
	//fwrite(textInput.buffer,sizeof(char),textInput.cur,file);
	fclose(file);

}

BOOL isValid(int input){
	if (input >= 32 && input <= 126){
		return TRUE;
	}
	return FALSE;
}
BOOL checkCommand(){
	if (shSystem.state == InMenu){
		doMenu();
		shSystem.state = shSystem.lastState;
		return TRUE;
	}else if (shSystem.state == InSave){
		saveFile();
		shSystem.state = shSystem.lastState;
		return TRUE;
	}else if (shSystem.state == InLoad){
		loadFile();
		shSystem.state = shSystem.lastState;
		return TRUE;
	}

	return FALSE;
}

BOOL inputHandler(){
	int input = getch();

	switch(input){
		case KEY_F(1):
			if (shSystem.state == InMenu){
				shSystem.state = shSystem.lastState;
				shSystem.lastState = InMenu;
			}else{
				shSystem.lastState = shSystem.state;
				shSystem.state = InMenu;
			}
			break;
		case KEY_UP:
			if (shSystem.state == InMenu){
				shSystem.menuSection--;
			}else if (shSystem.state == InDefault){
				shSystem.textY--;
			}
			break;
		case KEY_DOWN:
			if (shSystem.state == InMenu){
				shSystem.menuSection++;
			}else if (shSystem.state == InDefault){
				shSystem.textY++;
			}
			break;
		case KEY_LEFT:
			if (shSystem.state == InMenu){
				shSystem.menuIndex++;
			}else if (shSystem.state == InDefault){
				shSystem.textX++;
			}
			break;
		case KEY_RIGHT:
			if (shSystem.state == InMenu){
				shSystem.menuIndex--;
			}else if (shSystem.state == InDefault){
				shSystem.textX--;
			}
			break;
		case KEY_ENTER:
			if (!checkCommand()){
				addchar(input);
			}
			break;
		case KEY_EXIT:
			doExit();
			break;
		default:
			if (isValid(input)){
				shSystem.state = InDefault;
				addchar(input);
			}
			break;
	}

	return shSystem.isQuit;
}
