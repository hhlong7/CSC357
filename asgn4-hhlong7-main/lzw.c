#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"

void convert_code_to_binary(int code1, int code2, FILE *destination) {
    //declare bytes arr
    //take code1 and 2
    //convert to 3 bytes
    //fwrite them to dest file
    unsigned char bytes[3];
    bytes[0] = (code1 >> 4) & 0xFF;
    bytes[1] = ((code1 & 0x0F) << 4) | ((code2 >> 8) & 0x0F);
    bytes[2] = code2 & 0xFF;
    fwrite(bytes, 1, 3, destination);
}

void pad0(int code, FILE *dest) {
    //pad the code with 4 0's
    //+ 0000
    unsigned char bytes[2];
    bytes[0] = (code >> 4) & 0xFF;
    bytes[1] = (code & 0x0F) << 4;
    fwrite(bytes, 1, 2, dest);
}

int undo(FILE *source, int *code1, int *code2) {
    //declare bytes arr
    //fread 3 bytes from source file
    //convert to code1 and code2
    //literally undo the convert func
    int n = 0;
    unsigned char bytes[3];
    n = fread(bytes, 1, 3, source);
    if (n == 0) return 0;
    *code1 = (bytes[0] << 4) | (bytes[1] >> 4);
    *code2 = ((bytes[1] & 0x0F) << 8) | bytes[2];
    return n;
}

void encode(FILE *in, FILE *out) {
    // init all the var
    // loop i -> 128 values so 0-127
    // insert i into dict
    // malloc buffer
    // fread 1 byte from source
    // write code for the strings
    // call convert code to bin func
    // done then free mem

    Dict *dict;
    int i, next, prev, curr, n;
    unsigned char c;
    char key[2], *cvalue, *next_str;
    size_t len;

    dict = dctcreate();
    for (i = 0; i < 128; i++) {
        key[0] = (char)i;
        key[1] = '\0';
        dctinsert(dict, key, (void *)(long)i);
    }
    next = 0x080;
    cvalue = malloc(1);
    cvalue[0] = '\0';
    len = 0;

    prev = 0;
    curr = 0;
    while (fread(&c, 1, 1, in) == 1) {
        next_str = malloc(len + 2);
        memcpy(next_str, cvalue, len);
        next_str[len] = (char)c;
        next_str[len + 1] = '\0';
        if (dctget(dict, next_str) != NULL) {
            free(cvalue);
            cvalue = next_str;
            len++;
        } else {
            n = (int)(long)dctget(dict, cvalue);
            if (curr) {
                convert_code_to_binary(prev, n, out);
                curr = 0;
            } else {
                prev = n;
                curr = 1;
            }
            if (next <= 0xFFF) {
                dctinsert(dict, next_str, (void *)(long)next++);
            }
            free(next_str);
            free(cvalue);
            cvalue = malloc(2);
            cvalue[0] = (char)c;
            cvalue[1] = '\0';
            len = 1;
        }
    }
    n = (int)(long)dctget(dict, cvalue);
    if (curr) {
        convert_code_to_binary(prev, n, out);
    } else {
        pad0(n, out);
    }
    free(cvalue);
    dctdestroy(dict);
}

void decode(FILE *in, FILE *out) {
    //init the var, use arr not dict like encode
    //loop i -> 128 values so 0-127
    //malloc(2) while [0] = i, [1] = \0
    //rest is null
    //set prev to null
    //while loop undo func to get code1 and code2
    //set each code to next idx in the arr
    //write the string to dest file
    //loop thru, wathc out for null strings
    //free
    //done
    char *array[4096];
    char *pvalue, *cvalue;
    int i, next, code1, code2, n;
    size_t len;

    for (i = 0; i < 128; i++) {
        array[i] = malloc(2);
        array[i][0] = (char)i;
        array[i][1] = '\0';
    }
    for (i = 128; i < 4096; i++) {
        array[i] = NULL;
    }
    pvalue = NULL;
    next = 0x080;
    while ((n = undo(in, &code1, &code2)) > 0) {
        if (code1 == next) {
            len = strlen(pvalue);
            array[next] = malloc(len + 2);
            strcpy(array[next], pvalue);
            array[next][len] = pvalue[0];
            array[next][len + 1] = '\0';
            cvalue = array[next];
            next++;
        } else {
            cvalue = array[code1];
            if (pvalue != NULL) {
                len = strlen(pvalue);
                array[next] = malloc(len + 2);
                strcpy(array[next], pvalue);
                array[next][len] = cvalue[0];
                array[next][len + 1] = '\0';
                next++;
            }
        }
        fwrite(cvalue, 1, strlen(cvalue), out);
        pvalue = cvalue;
        if (n < 3) {
            break;
        }
        if (code2 == next) {
            len = strlen(pvalue);
            array[next] = malloc(len + 2);
            strcpy(array[next], pvalue);
            array[next][len] = pvalue[0];
            array[next][len + 1] = '\0';
            cvalue = array[next];
            next++;
        } else {
            cvalue = array[code2];
            if (pvalue != NULL) {
                len = strlen(pvalue);
                array[next] = malloc(len + 2);
                strcpy(array[next], pvalue);
                array[next][len] = cvalue[0];
                array[next][len + 1] = '\0';
                next++;
            }
        }
        fwrite(cvalue, 1, strlen(cvalue), out);
        pvalue = cvalue;
    }
    for (i = 0; i < next; i++) {
        free(array[i]);
    }
}

int main(int argc, char *argv[]) {
    //argv parsing
    //declare pointers
    //check for num of argv
    //check if -c or -x
    //open files
    //if null print error and exit
    //open output file for write
    //if null close, print error, exit
    //if -c call encode, otherwise decode
    //close both of them, exit, boom, done

    FILE *source, *destination;

    if (argc != 4) {
        fprintf(stderr, "usage: %s (-c | -x) SOURCE DESTINATION\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-c") != 0 && strcmp(argv[1], "-x") != 0) {
        fprintf(stderr, "usage: %s (-c | -x) SOURCE DESTINATION\n", argv[0]);
        return EXIT_FAILURE;
    }

    source = fopen(argv[2], "rb");
    if (source == NULL) {
        perror(argv[0]);
        return EXIT_FAILURE;
    }

    destination = fopen(argv[3], "wb");
    if (destination == NULL) {
        fclose(source);
        perror(argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-c") == 0) {
        encode(source, destination);
    } else {
        decode(source, destination);
    }

    fclose(source);
    fclose(destination);
    return EXIT_SUCCESS;
}
