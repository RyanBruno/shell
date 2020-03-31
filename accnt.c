//This file is for the accounting information
#include <stdio.h>
#include <stdlib.h> 
#include <regex.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <errno.h>


/* Prints all of the values in formating in accordance to the man page associated
* with the rusage function. */
void printRusage(int who) {
    struct rusage usage;

    if(getrusage(who, &usage) == -1)
        fprintf(stderr, "Error getting rusage.\n");

    printf("User CPU time used: %1.5lfs\n", (double) usage.ru_utime.tv_sec + ((double) usage.ru_utime.tv_usec / (double) 1000000));
    printf("System CPU time used: %1.5lfs\n", (double) usage.ru_stime.tv_sec + ((double) usage.ru_stime.tv_usec / (double) 1000000));
    printf("Max resident set size: %ld\n", usage.ru_maxrss);
    printf("Integral shared memory size: %ld\n", usage.ru_ixrss);
    printf("Integral unshared data size: %ld\n", usage.ru_idrss);
    printf("Integral unshared stack size: %ld\n", usage.ru_isrss);
    printf("Soft page faults: %ld\n", usage.ru_minflt);
    printf("Hard page faults: %ld\n", usage.ru_majflt);
    printf("Swaps: %ld\n", usage.ru_nswap);
    printf("Block input operations: %ld\n", usage.ru_inblock);
    printf("Block output operations: %ld\n", usage.ru_oublock);
    printf("IPC messages: %ld\n", usage.ru_msgsnd);
    printf("IPC messages received: %ld\n", usage.ru_msgrcv);
    printf("Signals received: %ld\n", usage.ru_nsignals);
    printf("Voluntary context switches: %ld\n", usage.ru_nvcsw);
    printf("Involuntary context switches: %ld\n", usage.ru_nivcsw);
}
