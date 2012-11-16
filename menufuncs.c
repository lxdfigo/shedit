#include "edit.h"
#include <string.h>
#include <ncurses.h>

void doUndo(){

}
void doCut(){

}
void doCopy(){

}
void doPaste(){

}
void doSearch(){

}
void doEdit(){
	switch(shSystem.menuSection){
		case 0:
			doUndo();
			break;
		case 1:
			doCut();
			break;
		case 2:
			doCopy();
			break;
		case 3:
			doPaste();
			break;
		case 4:
			doSearch();
			break;
	}
}
void doRun(){

}
void doSetBreakpoint(){

}
void doDelBreakpoint(){

}
void doDebug(){
	switch(shSystem.menuSection){
		case 0:
			doRun();
			break;
		case 1:
			doSetBreakpoint();
			break;
		case 2:
			doDelBreakpoint();
			break;
	}
}
void doAbout(){
	setSystemState(InAbout);
}
void doHelp(){
	switch(shSystem.menuSection){
		case 0:
			doAbout();
			break;
	}
}


void doNewFile(){
	clearText();
}

void doSave(){
	setSystemState(InSave);
	memset(textInput.tmpStr,0,sizeof(textInput.tmpStr));
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
