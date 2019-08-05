
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "../debug/dbg.h"

#define MAX_DATA 100

int read_string(char **out_string, int max_buffer)
{
    *out_string = calloc(1, max_buffer + 1);
    check_mem(*out_string);

    char *result = fgets(*out_string, max_buffer, stdin);
    check(result != NULL, "Input error.");

    return 0;

error:
    if (*out_string) free(*out_string);
    *out_string = NULL;
    return -1;
}

int read_int(int *out_int)
{
    char *input = NULL;
    int rc = read_string(&input, MAX_DATA);
    check(rc == 0, "Failed to read number.");

    *out_int = atoi(input);

    free(input);
    return 0;

error:
    if (input) free(input);
    return -1;
}

int read_scan(const char *fmt, ...)
{
    int i = 0;
    int rc = 0;
    int *out_int = NULL;
    char *out_char = NULL;
    char **out_string = NULL;
    int max_buffer = 0;

    va_list argp;
    va_start(argp, fmt);

    for (i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case '\0':
                    sentinel("Invalid format, you ended with %%.");
                    break;

                case 'd':
                    out_int = va_arg(argp, int *);
                    rc = read_int(out_int);
                    check(rc == 0, "Failed to read int.");
                    break;

                case 'c':
                    out_char = va_arg(argp, char *);
                    *out_char = fgetc(stdin);
                    break;

                case 's':
                    max_buffer = va_arg(argp, int);
                    out_string = va_arg(argp, char **);
                    rc = read_string(out_string, max_buffer);
                    check(rc == 0, "Failed to read string.");
                    break;

                default:
                    sentinel("Invalid format.");
            }
        } else {
            fgetc(stdin);
        }

        check(!feof(stdin) && !ferror(stdin), "Input error.");
    }

    va_end(argp);
    return 0;

error:
    va_end(argp);
    return -1;
}

int print(char *fmt, ...)
{
    int d;
    char c, *s;

    va_list argp;
    va_start(argp, fmt);

    for (int i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] == '%') {
            i++;
            switch(fmt[i]) {

                case 'x':
                    sprintf(s, "%x", va_arg(argp, int));
                    fputs(s, stdout);
                    break;

                case 'p':
                    sprintf(s, "%p", va_arg(argp, void *));
                    fputs(s, stdout);
                    break;

                case 's':
                    s = va_arg(argp, char *);
                    fputs(s, stdout);
                    break;

                case 'd':
                    d = va_arg(argp, int);
                    sprintf(s, "%d", d);
                    fputs(s, stdout);
                    break;

                case 'c':
                    c = va_arg(argp, int);
                    fputc(c, stdout);
                    break;

                default:
                    sentinel("Invalid format.");
            }
        } else {
            fputc(fmt[i], stdout);
        }
    }

    va_end(argp);
    return 0;
error:
    va_end(argp);
    return -1;
}

int main(int argc, char *argv[])
{
    char *first_name = NULL;
    char initial = ' ';
    char *last_name = NULL;
    int age = 0;

    printf("What's your first name? ");
    int rc = read_scan("%s", MAX_DATA, &first_name);
    check(rc == 0, "Failed first name.");

    printf("What's your initial? ");
    rc = read_scan("%c\n", &initial);
    check(rc == 0, "Failed initial.");

    printf("What's your last name? ");
    rc = read_scan("%s", MAX_DATA, &last_name);
    check(rc == 0, "Failed last name.");

    printf("How old are you? ");
    rc = read_scan("%d", &age);

    print("---- RESULTS ----\n");
    print("First Name: %s", first_name);
    print("Initial: '%c'\n", initial);
    print("Last Name: %s", last_name);
    print("Age: %d\n", age);

    print("---- OTHERS ----\n");

    print("%p\n", &age);
    print("%x\n", &age);

    free(first_name);
    free(last_name);
    return 0;
error:
    return -1;
}
