#ifndef _EDIT_SHEDIT
#define _EDIT_SHEDIT
#include "basic.h"


void initEditModule();
BOOL inputHandler();
void destroyEditModule();

void doMenu();

BOOL delWord(Word *word);
BOOL clearText();
void addchar(char input);
BOOL clearElements();
int delElement(Element *element);
Element *createElement(char input);
Element *copyElement(Element *input);
Word* createNewWord();
void rebuildWords(Word *w);

void eraseSelected(Element *begin,Element *end);
void loadFileinBuffer(char *filename);

void doCut();
void doCopy();
void doPaste();
void doSearch();
void doDelete();

void doRun();
void doStep();
void doSetBreakpoint();
#endif
