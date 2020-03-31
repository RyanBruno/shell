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
#include "accnt.h"

#define MAXBUFSIZE 1024

#define ERROR(msg) { \
    fprintf(stderr, msg); \
    return; } \

const char* commands[] = {
    "sushi",
    "exit",
    "setenv",
    "unsetenv",
    "cd",
    "pwd",
    "accnt",
};

int is_internal(char** tokens) {
    for (int i = 0; i < 7; i++)
        if (strcmp(tokens[0], commands[i]) == 0)
            return 1;

    return 0;
}

void internalCMD(char** tokens) {
    if(!(strcmp(tokens[0], "sushi"))) { //If it is the sushi command, do 
    //Set this to do something cooler later on. Also add some other kind of functional
    //commands for like hotkeying into directories, ect
        printf("\e[1;34mShippensburg University Shell\n");
        printf("\e[1;34mYour text is now blue!\n");

    }

    if(!(strcmp(tokens[0], "exit"))) {
        //Exit the entire SUSHI shell.
        printf("Thank you for using our shell\n");        
        printf("Accounting information:\n");

        printRusage(RUSAGE_SELF);
        exit(1); //this needs to be running in the parent process. This will kill all children.
    }

    if(!(strcmp(tokens[0], "setenv"))) {
        //takes input like: "setenv var 'value'"
        //Check if tokens[1] || tokens[2] is null
        if(tokens[1] == NULL || tokens[2] == NULL)
            ERROR("Please use: setenv var value\n");

        //do things here to set the variable
        if(setenv(tokens[1], tokens[2], 1) == -1)
            ERROR("Error setting variable.\n");
    }

    if(!(strcmp(tokens[0], "unsetenv"))) {
        //takes input like: "unsetenv var"
        //Check if tokens[1] is null
        if(tokens[1] == NULL)
            ERROR("Please use: unsetenv var\n");

        //do things here to unset the variable
        if(unsetenv(tokens[1]) == -1)
            ERROR("Error unsetting variable.\n");
    }

    if (!(strcmp(tokens[0], "cd"))) {
        char* dir = tokens[1];

        /* If given no args or ~ then change to home dir first */
        if (dir == NULL || dir[0] == '~') {
            char *homedir;

            if ((homedir = getenv("HOME")) == NULL)
               ERROR("Error! Please set the $HOME environment variable\n");

            chdir(homedir);
        }


        /* Remove ~ and ~/ */
        if (dir != NULL && dir[0] == '~') {
            dir++;
            if (dir[0] == '/')
                dir++;
        }

        if (dir == NULL || strlen(dir) == 0) return;

        /* Change directory */
        if (chdir(dir) < 0)
            ERROR("Error changing directory. No such file or directory\n");
    }

    if (!(strcmp(tokens[0], "pwd"))) {
        char cwd[MAXBUFSIZE];
        //Check and make sure it is not empty
        if ((getcwd(cwd, MAXBUFSIZE)) == NULL)
            ERROR("Error getting current working derectory.\n");

        printf("%s\n", cwd);
    }

    if (!(strcmp(tokens[0], "accnt"))) {
        //Get the accounting information for the shell
        printf("Please wait while we fetch the accounting data\n");

        printRusage(RUSAGE_SELF);
    }
} 
