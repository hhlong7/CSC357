/* Converts characters from tabs to spaces.
 * CSC 357, Assignment 1
 * Given code, Spring '24 */

#include <stdio.h>
#include "chars.h"

int main(void) {
 char next, str[9];
 int i;
 int n; 
 while (scanf("%c", &next) != EOF) {
        /* TODO: Call "ttos" here, then print the result. */
	n = ttos(next, str, '\0');
	for (i = 0; i < n; i++)
	 printf("%c", str[i]);	
    }

    return 0;
}
