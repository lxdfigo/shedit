
#include "ui.h"
#include "edit.h"
#include "controller.h"

int main(){
	initCurses();
	initEditModule();
	initUIModule();

	resetView();
	while(!shSystem.isQuit){
		inputHandler();
		updateView();
	}

	destroyUIModule();
	destroyEditModule();
	destroyCurses();
	return 0;
}
