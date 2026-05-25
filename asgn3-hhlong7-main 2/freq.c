#include <stdlib.h>
#include "dict.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* check if arg is too few or too many
open the file and read it charforchar, if null return output like out7.txt
create dict 
fget it
qsort it
print 10 most freq words with counts
free
*/

typedef struct freq {
    char *word;
    int freq;
} freq;

int cmp(const void *a, const void *b) {
    freq *wordA, *wordB;
    wordA = (freq *)a;
    wordB = (freq *)b;
    if (wordA->freq != wordB->freq) {
        return wordB->freq - wordA->freq;
    }
    return strcmp(wordA->word, wordB->word);
}

int main(int argc, char *argv[]) {
    int buffersize, i, j, no_keys, length;
    char c, *buffer, word[1024], **key, *ptr;
    FILE *file;
    Dict *dct;
    freq *frequency;
    if (argc < 2){
        fprintf(stderr, "%s: Too few arguments\n", argv[0]);
        return 1;
    }
    if (argc > 2) {
        fprintf(stderr, "%s: Too many arguments\n", argv[0]);
        return 1;
    }
    file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "%s: No such file or directory\n", argv[0]);
        return 1;
    }
    dct = dctcreate();
    buffersize = 64;
    buffer = (char *)malloc(buffersize * sizeof(char));
    while (fgets(buffer, buffersize, file) != NULL) {
        while (strlen(buffer) > 0 && buffer[strlen(buffer) - 1] != '\n' && !feof(file)) {
            buffersize = buffersize * 2;
            buffer = (char *)realloc(buffer, buffersize);
            fgets(buffer + strlen(buffer), buffersize / 2, file);
        }
        ptr = strtok(buffer, " \t\r\n\f\v");
        while (ptr != NULL) {
            length = 0;
            i = 0;
            while (ptr[i] != '\0') {
                c = ptr[i];
                if (isalpha(c)) {
                    word[length++] = (char)tolower(c);
                }
                i++;
            }
            word[length] = '\0';
            if (length == 0) {
                ptr = strtok(NULL, " \t\r\n\f\v");
                continue;
            }
            j = (int)(long)dctget(dct, word);
            dctinsert(dct, word, (void *)(long)(j + 1));
            ptr = strtok(NULL, " \t\r\n\f\v");
        }
    }
    key = dctkeys(dct);
    if (key == NULL) {
        free(buffer);
        fclose(file);
        dctdestroy(dct);
        return 0;
    }
    no_keys = dct->size;
    frequency = (freq *)malloc(no_keys * sizeof(freq));
    for (i = 0; i < no_keys; i++) {
        frequency[i].word = key[i];
        frequency[i].freq = (int)(long)dctget(dct, key[i]);
    }
    qsort(frequency, no_keys, sizeof(freq), cmp);
    for (i = 0; i < no_keys && i < 10; i++) {
        printf("%s (%d)\n", frequency[i].word, frequency[i].freq);
    }
    free(frequency);
    free(key);
    free(buffer);
    fclose(file);
    dctdestroy(dct);
    return 0;
}