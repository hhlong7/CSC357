#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    int dmax;
    int all;
    int flag;
} args;

//tree traversal
//raed file info itno struct
//if fails then would print path strerror and stuff, set error to 1 and return
//print indent for n times 4, where n = depth
//if flag, make a helper function that would help with the SISDIR, SIWUSR, SIRUSR funcs
//print file name
//also check with S_ISDIR, if dir then print / => call l_dir with path, d + 1, all, dmax, a, error
//sort, recurse, and cleanup (free)

int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

void helper(mode_t mode, char *file) { //0->3 is user
    file[0] = S_ISDIR(mode) ? 'd' : S_ISLNK(mode) ? 'l' : '-'; //file type
    file[1] = mode & S_IRUSR ? 'r' : '-'; //read
    file[2] = mode & S_IWUSR ? 'w' : '-'; //write
    file[3] = mode & S_IXUSR ? 'x' : '-'; //execute
    file[4] = mode & S_IRGRP ? 'r' : '-'; //4->6 group, read
    file[5] = mode & S_IWGRP ? 'w' : '-'; //write
    file[6] = mode & S_IXGRP ? 'x' : '-'; //execute
    file[7] = mode & S_IROTH ? 'r' : '-'; //7->9 other, read
    file[8] = mode & S_IWOTH ? 'w' : '-'; //write
    file[9] = mode & S_IXOTH ? 'x' : '-'; //execute
    file[10] = '\0';
}

void l_directory(const char *filepath, const char *filename, int d, args *a, int *error) {
    struct stat sts;
    int i, count = 0, cap;
    char file[11];
    char *buffer, **parent, *child;
    DIR *dir;
    struct dirent *dir_ptr;

    if (lstat(filepath, &sts) == -1) {
        for (i = 0; i < d * 4; i++) {
            printf(" ");
        }
        printf("%s: %s\n", filename, strerror(errno));
        *error = 1;
        return;
    }

    if (a->flag) {
        helper(sts.st_mode, file);
        printf("%s ", file);
    }
    for (i = 0; i < d * 4; i++) {
        printf(" ");
    }
    printf("%s", filename);

    if (a->flag && S_ISLNK(sts.st_mode)) {
        buffer = malloc(sts.st_size + 1);
        readlink(filepath, buffer, sts.st_size);
        buffer[sts.st_size] = '\0';
        printf(" -> %s", buffer);
        free(buffer);
    }
    printf("\n");

    if (S_ISDIR(sts.st_mode)) {
        if (a->dmax != -1 && d >= a->dmax) {
            return;
        }
        dir = opendir(filepath);
        if (dir == NULL) {
            for (i = 0; i < (d + 1) * 4; i++) {
                printf(" ");
            }
            printf("%s: %s\n", filename, strerror(errno));
            *error = 1;
            return;
        }
        count = 0;
        cap = 16;
        parent = malloc(cap * sizeof(char *));

        while ((dir_ptr = readdir(dir)) != NULL) {
            if (strcmp(dir_ptr->d_name, ".") == 0 || strcmp(dir_ptr->d_name, "..") == 0) {
                continue;
            }
            if (!a->all && dir_ptr->d_name[0] == '.') {
                continue;
            }
            if (count == cap) {
                cap = cap * 2;
                parent = realloc(parent, cap * sizeof(char *));
            }
            parent[count++] = strdup(dir_ptr->d_name);
        }
        closedir(dir);
        qsort(parent, count, sizeof(char *), compare);

        for (i = 0; i < count; i++) {
            child = malloc(strlen(filepath) + strlen(parent[i]) + 2);
            sprintf(child, "%s/%s", filepath, parent[i]);
            l_directory(child, parent[i], d + 1, a, error);
            free(child);
            free(parent[i]);
        }
        free(parent);
    }

}

//-d: how deep the tree can grow, limit to N
//-a: all files (even hidden) visible
//-l: long formant
//-b: invalid flag
//init all the varialbes
//check if the input is valid, input++
//else (argv(i) ?= -d), then argc or argv(i+1) != valid int
//print error and exit
//i + 2
//else (argv(i) ?= -1), flag = 1, i++
//else print error to stderr and exit
//if i < argc: path = argv(i) => i++
//leftover path so then print usage to stderr and exit
//format: usage message: "usage: ./tree [-a] [-d N] [-l] [PATH]\n" print this to stderr

int val_int(char *str) {
    int i = 0;
    if (str[0] == '\0') return 0;
    while (str[i] != '\0') {
        if (!isdigit((unsigned char)str[i])) return 0;
        i++;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    args a;
    int i = 1;
    int error = 0;
    char *path = ".";

    a.dmax = -1;
    a.all = 0;
    a.flag = 0;

    while (i < argc && argv[i][0] == '-') {
        if (strcmp(argv[i], "-a") == 0) {
            a.all = 1;
            i++;
        } else if (strcmp(argv[i], "-d") == 0) {
            if (i + 1 >= argc || !val_int(argv[i + 1])) {
                fprintf(stderr, "usage: %s [-a] [-d N] [-l] [PATH]\n", argv[0]);
                return EXIT_FAILURE;
            }
            a.dmax = atoi(argv[i+1]);
            i += 2;
        } else if (strcmp(argv[i], "-l") == 0) {
            a.flag = 1;
            i++;
        } else {
            fprintf(stderr, "usage: %s [-a] [-d N] [-l] [PATH]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }
    if (i < argc) {
        path = argv[i];
        i++;
    }
    if (i < argc) {
        fprintf(stderr, "usage: %s [-a] [-d N] [-l] [PATH]\n", argv[0]);
        return EXIT_FAILURE;
    }
    l_directory(path, path, 0, &a, &error);
    return error ? EXIT_FAILURE : EXIT_SUCCESS;
}

//done, passed all testcases letsgo bruh