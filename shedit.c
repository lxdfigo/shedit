
#include "ui.h"
#include "edit.h"
#include "controller.h"

int main(){
	initCurses();
	initEditModule();
	initUIModule();

	resetView();
	do{
		inputHandler();
		updateView();
	}while(!shSystem.isQuit);

	destroyUIModule();
	destroyEditModule();
	destroyCurses();
	return 0;
}
