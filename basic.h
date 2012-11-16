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
	InAbout,
	InDefault
};

typedef struct {
	int menuIndex;
	int menuSection;
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
	STRING_DOT,
	SELECTED,
	SEPARATE,
	EXPLAIN
};

struct _Word;

typedef struct _Element{
	char c;
	struct _Element *next;
	struct _Element *previous;
	struct _Word *father;
}Element;

typedef struct _Word{
	enum WordType type;
	Element *begin;
	Element *end;
	int count;
	struct _Word *next;
	struct _Word *previous;
}Word;

typedef struct{
	char tmpStr[1024];
	short tmpCur;
	Word *head;
	Element *curElement;
}TextInput; 

extern TextInput textInput;
extern SystemStatus shSystem;

#endif