/* Defines functions for sorting arrays in parallel.
 * CSC 357, Assignment 7
 * Given code, Winter '24 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "fsort.h"

/* fsort: Sorts an array using a parallelized merge sort.
 * TODO: Implement this function. It should sort the "n" elements of "base",
 *       each of "width" bytes, creating a child process to sort the latter
 *       half of the array if and only if "n > min", and returning 1 if the
 *       requisite child processes could not be created and 0 otherwise. */

//void the printer idx, 
//(char *)base + i * width, 
//and for pointer (char *)base + k * width

//helper function to merge two sorted half of arr, left and right

static void merger(void *base, size_t left, size_t right, size_t width, int (*cmp)(const void *, const void *)) {
    char *temporary = malloc((left + right) * width);
    char *l_ptr = (char *)base;
    char *r_ptr = (char *)base + left * width;
    size_t i = 0, j = 0, k = 0;

    while (i < left && j < right) {
        if (cmp(l_ptr + i * width, r_ptr + j *width) <= 0) {
            memcpy(temporary + k++ * width, l_ptr + i++ * width, width);
        } else {
            memcpy(temporary + k++ * width, r_ptr + j++ * width, width);
        }
    }
    while (i < left) {
        memcpy(temporary + k++ * width, l_ptr + i++ * width, width);
    }
    while (j < right) {
        memcpy(temporary + k++ * width, r_ptr + j++ * width, width);
    }
    memcpy(base, temporary, (left + right) * width);
    free(temporary);
}

//if n <= min, then sort whole arr by qsort return 0
//split arr, k = n / 2 for left side (this is for parent)
//n = n - k for right side, ts child
//make sure pointer point to wtv at idx k (right side)

//pipe and fork
//int the var for pipe fd[2]
//call pipe(fd), and fork(), if fork() == -1 close both pipes reutn 1

//for pid == 0, close fd[0] since its child?, call fsort for right 
//loop with pointer to right side, tot = n * bytes width, write to write pipe,
//close that pipe then exit

//for parent, close write pipe, call fsort for left then
//loop this time is read, pointer to right side, same logic as child
//but this time read func for pipe, close fd[0], waitpid
//merge

int fsort(
 void *base, size_t n, size_t width, size_t min,
 int (*cmp)(const void *, const void *)) {
    size_t k, right, total;
    ssize_t output;
    void *r_ptr;
    int fd[2], failed, status;
    char *buffer;
    pid_t pid;

    if (n <= min) {
        qsort(base, n, width, cmp);
        return 0;
    }

    k = n / 2;
    right = n - k;
    r_ptr = (char *)base + k * width;

    if (pipe(fd) == -1) {
        return 1;
    }

    pid = fork();
    if (pid == -1) {
        close(fd[0]);
        close(fd[1]);
        return 1;
    }

    if (pid == 0) {
        close(fd[0]);
        failed = fsort (r_ptr, right, width, min, cmp);
        buffer = (char *)r_ptr;
        total = right * width;
        while (total > 0) {
            output = write(fd[1], buffer, total);
            if (output <= 0) {
                break;
            }
            buffer = buffer + output;
            total = total - (size_t)output;
        }
        close(fd[1]);
        exit(failed);
    }
    close (fd[1]);
    failed = fsort(base, k, width, min, cmp);
    buffer = (char *)r_ptr;
    total = right * width;
    while (total > 0) {
        output = read(fd[0], buffer, total);
        if (output <= 0) {
            break;
        }
        buffer = buffer + output;
        total = total - (size_t)output;
    }
    close(fd[0]);

    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        failed = 1;
    }

    merger(base, k, right, width, cmp);
    return failed;
}
//fixed mem leaks (hopefully)
//bugs with wrong variable, didnt () for left + rihgt * witdh
//should be left + right then all together times width
    






