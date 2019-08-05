#include <stdio.h>
#include "../debug/dbg.h"

#define MAX_DATA 10

typedef enum EyeColor {
    BLUE_EYES, GREEN_EYES, BROWN_EYES,
    BLACK_EYES, OTHER_EYES
} EyeColor;

const char *EYE_COLOR_NAMES[] = {
    "Blue", "Green", "Brown", "Black", "Other"
};

typedef struct Person {
    int age;
    char first_name[MAX_DATA];
    char last_name[MAX_DATA];
    EyeColor eyes;
    float income;
} Person;

int lscanf(char *string, int max_len, char *errmsg)
{
    int len;
    char *rc = NULL;
    rc = fgets(string, max_len, stdin);
    check( rc != NULL, "%s",  errmsg);
    len = strlen(string);

    // clear stdin
    if (len > max_len - 1) while(getchar() != '\n');

    // trim newline
    if (string[len - 1] == '\n') string[len - 1] = 0;

error:
    return 1;
}

int main(int argc, char *argv[])
{
    Person you = {.age = 0 };
    int i = 0;

    printf("What's your First Name? ");
    lscanf(you.first_name, MAX_DATA, "Failed to read the first name");

    printf("What's your Last Name? ");
    lscanf(you.last_name, MAX_DATA, "Failed to read the last name");

    printf("How old are you? ");
    int rc = fscanf(stdin, "%d", &you.age);
    check(rc > 0 && you.age > 0, "You have to enter a number.");

    printf("What color are your eyes:\n");
    for (i = 0; i <= OTHER_EYES; i++) {
        printf("%d) %s\n", i + 1, EYE_COLOR_NAMES[i]);
    }
    printf("> ");

    int eyes = -1;
    rc = fscanf(stdin, "%d", &eyes);
    check(rc > 0, "You have to enter a number.");

    you.eyes = eyes - 1;
    check(you.eyes <= OTHER_EYES
            && you.eyes >= 0, "Do it right, that's not an option.");

    printf("How much do you make an hour? ");
    rc = fscanf(stdin, "%f", &you.income);
    check(rc > 0, "Enter a floating point number.");

    printf("----- RESULTS -----\n");

    printf("First Name: %s\n", you.first_name);
    printf("Last Name: %s\n", you.last_name);
    printf("Age: %d\n", you.age);
    printf("Eyes: %s\n", EYE_COLOR_NAMES[you.eyes]);
    printf("Income: %f\n", you.income);

    return 0;
error:

    return -1;
}

/* *
 * Lessons learned:
 *
 * 1. Never use fscanf, scanf for reading strings particularly from stdin. Why?
 *    i.  they don't read after spaces
 *    ii. causes buffer overflow 
 * 2. Never use gets for obvious reasons.
 * 3. No input reading function clears the stdin. It will cause you to skip the
 *    consecutive input call in case of entering more than specified number of
 *    of characters because the characters still stay in the stdin stream.
 *    So always clear stdin after reading input.
 * 4. So use something like lscanf to read strings of limited length from stdin
 *    that doesn't read newline and also clears the stdin stream.
 *
 * */
