
#include <pwd.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "../debug/dbg.h"

/* *
 * function to print the program usage
 * */
void usage()
{
    printf("Usage: logfind [-o] searchterm [searchterm, ...]\n"
           "Search through the log files\n\n"
           "-o  enable OR logic for searchterms\n");
}

/* *
 * function to check if a string/line is full of whitespace
 * */
int is_empty(const char *string) {
  while (*string != '\0') {
    if (!isspace((unsigned char)*string))
      return 0;
    string++;
  }
  return 1;
}

int main (int argc, char *argv[])
{

    // string to store home directory name
    const char *home_dir;
    // string to store the absolute path of ~/.logfind
    char logfind[100];

    // file descriptor to ~/.logfind
    FILE *logfind_fd;

    // string to store a line from ~/.logfind
    char *logfile = NULL;
    size_t length = 0;
    ssize_t read;

    // file descriptor to a log file mentioned in ~/.logfind
    FILE *nlogfile_fd;

    // string to store a line from a log file
    char *nlogline = NULL;
    size_t nlength = 0;
    ssize_t nread;

    // an integer to keep track of no of search terms found in a line
    int wc;
    // an integer to keep track of the line number of a log file
    int nline;
    // an integer to keep track of the number of search terms
    int nw = 0;

    // optind from getopt() to access argv[optind]
    extern int optind;

    // just an integer for iterative use
    int c;
    // an integer to determine whether OR logic is enabled or not
    int oflag = 0;

    // exit the program if no search term is provided
    if (argc == 1)  goto error;

    // determine whether to enable OR logic or not
    while ((c = getopt(argc, argv, "o")) != -1) {
        switch(c) {
            case 'o':
                oflag = 1;
                debug("Using the OR logic");
                break;
            default:
                goto error;
        }
    }

    // find the number of search terms
    for (c = optind; c < argc; c++ ) {
        debug("Search term: %s", argv[nw++]);
    }

    // get home directory
    home_dir = getpwuid(getuid())->pw_dir;
    debug("Home directory : %s", home_dir);

    // set the file descriptor for ~/.logfind to logfind_fd if it exists
    sprintf(logfind, "%s/.logfind", home_dir);
    logfind_fd = fopen(logfind, "r");
    check(logfind_fd != NULL, "Failed to read the \"%s\". Is it there?", logfind);
    debug("Reading the %s", logfind);


    // Thanks to getline() for allocating memory dynamically to store the line

    // Go through each line of ~/.logfind
    while((read = getline(&logfile, &length, logfind_fd)) != -1) {

        // go to next line if the line is empty
        if ((read < 2) || (is_empty(logfile)))
            continue;

        // remove the newline '\n'  from the name of log file
        logfile[read -1] = '\0';

        // determine if the log file exists or not
        nlogfile_fd = fopen(logfile, "r");
        if (nlogfile_fd == NULL) {
            debug("Failed to read the \"%s\".", logfile);
            continue;
        }

        debug("Going through the file %s", logfile);

        // line number
        nline = 0;
        // Go through the log file
        while ((nread = getline(&nlogline, &nlength, nlogfile_fd)) != -1) {
            nline++;
            if ((nread < 2) || (is_empty(nlogline)))
                continue;

            // 
            wc = 0;
            c = optind;
            for (; c < argc; c++) {
                if (strstr(nlogline, argv[c])) wc++;
            }

            if ((wc >= 1) && (oflag || wc == nw) ) {
                printf("%s:%d : %s", logfile, nline, nlogline);
            }

            // free it to store the next line  of log file in the iteration 
            free(nlogline);
            nlogline = NULL;
            nlength = 0;
        }

        // free nlogline after EOF
        free(nlogline);
        nlogline = NULL;
        nlength = 0;

        // close the log file
        fclose(nlogfile_fd);

        // free it to store the next line  of ~/.logfind in the iteration 
        free(logfile);
        logfile = NULL;
        length = 0;
    }

    // close the ~/.logfind
    fclose(logfind_fd);
    free(logfile);
    // free logfile after EOF
    return 0;
error:
    usage();
    return 1;
}
