#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/** Our old friend die from ex17 */
void die(const char *message)
{
    if (errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    exit(1);
}

/**
 * a typedef creates a fake type, in this
 * case for a function pointer.
 * */
typedef int (*compare_cb) (int a, int b);

/**
 * a typedef creates a fake type, in this
 * case for a function pointer.
 * */
typedef int *(*sort_cb) (int *numbers, int count, compare_cb cmp);


/* *
 * A classic bubble sort function that uses the
 * compare_cb to do the sorting. 
 * */
int *bubble_sort(int *numbers, int count, compare_cb cmp)
{
    int temp = 0;

    int *target = malloc(count *sizeof(int));

    if (!target)
        die("Memory error.");

    memcpy(target, numbers, count * sizeof(int));

    for (int i = 0; i < count; i++) {
        for (int j = 0; j < count - 1; j++) {
            if (cmp(target[j], target[j+1]) > 0) {
                temp = target[j+1];
                target[j+1] = target[j];
                target[j] = temp;              
            }
        }
    }

    printf("Using Bubble sort:    ");
    return target;
}

/* *
* A classic selection sort function that uses the
* compare_cb to do the sorting. 
* */
int *selection_sort(int *numbers, int count, compare_cb cmp)
{
    int temp = 0;
   
    int *target = malloc(count *sizeof(int));

    if (!target)
        die("Memory error.");

    memcpy(target, numbers, count * sizeof(int));


    for (int i = 0; i < count - 1; i++) {
        int imin = i;
        for (int j = (i+1); j < count; j++) {
            if (cmp(target[imin], target[j]) > 0) imin = j;
        }
        temp = target[i];
        target[i] = target[imin];
        target[imin] = temp;
    }

    printf("Using Selection sort: ");
    return target;
}



int sorted_order(int a, int b)
{
    return a - b;
}

int reverse_order(int a, int b)
{
    return b - a;
}

int strange_order(int a, int b)
{
    if (a == 0 || b == 0) {
        return 0;
    } else {
        return a % b;
    }
}

/**
 * Used to test that we are sorting things correctly
 * by doing the sort and printing it out.
 */
void test_sorting(int *numbers, int count, sort_cb sort, compare_cb cmp)
{
    int *sorted = sort(numbers, count, cmp);

    if (!sorted)
        die("Failed to sort as requested.");

    for (int i = 0; i < count; i++) {
        printf("%d ", sorted[i]);
    }
    printf("\n");

    free(sorted);
}

int main(int argc, char *argv[])
{
    if (argc < 2) die("USAGE: ex18 4 3 1 5 6");

    int count = argc - 1;
    char **inputs = argv + 1;

    int *numbers = malloc(count * sizeof(int));
    if (!numbers) die("Memory error.");

    for (int i = 0; i < count; i++) {
        numbers[i] = atoi(inputs[i]);
    }

    sort_cb sort[2] = {bubble_sort, selection_sort};
    compare_cb order[3] = {sorted_order, reverse_order, strange_order};

    for(int i = 0; i < 2; i++)
        for(int j = 0; j < 3; j++)
            test_sorting(numbers, count, sort[i], order[j]);

    free(numbers);

    return 0;
}

