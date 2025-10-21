#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <ulimit.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

extern char **environ;

int main(int argc, char *argv[]) {
    int opt;

    while ((opt = getopt(argc, argv, "ispudvcU:C:V:")) != -1) {
        switch (opt) {
            case 'i':
                printf("Real UID: %d, Effective UID: %d\n", (int)getuid(), (int)geteuid());
                printf("Real GID: %d, Effective GID: %d\n", (int)getgid(), (int)getegid());
                break;
            case 's':
                if (setpgid(0, 0) < 0) {
                    perror("setpgid");
                }
                break;
            case 'p':
                printf("PID: %d, PPID: %d, PGID: %d\n", (int)getpid(), (int)getppid(), (int)getpgrp());
                break;
            case 'u': {
                long lim = ulimit(UL_GETFSIZE);
                if (lim < 0) {
                    perror("ulimit get");
                } else {
                    printf("ulimit: %ld\n", lim);
                }
                break;
            }
            case 'U': {
                char *endptr;
                errno = 0;
                long newlim = strtol(optarg, &endptr, 10);
                if (errno != 0 || *endptr != '\0' || newlim < 0) {
                    fprintf(stderr, "Invalid ulimit value: %s\n", optarg);
                } else {
                    if (ulimit(UL_SETFSIZE, newlim) < 0) {
                        perror("ulimit set");
                    }
                }
                break;
            }
            case 'c': {
                struct rlimit rl;
                if (getrlimit(RLIMIT_CORE, &rl) < 0) {
                    perror("getrlimit");
                } else {
                    printf("Core limit: %llu\n", (unsigned long long)rl.rlim_cur);
                }
                break;
            }
            case 'C': {
                char *endptr;
                errno = 0;
                long newcore = strtol(optarg, &endptr, 10);
                if (errno != 0 || *endptr != '\0' || newcore < 0) {
                    fprintf(stderr, "Invalid core size: %s\n", optarg);
                } else {
                    struct rlimit rl;
                    rl.rlim_cur = (rlim_t)newcore;
                    rl.rlim_max = (rlim_t)newcore;  // Set both soft and hard limits
                    if (setrlimit(RLIMIT_CORE, &rl) < 0) {
                        perror("setrlimit");
                    }
                }
                break;
            }
            case 'd': {
                char buf[PATH_MAX];
                if (getcwd(buf, sizeof(buf)) != NULL) {
                    printf("CWD: %s\n", buf);
                } else {
                    perror("getcwd");
                }
                break;
            }
            case 'v': {
                for (char **env = environ; *env != NULL; env++) {
                    printf("%s\n", *env);
                }
                break;
            }
            case 'V':
                if (putenv(optarg) != 0) {
                    perror("putenv");
                }
                break;
            case '?':
                // getopt automatically prints error message for invalid option
                break;
            default:
                break;
        }
    }

    // Ignore any non-option arguments
    return 0;
}