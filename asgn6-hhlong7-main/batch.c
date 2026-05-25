#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

typedef struct {
    char **argv;
    int argc;
} jobs;

typedef struct {
    int n;
    int e;
    int v;
} opts;

//launch the jobs ( n at times)
//wait for one to be done
//if -v: print what started or finished
//if -e then stop
//repeat till all done

//loop:
//while j left to run or still runnin
//while next < no of j and running < n and not fail
//if -v: print + cmd to stdeerr
//fork()
//child: execvp if fail then perror + exit(1)
//parent: save pid + j index, runnin ++, next ++
//wait() for child to finish
//if -v: print - cmd to stdeerr
//if fail and -e
//kill all running children, wait for them, break
//else, running --
//wai() for any killed children/jobs

void print_command(jobs *j) {
    int i;
    for (i = 0; i < j->argc; i++) {
        if (i > 0) fprintf(stderr, " ");
        fprintf(stderr, "%s", j->argv[i]);
    }
    fprintf(stderr, "\n");
}

int run(jobs *j, int n_jobs, opts *options) {
    pid_t *pids = malloc(n_jobs * sizeof(pid_t));
    int *j_idx = malloc(n_jobs * sizeof(int));
    int running = 0, next = 0, failed = 0, exit_status = EXIT_SUCCESS, k, status;
    pid_t pid, finished;

    while (next < n_jobs || running > 0) {
        while (next < n_jobs && running < options->n && !failed) {
            if (options->v) {
                fprintf(stderr, "+ ");
                print_command(&j[next]);
            }
            pid = fork();
            if (pid == 0) {
                execvp(j[next].argv[0], j[next].argv); //this is child
                perror(j[next].argv[0]);
                exit(EXIT_FAILURE);
            }
            pids[running] = pid;
            j_idx[running] = next;
            running++;
            next++; //parent
        }
        if (running == 0) break;
        finished = wait(&status);
        for (k = 0; k < running; k++) {
            if (pids[k] == finished) {
                int idx = j_idx[k];
                if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                    exit_status = EXIT_FAILURE;
                    if (options->e) {
                        failed = 1;
                        for (int m = 0; m < running; m++) {
                            if (m != k) {
                                kill(pids[m], SIGTERM);
                            }
                        }
                    }
                }
                if (options->v) {
                    fprintf(stderr, "- ");
                    print_command(&j[idx]);
                }

                for (int m = k; m < running - 1; m++) {
                    pids[m] = pids[m + 1];
                    j_idx[m] = j_idx[m + 1];
                }
                running--;
                break;
            }
        }
    }
    while (running > 0) {
        wait(NULL);
        running--;
    }
    free(pids);
    free(j_idx);
    return exit_status;
}

//declare functions and var
//struct opts
//struct jobs, int no of them to 0
//loop while when i < argc and argv[i] != '--' 
//to check if -n, -e, -v, -h
//if -n, then read from file and execute commands
//if -e, then execute command from command line
//if -v, then print version
//esle, print usage then exit

//if i >= argc, print usage, exit

//loop for js i < argc
//if argv[i] == '--', then increment i
//if i >= argc, print usage, exit
//start a new job, job.argv[0] = argv[i]
//increment i, while i < argc and argv[i] != '--', add to job.argv
//i++, null terminate job.argv
//if no of jobs == 0, print usage and exit
//opts.n == -1, opts.n = no jobs => default all jobs exec


int if_val(char *v) {
    int i = 0;
    if (v[0] == '\0') return 0;
    while (v[i] != '\0') {
        if (!isdigit((unsigned char)v[i])) return 0;
        i++;
    }
    return atoi(v) >= 1;
}

int main(int argc, char *argv[]) {
    opts options = {-1, 0, 0};
    jobs *j = NULL;
    int n_jobs = 0;
    int i = 1;
    int cap = 8;

    j = malloc(cap * sizeof(jobs));
    while (i < argc && strcmp(argv[i], "--") != 0) {
        if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 >= argc || !if_val(argv[i + 1])) {
                fprintf(stderr, "usage: %s [-n N] [-e] [-v] -- COMMAND [-- COMMAND ...]\n", argv[0]);
                free(j);
                return EXIT_FAILURE;
            }
            options.n = atoi(argv[i + 1]);
            i += 2;
        } else if (strcmp(argv[i], "-e") == 0) {
            options.e = 1;
            i++;
        } else if (strcmp(argv[i], "-v") == 0) {
            options.v = 1;
            i++;
        } else {
            fprintf(stderr, "usage: %s [-n N] [-e] [-v] -- COMMAND [-- COMMAND ...]\n", argv[0]);
            free(j);
            return EXIT_FAILURE;
        }
    }
    if (i >= argc) {
        fprintf(stderr, "usage: %s [-n N] [-e] [-v] -- COMMAND [-- COMMAND ...]\n", argv[0]);
        free(j);
        return EXIT_FAILURE;
    }

    while (i < argc) {
        int k_cap;
        if (strcmp(argv[i], "--") != 0) {
            fprintf(stderr, "usage: %s [-n N] [-e] [-v] -- COMMAND [-- COMMAND ...]\n", argv[0]);
            free(j);
            return EXIT_FAILURE;
        }
        i++;
        if (i >= argc || strcmp(argv[i], "--") == 0) {
            fprintf(stderr, "usage: %s [-n N] [-e] [-v] -- COMMAND [-- COMMAND ...]\n", argv[0]);
            free(j);
            return EXIT_FAILURE;
        }
        if (n_jobs == cap) {
            cap *= 2;
            j = realloc(j, cap * sizeof(jobs));
        }
        k_cap = 4;
        j[n_jobs].argv = malloc(k_cap * sizeof(char *));
        j[n_jobs].argc = 0;

        while (i < argc && strcmp(argv[i], "--") != 0) {
            if (j[n_jobs].argc == k_cap - 1) {
                k_cap *= 2;
                j[n_jobs].argv = realloc(j[n_jobs].argv, k_cap * sizeof(char *));
            }
            j[n_jobs].argv[j[n_jobs].argc++] = argv[i];
            i++;
        }
        j[n_jobs].argv[j[n_jobs].argc] = NULL;
        n_jobs++;
    }
    if (n_jobs == 0) {
        fprintf(stderr, "usage: %s [-n N] [-e] [-v] -- COMMAND [-- COMMAND ...]\n", argv[0]);
        free(j);
        return EXIT_FAILURE;
    }
    if (options.n == -1) options.n = n_jobs;

    int result = run(j, n_jobs, &options);
    
    for (i = 0; i < n_jobs; i++) {
        free(j[i].argv);
    }
    free(j);
    return result;
}





