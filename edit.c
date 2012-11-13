#include "edit.h"
#include "controller.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ncurses.h>

#define TEXTLENSIZE 1024
const char * tmpfilename = "/tmp/shedit.tmp";
FILE* tmpFile;

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
	textInput.printOffset = 0;
	textInput.length = TEXTLENSIZE;
	textInput.buffer = malloc(TEXTLENSIZE);
	textInput.head = addNewWord();

}

void destroyEditModule(){
	fclose(tmpFile);
	free(textInput.buffer);
}
int isSeparate(char input){
	Word *w = textInput.current;
	if (input == '\"'){
		if (w->type != EXPLAIN)
			return 1;
	}
	if (input == '\n'){
		if (w->type != STRING)
			return 1;
	}
	if (input == ' ' || input == '\t'){
		if (w->type != STRING && w->type == EXPLAIN)
			return 1;
	}
	return 0;
}

int isFunction(char *str,int b,int e){
	return 0;
}
int isOperator(char *str,int b,int e){
	if (b == e - 1 && (
		str[b] == '*' || 
		str[b] == '-' ||
		str[b] == '+' ||
		str[b] == '=' ||
		str[b] == '/' ||
		str[b] == '|' ||
		str[b] == '&'
		)){
			return 1;
	}
	return 0;
}
int isParameter(char *str,int b,int e){
	return 0;
}
int isString(char *str,int b,int e){
	return 0;
}
int isExplain(char *str,int b,int e){
	return 0;
}

void checkWord(Word * word){
	if (isFunction(textInput.buffer,word->begin,word->end)){
		word->type = FUNCTION;
	}else if (isOperator(textInput.buffer,word->begin,word->end)){
		word->type = OPERATOR;
	}else if (isString(textInput.buffer,word->begin,word->end)){
		word->type = STRING;
	}else if (isParameter(textInput.buffer,word->begin,word->end)){
		word->type = PARAMER;
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
void doEdit(){

}
void doDebug(){

}
void doHelp(){

}
void doFile(){
	switch(shSystem.menuSection){
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			shSystem.isQuit = 1;
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
int isValid(int input){
	if (input >= 32 && input <= 126){
		return 1;
	}
	return 0;
}

int inputHandler(){
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
				shSystem.menuIndex--;
			}else{

			}
			break;
		case KEY_DOWN:
			if (shSystem.state == InMenu){
				shSystem.menuIndex++;
			}else{

			}
			break;
		case KEY_LEFT:
			if (shSystem.state == InMenu){
				shSystem.menuSection++;
			}else{

			}
			break;
		case KEY_RIGHT:
			if (shSystem.state == InMenu){
				shSystem.menuSection--;
			}else{

			}
			break;
		case KEY_ENTER:
			if (shSystem.state == InMenu){
				doMenu();
			}
			break;
		case KEY_EXIT:
			shSystem.state= InQuit;
			shSystem.isQuit = 1;
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
