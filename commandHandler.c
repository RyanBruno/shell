#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h> 
#include <regex.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MAXBUFSIZE 1024

#define WARN(...) { \
    fprintf(stderr, __VA_ARGS__); \
    return 1; } \

void rusage_print(struct rusage* r);

int internalCMD(char** tokens)
{
    if(!(strcmp(tokens[0], "sushi"))) { //If it is the sushi command, do 
    //Set this to do something cooler later on. Also add some other kind of functional
    //commands for like hotkeying into directories, ect
        printf("\e[1;34mShippensburg University Shell\n");
        printf("\e[1;34mYour text is now blue!\n");

        return 1;
    }

    if(!(strcmp(tokens[0], "exit"))) {
        struct rusage usage;
        struct rusage cusage;

        //Exit the entire SUSHI shell.
        printf("Thank you for using our shell\n");        
        printf("Accounting information:\n");

        /* Get and print process usage + children */
        if(getrusage(RUSAGE_SELF, &usage) == -1)
            WARN("Error getting rusage.\n");
        if(getrusage(RUSAGE_CHILDREN, &cusage) == -1)
            WARN("Error getting rusage.\n");

        /* Add then print the usage */
        rusage_add(&usage, &cusage);
        rusage_print(&usage);

        exit(1); //this needs to be running in the parent process. This will kill all children.
    }

    if(!(strcmp(tokens[0], "setenv"))) {
        //takes input like: "setenv var 'value'"
        //Check if tokens[1] || tokens[2] is null
        if(tokens[1] == NULL || tokens[2] == NULL)
            WARN("Please use: setenv var value\n");

        //do things here to set the variable
        if(setenv(tokens[1], tokens[2], 1) == -1)
            WARN("Error setting variable.\n");
        return 1;
    }

    if(!(strcmp(tokens[0], "unsetenv"))) {
        //takes input like: "unsetenv var"
        //Check if tokens[1] is null
        if(tokens[1] == NULL)
            WARN("Please use: unsetenv var\n");

        //do things here to unset the variable
        if(unsetenv(tokens[1]) == -1)
            WARN("Error unsetting variable.\n");
        return 1;
    }

    if (!(strcmp(tokens[0], "cd"))) {
        if(tokens[1] == NULL)
            WARN("Please use: cd dir\n");

        /* Change directory */
        if (chdir(tokens[1]) < 0)
            WARN("Error changing directory. No such file or directory\n");
        return 1;
    }

    if (!(strcmp(tokens[0], "pwd"))) {
        char* cwd;

        if ((cwd = getcwd(NULL, MAXBUFSIZE)) == NULL)
            WARN("Error getting current working derectory.\n");

        printf("%s\n", cwd);
        free(cwd);

        return 1;
    }

    if (!(strcmp(tokens[0], "accnt"))) {
        struct rusage usage;

        /* Get and print current processes usage */
        if(getrusage(RUSAGE_SELF, &usage) == 0)
            rusage_print(&usage);
        return 1;
    }
    return 0;
} 
