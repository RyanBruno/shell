//This file is for the accounting information
#include "accnt.h"


int getAccnt(int who, struct rusage *usage) {
    //Fill the pointer
    if(getrusage(who, usage) > 0) {
        return 1; 
    } else {
        fprintf(stderr, "Error getting rusage.\n");
        return -1;
    }

}

void printRusage(struct rusage *usage) {
    printf("User CPU time used: %1.5lfs\n", (double) usage->ru_utime.tv_sec + ((double) usage->ru_utime.tv_usec / (double) 1000000));
    printf("System CPU time used: %1.5lfs\n", (double) usage->ru_stime.tv_sec + ((double) usage->ru_stime.tv_usec / (double) 1000000));
    printf("Max resident set size: %ld\n", usage->ru_maxrss);
    printf("Integral shared memory size: %ld\n", usage->ru_ixrss);
    printf("Integral unshared data size: %ld\n", usage->ru_idrss);
    printf("Integral unshared stack size: %ld\n", usage->ru_isrss);
    printf("Soft page faults: %ld\n", usage->ru_minflt);
    printf("Hard page faults: %ld\n", usage->ru_majflt);
    printf("Swaps: %ld\n", usage->ru_nswap);
    printf("Block input operations: %ld\n", usage->ru_inblock);
    printf("Block output operations: %ld\n", usage->ru_oublock);
    printf("IPC messages: %ld\n", usage->ru_msgsnd);
    printf("IPC messages received: %ld\n", usage->ru_msgrcv);
    printf("Signals received: %ld\n", usage->ru_nsignals);
    printf("Voluntary context switches: %ld\n", usage->ru_nvcsw);
    printf("Involuntary context switches: %ld\n", usage->ru_nivcsw);
}
