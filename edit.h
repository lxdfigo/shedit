#ifndef _EDIT_SHEDIT
#define _EDIT_SHEDIT

typedef struct{
	int length;
	char *str;
}KeyWord;


void initEditModule();
BOOL inputHandler();
void destroyEditModule();


#endif
