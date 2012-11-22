
#include "ui.h"
#include "edit.h"
#include "controller.h"
//#include <io.h>

int main(int argc, char** argv){

	if (argc > 2){
		printw("Too many parameters!\n");
		return 0;
	}

	initCurses();
	initEditModule();
	initUIModule();

	if (argc == 2){
		memcpy(shSystem.fileName,argv[1],sizeof(shSystem.fileName));
		if (access(argv[1], 0) == 0){
			loadFileinBuffer(argv[1]);
		}
	}

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
