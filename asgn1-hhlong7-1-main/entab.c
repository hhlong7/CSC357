/* Converts characters from spaces to tabs.
 * CSC 357, Assignment 1
 * Given code, Spring '24 */

#include <stdio.h>
#include "chars.h"

int main(void) {
 char next, str[9];
 int i;
 int n;
 while (scanf("%c", &next) != EOF) {
        /* TODO: Call "stot" here, then print the result. */
	n = stot(next, str, '\0');
	for (i = 0; i < n; i++)
	 printf("%c", str[i]);
    }

    return 0;
}
