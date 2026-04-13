/* Defines functions for converting characters between tabs and spaces.
 * CSC 357, Assignment 1
 * Given code, Spring '24. */

#include "chars.h"

/* istext: Checks whether or not a character is plain text.
 * NOTE: Do not alter this function. It returns 1 if "c" has an ASCII code in
 *       the ranges [0x08, 0x0D] or [0x20, 0x7E], and 0 otherwise. */
int istext(char c) {
    return (c >= 0x08 && c <= 0x0D) || (c >= 0x20 && c <= 0x7E);
}

/* ttos: Converts a character from tabs to spaces.
 * TODO: Implement this function. It should return the number of characters
 *       placed into "str": generally 1 to 8 spaces if "next" is a tab, pending
 *       the current line length, and "next" otherwise. Use "dflt" instead if
 *       "next" is not plain text, and do nothing if neither is plain text.
 *       See also the given unit tests. */
int ttos(char next, char str[], char dflt) {
 static int line = 0;
 char ch;
 int i = 0;
 int space;
 if (istext(next))
	ch = next;
 else if (istext(dflt))
	ch = dflt;
 else {
	str[0] = '\0'; 
	return 0;
 }
 
 if (ch == '\t') {
	space = 8 - (line % 8);
	for (; i < space; i++)
		str[i] = ' ';
	str[space] = '\0';
	line += space;
	return space;
 }

 str[0] = ch;
 str[1] = '\0';
 if (ch >= '\n' && ch <= '\r')
	line = 0;
 else
	line++;
 return 1;
}


/* stot: Converts a character from spaces to tabs.
 * TODO: Implement this function. It should return the number of characters
 *       placed into "str": generally a tab or nothing if "next" is a space,
 *       pending the current line length, and 0 to 7 spaces followed by "next"
 *       otherwise. Use "dflt" instead if "next" is not plain text, and do
 *       nothing if neither is plain text. See also the given unit tests. */
int stot(char next, char str[], char dflt) {
 static int line = 0;
 static int space_count = 0;
 char ch;
 int i = 0;
 int count;
 
 if (istext(next))
	ch = next;
 else if (istext(dflt))
	ch = dflt;
 else {
	str[0] = '\0';
	return 0;
 }

 if (ch == ' ') {
	space_count++;
	if ((line + space_count) % 8 == 0) {
		str[0] = '\t';
		str[1] = '\0';
		line += space_count;
		space_count = 0;
		return 1;
	}
	str[0] = '\0';
	return 0;
 }

 for (; i < space_count; i++)
	str[i] = ' ';
 str[space_count] = ch;
 count = space_count + 1;
 str[count] = '\0';
 line += space_count;
 space_count = 0;

 if (ch >= '\n' && ch <= '\r')
	line = 0;
 else if (ch == '\t')
	line += 8 - (line % 8);
 else
	line++;
 return count;
}
