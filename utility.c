#include "edit.h"
#include "controller.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ncurses.h>


#define TEXTLENSIZE 1024
const char * tmpfilename = "/tmp/shedit.tmp";
FILE* tmpFile;

void initEditModule(){
	tmpFile = fopen(tmpfilename,"rw");
	textInput.tmpCur = 0;
	memset(textInput.tmpStr,0,sizeof(textInput.tmpStr));
	textInput.head = createNewWord();
	textInput.curElement = NULL;

}

void destroyEditModule(){
	clearWords();
	fclose(tmpFile);
}

void saveFile(){
	int i = 0;
	FILE *file = NULL;
	if (textInput.tmpStr[0] != 0){
		file = fopen(textInput.tmpStr,"w+");
		if (file == NULL) return;
		Word *w = textInput.head;
		while(w != NULL){
			Element *el = w->begin;
			while(el != NULL){
				fputc(el->c,file);
				el = el->next;
			}
			w = w->next;
		}
		fclose(file);
	}
	revertSystemState();
}

void loadFile(){
	int i = 0,j = 0;
	FILE *file = NULL;
	if (textInput.tmpStr[0] != 0){
		file = fopen(textInput.tmpStr,"r+");
		if (file == NULL) return;
		clearText();
		char buf[TEXTLENSIZE];
		while(true){
			j = fread(buf,sizeof(char),TEXTLENSIZE,file);
			if (j == EOF) break;
			for(i = 0; i < j; ++i){
				addchar(buf[i]);
			}
		}
		fclose(file);
	}
	revertSystemState();
}

BOOL isValid(int input){
	if (input == '\n' || input == ' '  || input == '\t'|| (input >= 32 && input <= 126)){
		return TRUE;
	}
	return FALSE;
}
void checkCommand(){
	if (shSystem.state == InMenu){
		doMenu();
	}else if (shSystem.state == InSave){
		saveFile();
	}else if (shSystem.state == InLoad){
		loadFile();
	}else if (shSystem.state == InAbout){
		revertSystemState();
	}else{
		addchar('\n');
	}
}
void checkMenu(){
	if (shSystem.state == InMenu){
		revertSystemState();
	}else{
		setSystemState(InMenu);
	}
}

void doDelete(){
	if (shSystem.state == InDefault){
		Element *el = textInput.curElement;
		if (el != NULL){
			Word *word = el->father;
			int sign = word->count;
			textInput.curElement = el->next;
			delElement(el);
			if (sign > 1){
				rebuildWords(word);
			}else{
				rebuildWords(word->previous);
			}
		}
	}
}

void doBackspace(){
	if (shSystem.state == InDefault){
		Element *el = textInput.curElement;
		if (el != NULL && el->previous != NULL){
			Word *word = el->previous->father;
			int sign = word->count;
			delElement(el->previous);
			if (sign > 1){
				rebuildWords(word);
			}else{
				rebuildWords(word->previous);
			}
		}
	}
}

void moveUp(){
	if (shSystem.state == InMenu){
		shSystem.menuSection--;
	}else if (shSystem.state == InDefault){
		int l1 = COLS;
		int n1 = 0;
		Element *el = textInput.curElement;
	}
}

void moveDown(){
	if (shSystem.state == InMenu){
		shSystem.menuSection++;
	}else if (shSystem.state == InDefault){
	}
}

void moveLeft(){
	if (shSystem.state == InMenu){
		shSystem.menuIndex--;
	}else if (shSystem.state == InDefault){
		if (textInput.curElement->previous != NULL){
			textInput.curElement = textInput.curElement->previous;
		}
	}
}

void moveRight(){
	if (shSystem.state == InMenu){
		shSystem.menuIndex++;
	}else if (shSystem.state == InDefault){
		if (textInput.curElement->next != NULL)
			textInput.curElement = textInput.curElement->next;
	}
}

BOOL inputHandler(){
	int input = getch();

	switch(input){
		case KEY_F(1):
			checkMenu();
			break;
		case '\n'://KEY_ENTER:
			checkCommand();
			break;
		case 27://KEY_EXIT:
			if (shSystem.state != InDefault){
				setSystemState(InDefault);
			}
			//doExit();
			break;
		case 127://KEY_DELETE
			doDelete();
			break;
		case 8://KEY_BACKSPACE
			doBackspace();
			break;
		case KEY_UP:
			moveUp();
			break;
		case KEY_DOWN:
			moveDown();
			break;
		case KEY_LEFT:
			moveLeft();
			break;
		case KEY_RIGHT:
			moveRight();
			break;
		default:
			//printw("Input is: %d : %c\n",input,input);
			if (isValid(input)){
				addchar(input);
			}
			break;
	}

	return shSystem.isQuit;
}
