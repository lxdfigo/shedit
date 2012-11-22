#include "edit.h"

void runCommand(char *command,char *result){
	FILE *fp;
	if ((fp = popen (command, "r")) == NULL)	{
		return;
	}
	char tmp[1024];
	while (fgets(tmp, sizeof(char) * 1024, fp) != NULL){
		memcpy(result,tmp,sizeof(tmp));
	}
	pclose (fp);
}

void doUndo(){

}


void doCut(){
	if (textInput.selected_begin == NULL) return;
	eraseSelected(textInput.selected_begin,textInput.selected_end);

	textInput.curElement = textInput.selected_begin->previous;

	if (textInput.selected_begin->previous != NULL){
		textInput.selected_begin->previous->next = textInput.selected_end->next;
		textInput.selected_begin->previous->father->end = textInput.selected_end->next;
	}
	textInput.selected_begin->father->begin = textInput.selected_begin;

	if (textInput.selected_end->next != NULL){
		textInput.selected_end->next->previous = textInput.selected_begin->previous;
		textInput.selected_end->next->father->begin = textInput.selected_end->next;
	}
	textInput.selected_end->father->end = textInput.selected_end->next;

	if (textInput.selected_begin->previous != NULL){
		rebuildWords(textInput.selected_begin->previous->father);
	}else if (textInput.selected_end->next != NULL){
		rebuildWords(textInput.selected_end->next->father);
	}

	textInput.copy_begin = textInput.selected_begin;
	textInput.copy_end = textInput.selected_end;
	textInput.selected_begin = NULL;
	textInput.selected_end = NULL;
	if (shSystem.state == InSelect)
		setSystemState(InDefault);
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
	end = &cur;
	begin = &head;

	//set father of all elements
	Word *fat = createNewWord();
	fat->begin = head;
	fat->end = cur->next;
	fat->type = NORMAL;
	fat->count = count;
	Element *el = head;
	while(el != cur->next){
		el->father = fat;
		el = el->next;
	}
}

void doCopy(){
	if (textInput.selected_begin == NULL) return;
	eraseSelected(textInput.selected_begin,textInput.selected_end);

	textInput.copy_begin = textInput.selected_begin;
	textInput.copy_end = textInput.selected_end;
	copyElements(&textInput.copy_begin,&textInput.copy_end);

	textInput.selected_begin = NULL;
	textInput.selected_end = NULL;
	if (shSystem.state == InSelect){
		setSystemState(InDefault);
	}
}
void doPaste(){
	if (textInput.copy_begin == NULL) return;
	Element *el = textInput.curElement;
	if (el == NULL){
		el = textInput.headWord->begin;
		if (el != NULL)
			el->previous = textInput.copy_end;
		textInput.copy_end = el;
		textInput.headWord = textInput.copy_begin->father;
		rebuildWords(textInput.headWord);
	}else{
		if (el->next != NULL)
			el->next->previous = textInput.copy_end;
		textInput.copy_end->next = el->next;
		el->next = textInput.copy_begin;
		textInput.copy_begin->previous = el;
		textInput.curElement = textInput.copy_end;
		rebuildWords(el->father);
	}

	textInput.copy_begin = NULL;
	textInput.copy_end = NULL;
}
void doSearch(){
	printw("do Search\n");
}
void doEdit(){
	switch(shSystem.menuSection){
		//case 0:
		//	doUndo();
		//	break;
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
void doRun(){
	setSystemState(InDebug);
	char command[1024];
	char result[1024];
	Element *el = textInput.headWord->begin;
	int i = 0;
	while(el != NULL && el->c != '\n'){
		command[i] = el->c;
		el = el->next;
		i++;
	}
	command[i] = '\n';
	runCommand(command,result);
	printw("%s",result);
}
void doStep(){

}
void doSetBreakpoint(){
	Point * p = textInput.breakpoints;
	while (p != NULL && p->ln < textInput.curLn) p = p->next;

	if (p != NULL && p->ln == textInput.curLn){
		if (p->next != NULL) p->next->previous = p->previous;
		if (p->previous != NULL) p->previous->next = p->next;
		free(p);
	}else{
		Point *ins = malloc(Point);
		ins->ln = textInput.curLn;
		ins->col = textInput.curCol;
		 p->previous = ins;
		if (p->previous != NULL) p->previous->next = ins;
		ins->previous = p->previous;
		ins->next = p;
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
}
void doAbout(){
	setSystemState(InAbout);
}
void doManual(){
	setSystemState(InManual);
}
void doHelp(){
	switch(shSystem.menuSection){
		case 0:
			doManual();
		case 1:
			doAbout();
			break;
	}
}


void doNewFile(){
	clearText();
}

void doSave(){
	setSystemState(InSave);
	if (shSystem.fileName[0] != 0){
		memcpy(textInput.tmpStr,shSystem.fileName,sizeof(textInput.tmpStr));
	}else{
		memset(textInput.tmpStr,0,sizeof(textInput.tmpStr));
	}
	textInput.tmpCur = 0;
}

void doLoad(){
	setSystemState(InLoad);
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
