//
//  controller.h
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
#ifndef _CONTROLLER_SHEDIT
#define _CONTROLLER_SHEDIT
#include "basic.h"

void initCurses();
void destroyCurses();

void setSubState(enum SubState state);
BOOL isSubState(enum SubState state);

void setSystemState(enum SystemState state);
void revertSystemState();
BOOL isSystemState(enum SystemState state);
#endif