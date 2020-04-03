//This file is for the accounting information

void rusage_add(struct rusage* dest, struct rusage* src)
{
    dest->ru_utime.tv_sec += src->ru_utime.tv_sec;
    dest->ru_utime.tv_usec+= src->ru_utime.tv_usec;
    dest->ru_stime.tv_sec += src->ru_stime.tv_sec;
    dest->ru_stime.tv_usec+= src->ru_stime.tv_usec;
    dest->ru_maxrss     += src->ru_maxrss;
    dest->ru_ixrss      += src->ru_ixrss;
    dest->ru_idrss      += src->ru_idrss;
    dest->ru_isrss      += src->ru_isrss;
    dest->ru_minflt     += src->ru_minflt;
    dest->ru_majflt     += src->ru_majflt;
    dest->ru_nswap      += src->ru_nswap;
    dest->ru_inblock    += src->ru_inblock;
    dest->ru_oublock    += src->ru_oublock;
    dest->ru_msgsnd     += src->ru_msgsnd;
    dest->ru_msgrcv     += src->ru_msgrcv;
    dest->ru_nsignals   += src->ru_nsignals;
    dest->ru_nvcsw      += src->ru_nvcsw;
    dest->ru_nivcsw     += src->ru_nivcsw;
    
}
/* Prints all of the values in formating in accordance to the man page associated
* with the rusage function. */
void rusage_print(struct rusage* r)
{
    printf("User CPU time used: %1.5lfs\n", (double) r->ru_utime.tv_sec + ((double) r->ru_utime.tv_usec / (double) 1000000));
    printf("System CPU time used: %1.5lfs\n", (double) r->ru_stime.tv_sec + ((double) r->ru_stime.tv_usec / (double) 1000000));
    printf("Max resident set size: %ld\n", r->ru_maxrss);
    printf("Integral shared memory size: %ld\n", r->ru_ixrss);
    printf("Integral unshared data size: %ld\n", r->ru_idrss);
    printf("Integral unshared stack size: %ld\n", r->ru_isrss);
    printf("Soft page faults: %ld\n", r->ru_minflt);
    printf("Hard page faults: %ld\n", r->ru_majflt);
    printf("Swaps: %ld\n", r->ru_nswap);
    printf("Block input operations: %ld\n", r->ru_inblock);
    printf("Block output operations: %ld\n", r->ru_oublock);
    printf("IPC messages: %ld\n", r->ru_msgsnd);
    printf("IPC messages received: %ld\n", r->ru_msgrcv);
    printf("Signals received: %ld\n", r->ru_nsignals);
    printf("Voluntary context switches: %ld\n", r->ru_nvcsw);
    printf("Involuntary context switches: %ld\n", r->ru_nivcsw);
}
