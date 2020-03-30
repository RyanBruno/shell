//This file will take care of the accounting information required for the shell. 
#include <sys/types.h>

/* This function takes a pointer to a usage structure. Once we have the pointer, fill the pointer with the generic
* accounting information. Returns 1 on completion */
int getAccnt(int who, struct rusage *usage);
/* takes a pointer to a rusage structure and prints all of the values in formating in accordance to the man page associated
* with the rusage function. */
void printRusage(struct rusage *usage);
