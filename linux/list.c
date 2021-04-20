#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define SEC_PER_DAY 86400
#define MAX_BUF_SIZE 1024
#define NUM_OPT 6 //number of available options
#define H_VALUE 0x3f3f3f3f
#define L_VALUE -1

int opt_a = 0, opt_r = 0, opt_g = 0, opt_h = H_VALUE, opt_l = L_VALUE, opt_m = H_VALUE;
int *pOpt[NUM_OPT] = {&opt_a, &opt_r, &opt_m, &opt_l, &opt_h, &opt_g};

void help() {
    printf("LIST 1.0.1 by Ridd, %s %s \
        \nUsage: list [OPTION]... [FILE]..., \
        \nList information about the FILEs (the current directory by default),\n \
        \n  -?        \tDisplay help\
        \n  -a        \tShow files starting with\
        \n  -r        \tList subdirectories recursively\
        \n  -l <bytes>\tOnly show files larger than <bytes>\
        \n  -h <bytes>\tOnly show files smaller than <bytes>\
        \n  -m <days> \tOnly show files modified within <days>\n",
           __DATE__, __TIME__);
    exit(0);
}

void list(char *path, char *pre_path, char *filename) {
    DIR *dp;
    struct dirent *dirp;
    struct stat st;
    if (stat(path, &st) < 0) {
        printf("%s: No such file or directory.\n", path);
        return;
    }
    if (S_ISDIR(st.st_mode)) {
        if ((dp = opendir(path)) == NULL) {
            printf("%s: No such file or directory.\n", path);
            return;
        }
        while ((dirp = readdir(dp)) != NULL) {
            if (!opt_a && '.' == dirp->d_name[0])
                continue;
            char new_path[256] = {0}, buf[256] = {0};
            strcat(new_path, path), strcat(new_path, "/");
            strcat(buf, pre_path), strcat(buf, dirp->d_name);
            if (dirp->d_type == 4) {
                if (opt_r && strcmp("..", dirp->d_name) && strcmp(".", dirp->d_name)) {
                    list(strcat(new_path, dirp->d_name), strcat(buf, "/"), dirp->d_name);
                } else if (opt_a && (dirp->d_name[0] == '.')) {
                    printf("%16ld  %s\n", st.st_size, buf);
                }
            } else {
                list(strcat(new_path, dirp->d_name), pre_path, dirp->d_name);
            }
        }
    } else {
        char buf[256] = {0};
        time_t now = time(NULL);
        strcat(buf, pre_path);
        if (st.st_size > opt_l && st.st_size < opt_h && (now - st.st_mtime) / SEC_PER_DAY < opt_m)
            printf("%16ld  %s\n", st.st_size, strcat(buf, filename));
    }
}

void listfile_in_cwd(char *filename, int *num) {
    char *cwd = getcwd(NULL, MAX_BUF_SIZE);
    strcat(cwd, "/");
    if (filename[0] == '/')
        list(filename, "", filename);
    else
        list(strcat(cwd, filename), "", filename);
    free(cwd);
    *num += 1;
}

void parse(int argc, char **argv) {
    int state = 0, num = 0;
    for (int i = 1; i < argc; i++) {// argv[0] is path to executable
        switch (state) {
            case 0:
                switch (argv[i][1]) {
                    case 'r':
                        opt_r = 1;
                        break;
                    case 'a':
                        opt_a = 1;
                        break;
                    case 'm':
                        state = 2;
                        break;
                    case 'l':
                        state = 3;
                        break;
                    case 'h':
                        state = 4;
                        break;
                    case '-':
                        state = 5;
                        break;
                    case '?':
                        help();
                        break;
                    default:
                        listfile_in_cwd(argv[i], &num);
                }
                break;
            case 8:
                listfile_in_cwd(argv[i], &num);
                break;
            default:
                if (state > 1 && state < 5)
                    *pOpt[state] = atoi(argv[i]);
                else if (state <= 1)
                    *pOpt[state] = 1;
                else if (state == 5) {
                    state = 8;
                    break;
                }
                state = 0;
                break;
        }
    }
    if ((state != 0) && (state != 8))
        help();
    if (num == 0)
        listfile_in_cwd("", &num);
}

int main(int argc, char *argv[]) {
    parse(argc, argv);
    return 0;
}
