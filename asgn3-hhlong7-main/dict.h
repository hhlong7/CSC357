/* Declares functions for implmenting a dictionary.
 * CSC 357, Assignment 3
 * Given code, Winter '24
 * NOTE: Do not alter this file. */

/* Node: Represents one key-value pair. */
typedef struct Node {
    char *key; /* string key */
    void *val; /* pointer to value */
    struct Node *next; /* pointer to next node */
} Node;

/* Dict: Represents a collection of key-value pairs. */
typedef struct Dict {
    int cap; /* current cap of arr */
    Node **arr; /* array of node* each slot is head of the linked list */
    int size; /* number of key and value pairs current in the dict */
} Dict;

Dict *dctcreate(); /* create a new empty dict */
void dctdestroy(Dict *); /* destroy exisiting dict */
void *dctget(Dict *, char *); /* get the value of the key */
void dctinsert(Dict *, char *, void *); /* insert new key and val pair */
void *dctremove(Dict *, char *); /* remove key while returning its value*/
char **dctkeys(Dict *); /* get all keys in the dict */
