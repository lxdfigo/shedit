//
// shedit.c
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
#include "ui.h"
#include "edit.h"
#include "controller.h"

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
