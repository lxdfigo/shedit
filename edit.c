#include "edit.h"
#include <string.h>
#include <ncurses.h>


#define KEYNUM     14
struct KeyWord{
	int length;
	char *str;
}keywords[KEYNUM] = {{2,"if"},{2,"do"},{4,"done"},{5,"while"},{4,"case"},{4,"then"},{4,"else"},{4,"elif"}
,{2,"fi"},{4,"esac"},{4,"echo"},{2,"ls"},{4,"more"},{8,"function"}};

Element *creatElement(char input){
	Element *el = malloc(sizeof(char));
	el->c = input;
	el->next = NULL;
	el->previous = NULL;
	el->father = NULL;
	return el;
}
Element * appNewElement(Element *element,char input){
	Element *el = creatElement(input);
	if (element == NULL){
		el->father = textInput.head;
		el->father->head = el;
	}else{
		el->previous = element;
		el->next = element->next;
		element->next->previous = el;
		element->next = el;
		el->father = element->father;
	}
	el->father->count++;
	return el;
}

Word* createNewWord(){
	Word *w = malloc(sizeof(Word));
	w->begin = NULL;
	w->end = NULL;
	w->next = NULL;
	w->previous = NULL;
	w->type = NORMAL;
	w->count = 0;
	return w;
}

Word* appNewWord(Word *word){
	Word *w = createNewWord();
	w->next = word->next;
	w->previous = word;
	word->next->previous = w;
	word->next = w;

	return w;
}

void delElement(Element *element){
	Word *word = element->father;
	if (element == word->begin){
		word->begin = element->next;
	}
	element->previous->next = element->next;
	free(element);
	element->father->count--;
	if (element->father->count == 0){
		delWord(element->father);
	}
}

void clearWord(Word *word){
	Element *el = word->begin;
	while (el != word->end){
		Element *tmp = el;
		el = el->next;
		free(tmp);
	}
	word->begin = NULL;
	free(word);
}

void delWord(Word *word){
	if (word->next != NULL)
		word->next->previous = word->previous;
	if (word->previous != NULL)
		word->previous->next = word->next;
	clearWord(word);
}

void clearWords(){
	Word *w = textInput.head;
	while (w != NULL){
		Word *tmp = w;
		w = w->next;
		clearWord(tmp);
	}
	textInput.head = NULL;
}
void clearText(){
	clearWords();
	textInput.head = createNewWord();
	textInput.curElement = NULL;
}


BOOL isFunction(Word *word){
	return FALSE;
}
BOOL isOperator(Word *word){
	Element *el = word->begin;
	if (el->next == NULL && (
		el->c == '*' || 
		el->c == '-' ||
		el->c == '+' ||
		el->c == '=' ||
		el->c == '/' ||
		el->c == '|' ||
		el->c == '&'
		)){
			return TRUE;
	}
	return FALSE;
}
BOOL isKeyWord(Word *word){
	int i,j;
	for(i = 0; i < KEYNUM; ++i){
		BOOL sign = TRUE;
		if (word->count == keywords[i].length){
			Element * el = word->begin;
			for(j = 0; j < word->count; ++j){
				if (el == NULL || keywords[i].str[j] != el->c){
					sign = FALSE;
					break;
				}
				el = el->next;
			}
			if (sign == TRUE) return TRUE;
		}
	}
	return FALSE;
}
BOOL isString(Word *word){
	Element *el = word->begin;
	if (el->c == '\"')
		return TRUE;
	return FALSE;
}
BOOL isStringDot(Word *word){
	Element *el = word->begin;
	if (el->c == '\'')
		return TRUE;
	return FALSE;
}
BOOL isExplain(Word *word){
	Element *el = word->begin;
	if (el->c == '#')
		return TRUE;
	return FALSE;
}
BOOL isSeparate(Word *word){
	Element *el = word->begin;
	if (el->c == ' ' || el->c == '\t' || el->c == '\n')
		return TRUE;
	return FALSE;
}


void checkWord(Word * word){
	if (word->begin == NULL)
		return;

	if (isFunction(word)){
		word->type = FUNCTION;
	}else if (isOperator(word)){
		word->type = OPERATOR;
	}else if (isString(word)){
		word->type = STRING;
	}else if (isStringDot(word)){
		word->type = STRING_DOT;
	}else if (isKeyWord(word)){
		word->type = KEYWORD;
	}else if (isExplain(word)){
		word->type = EXPLAIN;
	}else if (isSeparate(word)){
		word->type = SEPARATE;
	}else{
		word->type = NORMAL;
	}
}

int checkSeparate(Word *word,char input){
	switch(word->type){
		case FUNCTION:
		case KEYWORD:
		case OPERATOR:
		case NORMAL:
			if (input == '#' || input == '\"' || input == '\''||
				input == '\n' || input == ' ' || input == '\t'){
					return 1;
			}
			break;
		case STRING:
			if (input == '\"'){
				return 2;
			}
			break;
		case STRING_DOT:
			if (input == '\''){
				return 2;
			}
			break;
		case SEPARATE:
			if (input != '\n' && input != ' ' & input != '\t'){
				return 1;
			}
			break;
		case EXPLAIN:
			if (input == '\n'){
				return 2;
			}
			break;
	}
	return 0;
}

void linkElementInWord(Element *el,Word *word){
	if (word->begin == word->end){
		word->begin = el;
		word->end = el->next;
	}else{
		word->end = el;
	}
	word->count++;
}

Word *rebuildWord(Word *word){
	Element *el = word->begin;

	Word *head = createNewWord();
	Word *curWord = head;
	curWord->begin = el;
	curWord->end = el->next;
	curWord->count++;

	el = el->next;
	while(el != word->end){
		int sign = checkSeparate(curWord,el->c);
		switch (sign){
			case 1:
				curWord = appNewWord(curWord);
			case 0:
				linkElementInWord(el,curWord);
				break;
			case 2:
				linkElementInWord(el,curWord);
				curWord = appNewWord(curWord);
				break;
		}
		checkWord(curWord);
		el = el->next;
	}
	word->previous->next = head;
	curWord->next = word->next;
	word->next->previous = curWord;
	head->previous = word->previous;
	free(word);
	return curWord;
}

BOOL isCombine(Word *w1,Word*w2){
	BOOL result = FALSE;
	switch (w1->type){
		case FUNCTION:
		case KEYWORD:
		case OPERATOR:
		case NORMAL:
			if (w2->type != SEPARATE)
				result = TRUE;
			break;
		case STRING:
			if (w2->type == STRING)
				result = TRUE;
			break;
		case STRING_DOT:
			if (w2->type == STRING_DOT)
				result = TRUE;
			break;
		case SEPARATE:
			if (w2->type == SEPARATE)
				result = TRUE;
			break;
		case EXPLAIN:
			result = TRUE;
			break;
	}
	if (result == TRUE){
		w1->next = w2->next;
		if (w2->next != NULL)
			w2->next->previous = w1;
		w1->end = w2->end;
		free(w2);
	}
	return result;
}

void rebuildWords(Word *word){
	Word *word;
	do{
		word = rebuildWord(word);
	}while (isCombine(word,word->next) == TRUE);
}

void addCharInBuffer(char input){
	textInput.curElement = appNewElement(textInput.curElement,input);
	rebuildWords(textInput.curElement->father);
}

void addCharInTemp(char input){
	textInput.tmpStr[textInput.tmpCur] = input;
	textInput.tmpCur++;
}

void addchar(char input){
	switch(shSystem.state){
		case InSave:
		case InLoad:
			addCharInTemp(input);
			break;
		case InDefault:
			addCharInBuffer(input);
			break;
	}
}
