//This file will take care of the accounting information required for the shell. 
#include <sys/types.h>

/* takes a pointer to a rusage structure and prints all of the values in formating in accordance to the man page associated
* with the rusage function. */
void printRusage(int who);
