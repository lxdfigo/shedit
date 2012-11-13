#ifndef _CONTROLLER_SHEDIT
#define _CONTROLLER_SHEDIT

enum SystemState{
	InMenu,
	InTextEdit,
	InDebug,
	InQuit,
	InDefault
};

typedef struct {
	int menuIndex;
	int menuSection;
	int textX;
	int textY;
	int isQuit;
	enum SystemState lastState;
	enum SystemState state;

}SystemStatus;

enum WordType{
	FUNCTION,
	PARAMER,
	OPERATOR,
	NORMAL,
	STRING,
	EXPLAIN
};

struct _Word{
	enum WordType type;
	long begin,end;
	struct _Word *next;
	struct _Word *previous;
};

typedef struct _Word Word;

typedef struct{
	long cur;
	long length;
	long printOffset;
	char *buffer;
	Word *head;
	Word *current;
}TextInput; 

extern TextInput textInput;
extern SystemStatus shSystem;

void initCurses();
void destroyCurses();

#endif