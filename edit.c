//
//  edit.c
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
#include "edit.h"


#define KEYNUM     14
#define OPERATORNUM 14
struct KeyWord{
	int length;
	char *str;
}keywords[KEYNUM] = {{2,"if"},{2,"do"},{4,"done"},{5,"while"},{4,"case"},{4,"then"},{4,"else"},{4,"elif"}
,{2,"fi"},{4,"esac"},{4,"echo"},{2,"ls"},{4,"more"},{8,"function"}};

char operators[OPERATORNUM] = {'+','/','-','*','=','(',')','<','>','%','^','!','&','|'};

Element *createElement(char input){
	Element *el = malloc(sizeof(Element));
	el->c = input;
	el->next = NULL;
	el->previous = NULL;
	el->father = NULL;
	el->isSelected = FALSE;
	return el;
}
Element *copyElement(Element *input){
	if (input == NULL) return NULL;
	Element *el = createElement(input->c);
	el->next = input->next;
	el->previous = input->previous;
	el->father = input->father;
	el->isSelected = input->isSelected;
	return el;
}
Element * appNewElement(Element *element,char input){
	Element *el = createElement(input);
	if (element == NULL){
		Word *word = textInput.headWord;
		textInput.headWord = createNewWord();
		el->father = textInput.headWord;
		el->father->begin = el;
		if (word != NULL){
			word->previous = textInput.headWord;
			textInput.headWord->next = word;
			el->next = word->begin;
			if (word->begin != NULL)
				word->begin->previous = el;
		}
		el->father->end = el->next;
	}else{
		el->previous = element;
		el->next = element->next;
		if (element->next != NULL)
			element->next->previous = el;
		element->next = el;
		el->father = element->father;
	}
	el->father->type = NORMAL;
	el->father->count++;
	return el;
}

Word* createNewWord(){
	Word *w = malloc(sizeof(Word));
	w->begin = NULL;
	w->end = NULL;
	w->next = NULL;
	w->previous = NULL;
	w->type = EMPTY;
	w->count = 0;
	return w;
}

Word* appNewWord(Word *word){
	Word *w = createNewWord();
	w->next = word->next;
	w->previous = word;
	if (word->next != NULL)
		word->next->previous = w;
	word->next = w;

	return w;
}

BOOL isEmptyWord(Word *word){
	if (word == NULL) return TRUE;
	if (word->count <= 0 || word->begin == word->end) {
		word->type = EMPTY;
		return TRUE;
	}
	return FALSE;
}

BOOL replaceWord(Word *word,Word *headWord,Word * endWord){
	if (word == NULL || headWord == NULL || endWord == NULL) return FALSE;
	if (word->previous != NULL){
		word->previous->next = headWord;
	}
	headWord->previous = word->previous;

	if (word->next != NULL)
		word->next->previous = endWord;
	endWord->next = word->next;

	if (textInput.headWord == word){
		textInput.headWord = headWord;
	}

	free(word);
	return TRUE;
}

Word* combineWords(Word *w1,Word* w2){
	if (w1 == NULL) return w2;
	if (w2 == NULL) return w1;

	Element *el = w2->begin;
	while (el != w2->end){
		el->father = w1;
		el = el->next;
	}
	w1->end = w2->end;
	w1->count += w2->count;

	if (w2->next != NULL)
		w2->next->previous = w1;
	w1->next = w2->next;

	if (w1->begin == w1->end)
		w1->type = EMPTY;
	else
		w1->type = NORMAL;
	free(w2);
	return w1;
}

BOOL delWord(Word *word){
	if (word == NULL) return FALSE;
	if (word->next != NULL)
		word->next->previous = word->previous;
	if (word->previous != NULL)
		word->previous->next = word->next;

	if (textInput.headWord == word){
		textInput.headWord = word->next;
	}
	free(word);
	return TRUE;
}

int delElement(Element *element){
	if (element == NULL) return;
	Word *word = element->father;
	if (word == NULL){
		printw("There is no father of Element!\n");
		return;
	}
	if (element == word->begin){
		word->begin = element->next;
	}
	if (element->previous != NULL)
		element->previous->next = element->next;
	if (element->next != NULL)
		element->next->previous = element->previous;

	free(element);
	word->count--;
	if (isEmptyWord(word)){
		if (word != textInput.headWord)
			delWord(word);
		return 1;
	}
	return 0;
}

BOOL clearWords(){
	Element *el = textInput.headWord->begin;
	while(el != NULL){
		Element *te = el->next;
		if (te == NULL || te->father != el->father){
			delWord(el->father);
		}
		el = te;
	}
	textInput.headWord = createNewWord();
	return TRUE;
}

BOOL clearElements(){
	Element *el = textInput.headWord->begin;
	while(el != NULL){
		Element *e = el;
		el = el->next;
		delElement(e);
	}
	textInput.headWord = createNewWord();
	textInput.curElement = NULL;
	return TRUE;
}

BOOL clearText(){
	if (!clearElements()) return FALSE;
	textInput.headWord = createNewWord();
	textInput.curElement = NULL;
	return TRUE;
}

BOOL isFunction(Word *word){
	return FALSE;
}
BOOL isOperator(Word *word){
	Element *el = word->begin;
	int i;
	for(i = 0; i < OPERATORNUM; ++i){
		if (el->c == operators[i])
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
	if (el->c == ' ' || el->c == '\t')
		return TRUE;
	return FALSE;
}
BOOL isNewline(Word *word){
	Element *el = word->begin;
	if (el->c == '\n')
		return TRUE;
	return FALSE;
}


void checkWord(Word * word){
	if (word == NULL){
		printw("There is no word!\n");
		return;
	}
	if (word->begin == word->end)
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
	}else if (isNewline(word)){
		word->type = NEWLINE;
	}else{
		word->type = NORMAL;
	}
}

int checkSeparate(Word *word,char input){
	switch(word->type){
		case EMPTY:
			break;
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
			if (input != ' ' & input != '\t'){
				return 1;
			}
			break;
		case NEWLINE:
			if (input != '\n')
				return 1;
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
	if (word == NULL || el == NULL) return;
	if (word->begin == word->end){
		word->begin = el;
	}
	word->end = el->next;
	word->count++;
	el->father = word;
	word->type = NORMAL;
}

BOOL isCombine(Word *w1,Word*w2){
	if (w1 == NULL || w2 == NULL) return FALSE;
	BOOL result = FALSE;
	if (w1->type == EMPTY || w2->type == EMPTY) result = TRUE;
	switch (w1->type){
		case FUNCTION:
		case KEYWORD:
		case OPERATOR:
		case NORMAL:
			if (w2->type != SEPARATE)
				result = TRUE;
			break;
		case SEPARATE:
			if (w2->type != NORMAL && w2->type != OPERATOR && 
				w2->type != KEYWORD && w2->type != FUNCTION)
				result = TRUE;
		case STRING:
		case STRING_DOT:
		case EXPLAIN:
			result = TRUE;
			break;
	}
	if (result == TRUE){
		w1 = combineWords(w1,w2);
	}
	return result;
}

Word *rebuildWord(Word *word){
	if (word == NULL) return NULL;

	Element *el = word->begin;

	if (el == word->end) 
		return word;
	Word *headWord = createNewWord();
	Word *curWord = headWord;

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
	if (headWord != curWord && isEmptyWord(curWord)){
		Word *tmp = curWord;
		curWord = tmp->previous;
		delWord(tmp);
	}
	replaceWord(word,headWord,curWord);
	return curWord;
}

void resetWords(){
	Element *el = textInput.headWord->begin;
	clearWords();
	textInput.headWord = createNewWord();
	while(el != NULL){
		linkElementInWord(el,textInput.headWord);
		el = el->next;
	}
	rebuildWord(textInput.headWord);
}


void rebuildWords(Word *word){
	resetWords();
return;
	do{
		word = rebuildWord(word);
	}while (word != NULL && isCombine(word,word->next));
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
		case InSelect:
			doDelete();
		case InDefault:
			addCharInBuffer(input);
			break;
	}
}
