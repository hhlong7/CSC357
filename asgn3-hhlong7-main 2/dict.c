/* Defines functions for implmenting a dictionary.
 * CSC 357, Assignment 3
 * Given code, Winter '24 */

#include <stdlib.h>
#include "dict.h"
#include <stdio.h>
#include <string.h>

/* dcthash: Hashes a string key.
 * NOTE: This is certainly not the best known string hashing function, but it
 *       is reasonably performant and easy to predict when testing. */
static unsigned long int dcthash(char *key) {
    unsigned long int code = 0, i;

    for (i = 0; i < 8 && key[i] != '\0'; i++) {
        code = key[i] + 31 * code;
    }

    return code;
}

/* dctcreate: Creates a new empty dictionary.
 * TODO: Implement this function. It should return a pointer to a newly
 *       dynamically allocated dictionary with an empty backing array. */
Dict *dctcreate() {
    /* malloc it and init it
    // cap = 5
    // calloc the arr so all the slots are null 
    // size = 0
    // return the pointer */
        Dict *dctpointer = (Dict *)malloc(sizeof(Dict));
        dctpointer->cap = 5;
        dctpointer->arr = (Node **)calloc(dctpointer->cap, sizeof(Node *));
        dctpointer->size = 0;

    return dctpointer;
}

/* dctdestroy: Destroys an existing dictionary.
 * TODO: Implement this function. It should deallocate a dictionary, its
 *       backing array, and all of its nodes. */
void dctdestroy(Dict *dct) {
    /* loop from 0 to cap
     set current to the head of the linked list at that slot
     go to next val if the currnet val is not null
     free the current node key and set current to the next node
     free arr and dict */
    int i; 
    Node *current, *next;
    for (i=0; i < dct->cap; i++) {
        current = dct->arr[i];
        while (current != NULL) {
            next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
    }
    free(dct->arr);
    free(dct);
}

/* dctget: Gets the value to which a key is mapped.
 * TODO: Implement this function. It should return the value to which "key" is
 *       mapped, or NULL if it does not exist. */
void *dctget(Dict *dct, char *key) {
    /* get the index of the slot
    set current to the head of the linked list at that slot
    if the value is not null then return it
    return null if key not found */
    int i;
    Node *current;
    i = (int)(dcthash(key) % (unsigned long int)dct->cap);
    current = dct->arr[i];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->val;
        }
        current = current->next;
    }
    return NULL;
}

/* dctinsert: Inserts a key, overwriting any existing value.
 * TODO: Implement this function. It should insert "key" mapped to "val".
 * NOTE: Depending on how you use this dictionary, it may be useful to insert a
 *       dynamically allocated copy of "key", rather than "key" itself. Either
 *       implementation is acceptable, as long as there are no memory leaks. */
void dctinsert(Dict *dct, char *key, void *val) {
    /* init i and current like other func
    check if the key exist yet, if ya then return right away
    if not rehash it
    create a new node and add to the slot */

    int i, newcap, newi, j;
    Node **newarr, *next, *current;
    i = (int)(dcthash(key) % (unsigned long int)dct->cap);
    current = dct->arr[i];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->val = val;
            return;
        }
        current = current->next;
    }
    
    if (dct->size >= dct->cap) {
        newcap = dct->cap * 2 + 1;
        newarr = (Node **)calloc(newcap, sizeof(Node *));
        for (j = 0; j < dct->cap; j++) {
            current = dct->arr[j];
            while (current != NULL) {
                    next = current->next;
                    newi = (int)(dcthash(current->key) % (unsigned long int)newcap);
                    current->next = newarr[newi];
                    newarr[newi] = current;
                    current = next;
                }
        }
        free(dct->arr);
        dct->arr = newarr;
        dct->cap = newcap;
    }
    i = (int)(dcthash(key) % (unsigned long int)dct->cap);
    current = (Node *)malloc(sizeof(Node));
    current->key = (char *)malloc(strlen(key) + 1);
    strcpy(current->key, key);
    current->val = val;
    current->next = dct->arr[i];
    dct->arr[i] = current;
    dct->size++;
}

/* dctremove: Removes a key and the value to which it is mapped.
 * TODO: Implement this function. It should remove "key" and return the value
 *       to which it was mapped, or NULL if it did not exist. */
void *dctremove(Dict *dct, char *key) {
    /* init the values needed for the func
    set i to dcthash % cap
    set current to the head of the linked list at that slot
    set prev to NULL
    if current is not null then loop
    if key matches current key then remove node, free, save, and return val
    if not then move prev and current forward, decrement size too
    return null if key not found */
    
    int i;
    Node *current, *previous;
    void *value;
    i = (int)(dcthash(key) % (unsigned long int)dct->cap);
    current = dct->arr[i];
    previous = NULL;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            value = current->val;
            if (previous == NULL) {
                dct->arr[i] = current->next;
            } else {
                previous->next = current->next;
            }
            free(current->key);
            free(current);
            dct->size--;
            return value;
        } 
        previous = current;
        current = current->next;
    }
    return NULL;
}

/* dctkeys: Enumerates all of the keys in a dictionary.
 * TODO: Implement this function. It should return a dynamically allocated array
 *       of pointers to the keys, in no particular order, or NULL if empty. */
char **dctkeys(Dict *dct) {
    /* init the values at the top bc of ansi
    check if size = 0 then reutrn null
    set key = malloc
    loop 
    return key */

    int i, j = 0;
    Node *current;
    char **keys;
    if (dct->size == 0) {
        return NULL;
    }
    keys = (char **)malloc(dct->size * sizeof(char *));
    for (i = 0; i < dct->cap; i++) {
        current = dct->arr[i];
        while (current != NULL) {
            keys[j] = current->key;
            j++;
            current = current->next;
        }
    }
    return keys;
}
