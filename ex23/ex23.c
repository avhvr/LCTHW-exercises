#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../debug/dbg.h"

/**
 * a typedef creates a fake type, in this
 * case for a function pointer
 */
typedef int (*device_copy) (char *from, char *to, int count);

int time_it(device_copy func, char *name, char *to, char *from, int count)
{
    int rc = 0;
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    rc = func(to, from, count);
    end = clock();

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to run %s is %lf\n", name, cpu_time_used);
    return rc;
}

int normal_copy(char *to, char *from, int count)
{
    int i = 0;
    for(i = 0; i < count; i++) {
        to[i] = from[i];
    }

    return i;
}

int duffs_device(char *to, char *from, int count)
{
    {
        int n = (count + 7) / 8;

        switch (count % 8) {
            case 0:
                do {
                    *to++ = *from++;
                case 7:
                *to++ = *from++;
                case 6:
                *to++ = *from++;
                case 5:
                *to++ = *from++;
                case 4:
                *to++ = *from++;
                case 3:
                *to++ = *from++;
                case 2:
                *to++ = *from++;
                case 1:
                *to++ = *from++;
                } while (--n > 0);
        }
    }

    return count;
}

int zeds_device(char *to, char *from, int count)
{
    {
        int n = (count + 7) / 8;

        switch (count % 8) {
            case 0:
again:      *to++ = *from++;

            case 7:
            *to++ = *from++;
            case 6:
            *to++ = *from++;
            case 5:
            *to++ = *from++;
            case 4:
            *to++ = *from++;
            case 3:
            *to++ = *from++;
            case 2:
            *to++ = *from++;
            case 1:
            *to++ = *from++;
            if (--n > 0)
                goto again;
        }
    }

    return count;
}

int valid_copy(char *data, int count, char expects)
{
    int i = 0;
    for (i = 0; i < count; i++) {
        if (data[i] != expects) {
            log_err("[%d] %c != %c", i, data[i], expects);
            return 0;
        }
    }

    return 1;
}

int main(int argc, char *argv[])
{
    char from[1000000] = {'a'};
    char to[1000000] = {'c'};
    int rc = 0;

    // set up the from to have some stuff
    memset(from, 'x', 1000000);
    // set it to a failure mode
    memset(to, 'y', 1000000);
    check(valid_copy(to, 1000000, 'y'), "Not iniatialized right.");

    // use normal copy to
    rc = time_it(normal_copy, "normal_copy", to, from, 1000000);
    check(rc == 1000000, "Normal copy failed: %d", rc);
    check(valid_copy(to, 1000000, 'x'), "Normal copy failed.");

    // reset
    memset(to, 'y', 1000000);

    // duffs version
    rc = time_it(duffs_device, "duffs_device", to, from, 1000000);
    check(rc == 1000000, "Duff's device failed: %d", rc);
    check(valid_copy(to, 1000000, 'x'), "Duff's device failed copy.");

    // reset
    memset(to, 'y', 1000000);

    // zed's version
    rc = time_it(zeds_device, "zeds_device", to, from, 1000000);
    check(rc == 1000000, "Zed's device failed: %d", rc);
    check(valid_copy(to, 1000000, 'x'), "Zed's device failed copy.");

    return 0;
error:
    return 1;
}
