//
//  menufuncs.c
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

void runCommand(char *command,char *result){
	FILE *fp;
	if ((fp = popen (command, "r")) == NULL)	{
		return;
	}
	int rs = 0;
	int tmp = 0;
	do{
		tmp = fgets(result + rs, sizeof(char) * TEXTLENSIZE - rs, fp);
		rs += tmp;
	}while(rs < TEXTLENSIZE && tmp == 0);
	pclose (fp);
}

void doUndo(){

}


void doCut(){
	if (textInput.selected_begin == NULL) return;

	textInput.curElement = textInput.selected_begin->previous;

	if (textInput.selected_begin == textInput.beginElement){
		textInput.beginElement = textInput.selected_begin->next;
	}

	if (textInput.selected_begin->previous != NULL){
		textInput.selected_begin->previous->next = textInput.selected_end->next;
	}

	if (textInput.selected_end->next != NULL){
		textInput.selected_end->next->previous = textInput.selected_begin->previous;
	}

	//rebuildWords(textInput.selected_begin->previous->father);


	textInput.copy_begin = textInput.selected_begin;
	textInput.copy_end = textInput.selected_end;

	resetState();
}
void copyElements(Element **begin,Element **end){
	if (*begin == NULL || *end == NULL) return;

	Element *b = *begin;
	Element *e = *end;
	Element *head = createElement(b->c);
	Element *cur = head;
	int count = 1;
	b = b->next;
	while(b != e->next){
		Element *tmp = createElement(b->c);
		count++;

		cur->next = tmp;
		tmp->previous = cur;
		cur = tmp;
		b = b->next;
	}

	*begin = head;
	*end = cur;

	return;
}

void doCopy(){
	if (textInput.selected_begin == NULL) return;

	textInput.copy_begin = textInput.selected_begin;
	textInput.copy_end = textInput.selected_end;
	copyElements(&textInput.copy_begin,&textInput.copy_end);

	resetState();
}
void doPaste(){
	if (textInput.copy_begin == NULL) return;
	Element *el = textInput.curElement;
	if (el == NULL){
		el = textInput.beginElement;
		if (el != NULL)
			el->previous = textInput.copy_end;
		textInput.copy_end->next = el;
		textInput.beginElement = textInput.copy_begin;
	}else{
		if (el->next != NULL)
			el->next->previous = textInput.copy_end;
		textInput.copy_end->next = el->next;
		el->next = textInput.copy_begin;
		textInput.copy_begin->previous = el;
		//need to separate two words
	}
	textInput.curElement = textInput.copy_end;

	textInput.copy_begin = NULL;
	textInput.copy_end = NULL;
	resetState();
	//rebuildWords(textInput.headWord);
}
void doSearch(){
	printw("do Search\n");
}
void doEdit(){
	switch(shSystem.menuSection){
		case 0:
			doCut();
			break;
		case 1:
			doCopy();
			break;
		case 2:
			doPaste();
			break;
		case 3:
			doSearch();
			break;
	}
}
Element* getCurCommand(char *command,Element *begin){
	int i = 0;
	Element *el = begin;
	if (el == NULL) el = textInput.beginElement;

	while(el != NULL && el->c == '\n') 
		el = el->next;
	while(el != NULL && el->c != '\n'){
		command[i] = el->c;
		el = el->next;
		i++;
	}
	command[i] = '\n';
	return el;
}
void doRun(){
	int i = 0;
	int ln = getElementLine(textInput.curCommand);
	Point *p = textInput.breakpoints;
	unsigned int count = -1;
	while (p != NULL){
		if (p->ln > ln){
			count = p->ln - ln;
			break;
		}
		p = p->next;
	}
	for(i = 0; i < count && textInput.curCommand != NULL; ++i){
		doStep();
	}

}
void doStep(){
	setSystemState(InDebug);
	char command[TEXTLENSIZE];
	textInput.curCommand = getCurCommand(command,textInput.curCommand);
	runCommand(command,shSystem.status);
}
void doSetBreakpoint(){
	Point * p = textInput.breakpoints;

	while (p != NULL && p->ln < textInput.curLn) p = p->next;

	if (p != NULL && p->ln == textInput.curLn){
		if (p->next != NULL) p->next->previous = p->previous;
		if (p->previous != NULL) p->previous->next = p->next;
		if (textInput.breakpoints == p)
			textInput.breakpoints = p->next;
		free(p);
	}else{
		Point *ins = (Point *)malloc(sizeof(Point));
		ins->ln = textInput.curLn;
		ins->col = textInput.curCol;
		ins->next = NULL;
		ins->previous = NULL;
		if (p != NULL){
			if (p->previous != NULL) p->previous->next = ins;
			p->previous = ins;
			ins->previous = p->previous;
			ins->next = p;
		}
		if (p == textInput.breakpoints)
			textInput.breakpoints = ins;
	}
}
void doDebug(){
	switch(shSystem.menuSection){
		case 0:
			doRun();
			break;
		case 1:
			doStep();
			break;
		case 2:
			doSetBreakpoint();
			break;
	}
	resetState();
}
void doAbout(){
	setSubState(InAbout);
}
void doManual(){
	setSubState(InManual);
}
void doHelp(){
	switch(shSystem.menuSection){
		case 0:
			doManual();
			break;
		case 1:
			doAbout();
			break;
	}
}


void doNewFile(){
	clearText();
	resetState();
}

void doSave(){
	setSubState(InSave);
	memset(textInput.tmpStr,0,sizeof(textInput.tmpStr));
	memcpy(textInput.tmpStr,shSystem.fileName,sizeof(textInput.tmpStr));
	textInput.tmpCur = 0;
}

void doLoad(){
	setSubState(InLoad);
	memset(textInput.tmpStr,0,sizeof(textInput.tmpStr));
	textInput.tmpCur = 0;
}

void doExit(){
	setSystemState(InQuit);
	shSystem.isQuit = TRUE;
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
