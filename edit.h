#ifndef _EDIT_SHEDIT
#define _EDIT_SHEDIT
#include "basic.h"


void initEditModule();
BOOL inputHandler();
void destroyEditModule();

void doMenu();

void clearText();
void addchar(char input);
void clearWords();
void delElement(Element *element);
Element *creatElement(char input);
Word* createNewWord();
void rebuildWords(Word *w);

#endif
