#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>

#define LISTEN_BACKLOG 128
#define READ_SIZE 4096

//int funciton and variables, char ports, user, remote host n port
//int i =1, loop in a while block with i < argc n argv[i][0] = '-'
//if argv[i] = -u then set user to argv[i+1], i++++
//else tho = -1? then timeout = atoi(argv[i+1]), i++++
//else then goto usage error

//port = argv[i] then increment
//if argv[i] is there then hostname port
//if argv[i+1] is there then goto usage error`

typedef struct {
    char *data;
    int len;
    int cap;
} Buffer;

static struct pollfd *pfds = NULL;
static Buffer *buf = NULL;
static int fdsn = 0;
static int fdscap = 0;

static void append(Buffer *b, const char *data, int n) {
    if (b->len + n > b->cap) {
        b->cap = (b->len + n) * 2 + 64;
        b->data = realloc(b->data, b->cap);
    }
    memcpy(b->data + b->len, data, n);
    b->len += n;
}

static void memrmv(Buffer *b, int n) {
    memmove(b->data, b->data + n, b->len - n);
    b->len -= n;
}

static void cap(int n) {
    if (n > fdscap) {
        fdscap = n * 2 + 4;
        pfds = realloc(pfds, fdscap * sizeof(struct pollfd));
        buf = realloc(buf, fdscap * sizeof(Buffer));
    }
}

static void fd_add(int fd) {
    cap(fdsn + 1);
    pfds[fdsn].fd = fd;
    pfds[fdsn].events = POLLIN;
    pfds[fdsn].revents = 0;
    memset(&buf[fdsn], 0, sizeof(Buffer));
    fdsn++;
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

static void fd_remove(int i) {
    free(buf[i].data);
    close(pfds[i].fd);
    memmove(&pfds[i], &pfds[i + 1], (fdsn - i - 1) * sizeof(struct pollfd));
    memmove(&buf[i], &buf[i + 1], (fdsn - i - 1) * sizeof(Buffer));
    fdsn--;
}

static void fwd(int idx, const char *data, int len) {
    for (int i = 2; i < fdsn; i++) {
        if (i == idx) continue;
        int w = 0;
        while (w < len) {
            int n = write(pfds[i].fd, data + w, len - w);
            if (n <= 0) break;
            w += n;
        }
    }
}

int main(int argc, char *argv[]) {
    const char *username = "nobody";
    int timeout = -1;
    int i = 1;

    while (i < argc && argv[i][0] == '-') {
        if (strcmp(argv[i], "-u") == 0) {
            if (i + 1 >= argc) goto usage;
            username = argv[++i];
            i++;
        } else if (strcmp(argv[i], "-t") == 0) {
            if (i + 1 >= argc) goto usage;
            timeout = atoi(argv[++i]);
            i++;
        } else {
            goto usage;
        }
    }

    if (i >= argc) goto usage;
    char *port_str = argv[i++];

    char *remote = NULL;
    if (i < argc) {
        remote = argv[i++];
    }

    if (i < argc) goto usage;

    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, port_str, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "%s: %s\n", port_str, gai_strerror(status));
        return EXIT_FAILURE;
    }

    int listen_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(listen_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror(port_str);
        freeaddrinfo(res);
        close(listen_fd);
        return EXIT_FAILURE;
    }
    freeaddrinfo(res);
    listen(listen_fd, LISTEN_BACKLOG);

    int remote_fd = -1;
    if (remote) {
        char *c = strrchr(remote, ':');
        if (!c) goto usage;
        *c = '\0';
        char *rhost = remote;
        char *rport = c + 1;

        struct addrinfo rhints, *rres;
        memset(&rhints, 0, sizeof(rhints));
        rhints.ai_family = AF_INET;
        rhints.ai_socktype = SOCK_STREAM;

        status = getaddrinfo(rhost, rport, &rhints, &rres);
        if (status != 0) {
            fprintf(stderr, "%s: %s\n", rhost, gai_strerror(status));
            close(listen_fd);
            return EXIT_FAILURE;
        }

        remote_fd = socket(rres->ai_family, rres->ai_socktype, rres->ai_protocol);
        if (connect(remote_fd, rres->ai_addr, rres->ai_addrlen) < 0) {
            perror(rhost);
            freeaddrinfo(rres);
            close(remote_fd);
            close(listen_fd);
            return EXIT_FAILURE;
        }
        freeaddrinfo(rres);
    }

    fd_add(STDIN_FILENO);
    fd_add(listen_fd);
    if (remote_fd >= 0) fd_add(remote_fd);

    char tmp[READ_SIZE];

    while (1) {
        int ret = poll(pfds, fdsn, timeout);
        if (ret < 0) break;
        if (ret == 0) break;

        if (pfds[1].revents & POLLIN) {
            int new_fd = accept(pfds[1].fd, NULL, NULL);
            if (new_fd >= 0) fd_add(new_fd);
        }

        for (int i = 2; i < fdsn; i++) {
            if (!(pfds[i].revents & (POLLIN | POLLHUP))) continue;
            int n = read(pfds[i].fd, tmp, sizeof(tmp));
            if (n > 0) {
                append(&buf[i], tmp, n);
                while (1) {
                    char *nl = memchr(buf[i].data, '\n', buf[i].len);
                    if (!nl) break;
                    int line_len = (int)(nl - buf[i].data) + 1;
                    fwrite(buf[i].data, 1, line_len, stdout);
                    fflush(stdout);
                    fwd(i, buf[i].data, line_len);
                    memrmv(&buf[i], line_len);
                }
            } else {
                fd_remove(i);
                i--;
            }
        }

        if (pfds[0].fd >= 0 && (pfds[0].revents & (POLLIN | POLLHUP | POLLNVAL))) {
            int n = read(STDIN_FILENO, tmp, sizeof(tmp));
            if (n > 0) {
                append(&buf[0], tmp, n);
                while (1) {
                    char *nl = memchr(buf[0].data, '\n', buf[0].len);
                    if (!nl) break;
                    int line_len = (int)(nl - buf[0].data) + 1;
                    int ulen = (int)strlen(username);
                    int total = ulen + 2 + line_len;
                    char *msg = malloc(total);
                    memcpy(msg, username, ulen);
                    msg[ulen] = ':';
                    msg[ulen + 1] = ' ';
                    memcpy(msg + ulen + 2, buf[0].data, line_len);
                    fwd(-1, msg, total);
                    free(msg);
                    memrmv(&buf[0], line_len);
                }
            } else {
                pfds[0].fd = -1;
                pfds[0].events = 0;
            }
        }

    }

    for (int i = 0; i < fdsn; i++) {
        free(buf[i].data);
        if (pfds[i].fd >= 0 && pfds[i].fd != STDIN_FILENO)
            close(pfds[i].fd);
    }
    free(pfds);
    free(buf);
    return EXIT_SUCCESS;

usage:
    fprintf(stderr, "usage: %s [-u USERNAME] [-t TIMEOUT] PORT [HOSTNAME:PORT]\n", argv[0]);
    return EXIT_FAILURE;
}
