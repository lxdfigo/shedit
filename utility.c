//
//  utility.c
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
#include "edit.h"
#include "controller.h"


const char * tmpfilename = "/tmp/shedit.tmp";
FILE* tmpFile;

void initEditModule(){
	tmpFile = fopen(tmpfilename,"rw");
	textInput.tmpCur = 0;
	memset(textInput.tmpStr,0,sizeof(textInput.tmpStr));
	textInput.beginElement = NULL;
	textInput.curElement = NULL;
	textInput.selected_begin = NULL;
	textInput.selected_end = NULL;
	textInput.copy_begin = NULL;
	textInput.copy_end = NULL;
	textInput.breakpoints = NULL;
}

void destroyEditModule(){
	clearElements();
	fclose(tmpFile);
}

// Head Element is the last element of the last line
Element * findLineHead(Element *el,int *sub){
	int i = 0, n = 0;
	if (el == NULL || el->c == '\n') return el;
	
	while(el != NULL && el->c != '\n'){
		el = el->previous;
		n++;
	}
	*sub = n - n / screen.width * screen.width;
	n -= *sub;

	if (el == NULL){
		el = textInput.beginElement;
		n--;
	}

	for(i = 0; i < n; ++i)
		el = el->next;
	return el;
}
Element * findLineEnd(Element *el,int *sub){
	int i = 0;
	el = findLineHead(el,sub);
	for(i = 0; i < screen.width - 1; ++i){
		if (el == NULL){
			el = textInput.beginElement;
			if (el == NULL) return el;
		}else{
			if (el->next == NULL || el->next->c == '\n') break;
			el = el->next;
		}
	}
	return el;

}
int getElementLine(Element * el){
	if (el == NULL) return 1;
	int n = 1;
	el = findLineHead(el,&n);
	int count = 1;
	while(el != NULL){
		el = el->previous;
		el = findLineHead(el,&n);
		count++;
	}
	return count;
}
void updateScreenBeginElement(){
	int upline = screen.erey - screen.ery - 4;
	if (textInput.curElement == NULL){
		textInput.screenBeginElement = textInput.beginElement;
		textInput.curCol = 1;
		textInput.curLn = 1;
	}else{
		Element *el = textInput.curElement;
		int n = 0;
		el = findLineHead(el,&n);
		int Ln = 1;
		int Col = n + 1;
		Element *head = el;
		while(el != NULL){
			el = el->previous;
			el = findLineHead(el,&n);
			Ln++;
			if (Ln <= upline) head = el;
		}
		textInput.curCol = Col;
		textInput.curLn = Ln;

		if (head == NULL)
			textInput.screenBeginElement = textInput.beginElement;
		else
			textInput.screenBeginElement = head->next;
	}
	textInput.screenEndElement = NULL;
}
bool saveBufferInFile(char *filename){
	FILE *file = NULL;
	file = fopen(filename,"w+");
	if (file == NULL) return false;
	Element *el = textInput.beginElement;
	while(el != NULL){
		fputc(el->c,file);
		el = el->next;
	}
	fclose(file);
	return true;
}

bool loadFileinBuffer(char *filename){
	int i = 0,n = 0;
	FILE *file = NULL;
	file = fopen(filename,"r+");
	if (file == NULL) return false;

	clearText();
	char buf[TEXTLENSIZE];
	while(!feof(file)){
		n = fread(buf,sizeof(char),TEXTLENSIZE,file);
		for(i = 0; i < n; ++i){
			addchar(buf[i]);
		}
	}
	fclose(file);
	textInput.curCommand = NULL;
	updateScreenBeginElement();
	return true;
}
void saveFile(){
	if (saveBufferInFile(textInput.tmpStr)){
		memset(shSystem.fileName,0,sizeof(shSystem.fileName));
		memcpy(shSystem.fileName,textInput.tmpStr,sizeof(textInput.tmpStr));
	}
	setSystemState(InTextEdit);
}

void loadFile(){
	if (loadFileinBuffer(textInput.tmpStr)){
		memset(shSystem.fileName,0,sizeof(shSystem.fileName));
		memcpy(shSystem.fileName,textInput.tmpStr,sizeof(shSystem.fileName));
	}
	setSystemState(InTextEdit);
}

BOOL isValid(int input){
	if (input == '\n' || input == ' '  || input == '\t'|| (input >= 32 && input <= 126)){
		return TRUE;
	}
	return FALSE;
}
void checkMenu(){
	if (isSystemState(InMenu)){
		resetState();
	}else{
		setSystemState(InMenu);
		setSubState(InDefault);
	}
}
void checkMenuCommand(){
	switch(shSystem.subState){
		case InDefault:
			doMenu();
			break;
		case InSave:
			saveFile();
			break;
		case InLoad:
			loadFile();
			break;
		case InAbout:
		case InManual:
			checkMenu();
			break;
	}
}
void checkCommand(){
	switch(shSystem.state){
		case InMenu:
			checkMenuCommand();
			break;
		case InTextEdit:
			addchar('\n');
			break;
	}
}


void deleteSelectedWords(){
	if (textInput.selected_begin == NULL) return;
	textInput.curElement = textInput.selected_begin->previous;
	if (textInput.selected_begin == textInput.beginElement){
		textInput.beginElement = textInput.selected_end->next;
	}
	Element *el = textInput.selected_begin;
	while (el != textInput.selected_end){
		Element *tmp = el;
		el = el->next;
		delElement(tmp);
	}
	delElement(el);
	textInput.selected_begin = NULL;
	textInput.selected_end = NULL;
}

void doDelete(){
	if (!isSystemState(InTextEdit)) return;
	if (isSubState(InDefault)){
		Element *el = textInput.curElement;
		if (el == NULL){ 
			el = textInput.beginElement;
			textInput.beginElement = textInput.beginElement->next;
		}else{ 
			el = el->next;
		}
		delElement(el);
	}else if(isSubState(InSelect)){
		deleteSelectedWords();
		setSubState(InDefault);
	}
}

void doBackspace(){
	if (isSystemState(InTextEdit)){
		if (isSubState(InDefault)){
			Element *el = textInput.curElement;
			if (el != NULL){
				textInput.curElement = el->previous;
				if (el == textInput.beginElement){
					textInput.beginElement = textInput.beginElement->next;
				}
				delElement(el);
			}
		}else if(isSubState(InSelect)){
			deleteSelectedWords();
			setSubState(InDefault);
		}
	}else if (isSystemState(InMenu)){
		if (isSubState(InSave) || isSubState(InLoad)){
			int i =0;
			for (i = 1; i < TEXTLENSIZE; ++i){
				if (textInput.tmpStr[i] == '\0'){
					textInput.tmpStr[i-1] = '\0';
					textInput.tmpCur--;
					textInput.tmpCur = textInput.tmpCur < 0 ? 0 : textInput.tmpCur;
					break;
				}
			}
		}
	}
}

void eraseSelected(Element *begin,Element *end){
	if (begin == NULL || end == NULL) return;

	while(begin != end){
		begin->isSelected = FALSE;
		begin = begin->next;
	}
	begin->isSelected = FALSE;
}

void setSelected(Element *begin,Element *end){
	if (begin == NULL || end == NULL) return;

	while(begin != end){
		begin->isSelected = TRUE;
		begin = begin->next;
	}
	begin->isSelected = TRUE;
}
BOOL isElementAheadElement(Element *head, Element *behind){
	if (behind == NULL) return FALSE;
	if (head == NULL) return TRUE;

	do{
		behind = behind->previous;
		if (head == behind) return TRUE;
	}while(behind != NULL);

	return FALSE;
}

void getSelectElements(){
	if (!isSubState(InSelect)) return;

	eraseSelected(textInput.selected_begin,textInput.selected_end);
	textInput.selected_begin = NULL;
	textInput.selected_end = NULL;
	Element *el= textInput.curElement;
	if (isElementAheadElement(textInput.selected_center,el)){
		if (textInput.selected_center == NULL){
			textInput.selected_begin = textInput.beginElement;
		}else{
			textInput.selected_begin = textInput.selected_center->next;
		}
		textInput.selected_end = el;
	}else if (el != textInput.selected_center){
		if (el == NULL){
			textInput.selected_begin = textInput.beginElement;
		}else{
			textInput.selected_begin = el->next;
		}
		textInput.selected_end = textInput.selected_center;
	}
	setSelected(textInput.selected_begin,textInput.selected_end);
}


void moveUp(){
	if (isSystemState(InMenu)){
		shSystem.menuSection--;
	}else if (isSystemState(InTextEdit)){
		int n1 = 0, n2 = 0, i = 0, n = 0;
		if (textInput.curElement == NULL) return;
		Element *el = findLineHead(textInput.curElement,&n1);
		if (el != NULL){
			el = el->previous;
			el = findLineHead(el,&n2);
			n = n2 < n1 ? n2 : n1;
			if (el == NULL && n > 0){
				el = textInput.beginElement;
				n--;
			}
			
			for(i = 0; i < n; ++i){
				el = el->next;
			}
			textInput.curElement = el;
		}
		getSelectElements();
	}
}

void moveDown(){
	if (isSystemState(InMenu)){
		shSystem.menuSection++;
	}else if (isSystemState(InTextEdit)){
		int i = 0, n = 0;
		Element *el = textInput.curElement;
		el = findLineEnd(el,&n);
		if (el != NULL && el->next != NULL){
			el = el->next;
			for(i = 0; i < n; ++i){
				if (el->next == NULL || el->next->c == '\n') break;
				el = el->next;
			}
			textInput.curElement = el;
		}
		getSelectElements();
	}
}

void moveLeft(){
	if (isSystemState(InMenu)){
		shSystem.menuIndex--;
	}else if (isSystemState(InTextEdit)){
		if (textInput.curElement != NULL){
			textInput.curElement = textInput.curElement->previous;
		}
		getSelectElements();
	}
}

void moveRight(){
	if (isSystemState(InMenu)){
		shSystem.menuIndex++;
	}else if (isSystemState(InTextEdit)){
		if (textInput.curElement == NULL ){
			textInput.curElement = textInput.beginElement;
		}else if (textInput.curElement->next != NULL){
			textInput.curElement = textInput.curElement->next;
		}
		getSelectElements();
	}
}

void doPageUp(){
	if (!isSystemState(InTextEdit)) return;
	int i;
	int upline = screen.erey - screen.ery - 4;
	for(i = 0; i < upline; ++i){
		moveUp();
	}
	getSelectElements();
}

void doPageDown(){
	if (!isSystemState(InTextEdit)) return;
	int i;
	int upline = screen.erey - screen.ery - 4;
	for(i = 0; i < upline; ++i){
		moveDown();
	}
	getSelectElements();
}

void doSelect(){
	if (isSystemState(InTextEdit)){
		eraseSelected(textInput.selected_begin,textInput.selected_end);
		textInput.selected_begin = NULL;
		textInput.selected_end = NULL;
		textInput.selected_center = NULL;
		if (!isSubState(InSelect)){
			setSubState(InSelect);
			textInput.selected_center = textInput.curElement;
		}else{
			setSubState(InDefault);
			textInput.selected_center = NULL;
		}
	}
}
Element *getEndElement(Element *begin){
	if (begin == NULL) return NULL;
	while(begin->next != NULL){
		begin = begin->next;
	}
	return begin;
}

void selectedAll(){
	textInput.selected_begin = textInput.beginElement;
	textInput.selected_end = getEndElement(textInput.selected_begin);
}

void resetState(){
	eraseSelected(textInput.selected_begin,textInput.selected_end);
	textInput.selected_begin = NULL;
	textInput.selected_end = NULL;
	setSystemState(InTextEdit);
	setSubState(InDefault);
}
BOOL inputHandler(){
	int input = getch();

	switch(input){
		case KEY_F(1):
			checkMenu();
			break;
		case KEY_F(5):
			doRun();
			break;
		case KEY_F(9):
			doSetBreakpoint();
			break;
		case KEY_F(10):
			doStep();
			break;
		case '\n'://KEY_ENTER:
			checkCommand();
			break;
		case 27://KEY_EXIT:
			resetState();
			break;
		case 330://KEY_DELETE
			doDelete();
			break;
		case 263://KEY_BACKSPACE
			doBackspace();
			break;
		case 19://Ctrl+S
			saveBufferInFile(shSystem.fileName);
			break;
		case 24://Ctrl+X
			doCut();
			break;
		case 3://Ctrl+C
			doCopy();
			break;
		case 22://Ctrl+V
			doPaste();
			break;
		case 6://Ctrl+F
			doSearch();
			break;
		case 12://Ctrl+L
			doSelect();
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
		case 339://KEY_PAGEUP
			doPageUp();
			break;
		case 338://KEY_PAGEDOWN
			doPageDown();
			break;
		default:
			//printw("Input is: %d : %c\n",input,input);
			if (isValid(input)){
				addchar(input);
			}
			break;
	}
	updateScreenBeginElement();

	return shSystem.isQuit;
}
