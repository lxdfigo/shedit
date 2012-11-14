#ifndef _BASIC_SHEDIT
#define _BASIC_SHEDIT

#define BOOL char
//#define TRUE 1
//#define FALSE 0

enum SystemState{
	InMenu,
	InTextEdit,
	InDebug,
	InQuit,
	InLoad,
	InSave,
	InDefault
};

typedef struct {
	int menuIndex;
	int menuSection;
	int textX;
	int textY;
	BOOL isQuit;
	enum SystemState lastState;
	enum SystemState state;
}SystemStatus;

enum WordType{
	FUNCTION,
	KEYWORD,
	OPERATOR,
	NORMAL,
	STRING,
	STRING2,
	SELECTED,
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
	char tmpstr[1024];
	short tmpcur;
	char *buffer;
	Word *head;
	Word *current;
}TextInput; 

extern TextInput textInput;
extern SystemStatus shSystem;

#endif