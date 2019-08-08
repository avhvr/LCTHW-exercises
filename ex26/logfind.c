
#include <pwd.h>
#include <glob.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "../debug/dbg.h"

char *LOGFIND = ".logfind";

/* determine whether OR logic is enabled or not */
int OFLAG = 0;

/* number of search terms */
int NSTF = 0;

/* print the program usage */
void usage()
{
    printf("Usage: logfind [-o] searchterm [searchterm, ...]\n"
           "Search through the log files in ~/%s\n\n"
           "-o  enable OR logic for searchterms\n", LOGFIND);
    exit(1);
}

/* check if a string is full of whitespace */
int is_empty(const char *string) {
  while (*string != '\0') {
    if (!isspace((unsigned char)*string))
      return 0;
    string++;
  }
  return 1;
}

/* search through the file and print the lines with search terms */
void search_the_logfile(char *logfile, FILE *logfile_fd, int argc, char *argv[])
{
    char *logline = NULL;
    size_t length = 0;
    ssize_t read;

    int nstf;
    int line = 0;

    extern int optind;

    while ((read = getline(&logline, &length, logfile_fd)) != -1) {
        line++;
        if (is_empty(logline))
            continue;

        nstf = 0;

        for (int c = optind; c < argc; c++) {
            if (strstr(logline, argv[c])) nstf++;
        }

        if ((nstf >= 1) && (OFLAG || nstf == NSTF))
                printf("%s:%d : %s", logfile, line, logline);
    }

    fclose(logfile_fd);
    free(logline);
}

int main (int argc, char *argv[])
{
    if (argc == 1)  usage();

    const char *home_dir;    // home directory

    char logfind[100];    // absolute path to ~/.logfind

    FILE *logfind_fd;    // ~/.logfind

    char *logfile = NULL;
    size_t length = 0;
    ssize_t read;

    FILE *logfile_fd;     // a log file mentioned in ~/.logfind

    int ret;
    extern int optind;

    glob_t files;

    while ((ret = getopt(argc, argv, "o")) != -1) {
        switch(ret) {
            case 'o':
                OFLAG = 1;
                debug("Using the OR logic");
                break;
            default:
                goto error;
        }
    }

    for (int i = optind; i < argc; i++ ) NSTF++;

    home_dir = getpwuid(getuid())->pw_dir;

    sprintf(logfind, "%s/%s", home_dir, LOGFIND);
    debug("~/.logfind : %s", logfind);
    logfind_fd = fopen(logfind, "r");
    check(logfind_fd != NULL, "%s", logfind);

    while((read = getline(&logfile, &length, logfind_fd)) != -1) {

        if (is_empty(logfile)) continue;

        logfile[read -1] = '\0';

        ret = glob(logfile, GLOB_NOCHECK, NULL, &files);
        check(ret == 0, "%s", logfile);

        for (int i = 0; i < files.gl_pathc; i++) {
            logfile_fd = fopen(files.gl_pathv[i], "r");
            if (logfile_fd == NULL) {
                log_err("%s", logfile);
                continue;
            }
            debug("Reading %s", files.gl_pathv[i]);
            search_the_logfile(files.gl_pathv[i], logfile_fd, argc, argv);
        }

        globfree(&files);
    }

    fclose(logfind_fd);
    free(logfile);
    return 0;
error:
    if (logfind_fd) fclose(logfind_fd);
    if (logfile) free(logfile);
    return 1;
}
