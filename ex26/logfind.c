
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
    if (argc == 1)  goto error;

    // home directory
    const char *home_dir;
    // absolute path to ~/.logfind
    char logfind[100];

    // ~/.logfind
    FILE *logfind_fd;

    char *logfile = NULL;
    size_t length = 0;
    ssize_t read;

    // a log file mentioned in ~/.logfind
    FILE *logfile_fd;

    int c;
    extern int optind;

    while ((c = getopt(argc, argv, "o")) != -1) {
        switch(c) {
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
    debug("Home directory : %s", home_dir);

    sprintf(logfind, "%s/%s", home_dir, LOGFIND);
    logfind_fd = fopen(logfind, "r");
    check(logfind_fd != NULL, "%s", logfind);

    while((read = getline(&logfile, &length, logfind_fd)) != -1) {

        if (is_empty(logfile)) continue;

        logfile[read -1] = '\0';

        logfile_fd = fopen(logfile, "r");
        if (logfile_fd == NULL) {
            log_err("%s", logfile);
            continue;
        }

        debug("Reading %s", logfile);
        search_the_logfile(logfile, logfile_fd, argc, argv);
    }

    fclose(logfind_fd);
    free(logfile);
    return 0;
error:
    usage();
    return 1;
}
