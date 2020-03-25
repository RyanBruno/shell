//This file will take care of the accounting information required for the shell. 
#include <sys/types.h>

int getAccnt(int who, struct rusage *usage);
void printRusage(struct rusage *usage);
