#include "commandHandler.h"
#include "accnt.h"
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
//this function handles internal commands.

#define MAXBUFSIZE 1024

struct rusage usage;

int internalCMD(char** tokens) {
    if(!(strcmp(tokens[0], "sushi"))) { //If it is the sushi command, do 
    //Set this to do something cooler later on. Also add some other kind of functional
    //commands for like hotkeying into directories, ect
        printf("\e[1;34mShippensburg University Shell\n");
        printf("\e[1;34mYour text is now blue!\n");

        return 1; //return so the caller knows we have executed the command.
    }

    if(!(strcmp(tokens[0], "exit"))) {
        //Exit the entire SUSHI shell.
        printf("Thank you for using our shell\n");        
        printf("Accounting information:\n");
        if(getAccnt(RUSAGE_SELF, &usage) > 0) { 
            fprintf(stderr, "Error getting usage. Must be borked\n");
        } else {
            printRusage(&usage);
        }
        exit(1); //this needs to be running in the parent process. This will kill all children.

    }

    if(!(strcmp(tokens[0], "setenv"))) {
        //takes input like: "setenv var 'value'"
        //Check if tokens[1] || tokens[2] is null
        if(tokens[1] != NULL && tokens[2] != NULL) {
            //do things here to set the variable
            if(setenv(tokens[1], tokens[2], 1) < 0) {
                fprintf(stderr, "Error setting variable.\n");
            }

        } else {
            printf("Please use: setenv var value\n");
        }

        return 1;

    }

    if(!(strcmp(tokens[0], "unsetenv"))) {
        //takes input like: "unsetenv var"
        //Check if tokens[1] is null
        if(tokens[1] != NULL) {
            //do things here to unset the variable
            if(unsetenv(tokens[1]) < 0) {
                fprintf(stderr, "Error unsetting variable.\n");
            }
        //if the command is not set using the proper number of arguments, remind the user how to use the command.
        } else {
            printf("Please use: unsetenv var\n");
        }

        return 1;
    }

    if(!(strcmp(tokens[0], "cd"))) {
        //takes input like: "cd 'dir'"
        //Check if tokens[1] is null
        if(tokens[1] != NULL) {
            //do things here to set the dir
            //Make sure to check if ~/ is used. If it is, replace it with home director
            
            //Check if the first two chars are ~/
            if(tokens[1][0] == '~' && tokens[1][1] == '/') {
                //If true, then we need the home directory. Hopefully it is set. If not give an error
                char *homedir;
                if ((homedir = getenv("HOME")) == NULL) {
                   fprintf(stderr, "Error! Please set the $HOME environment variable\n");
                   return 1;
                }
                //Move the memory over two spaces to delete the first two tokens.
                memmove(tokens[1], &tokens[1][2], strlen(tokens[1]) - 1);

                //manipulate the strings to get the desired output
                strcat(homedir, "/");
                strcat(homedir, tokens[1]);
                strcpy(tokens[1], homedir);
            }
            printf("%s\n", tokens[1]);
            if(chdir(tokens[1]) < 0) {
                //we are borked
                fprintf(stderr, "Error changing directory. No such file or directory\n");
            }

        } else {
            //Then we assume that the user wanted to change to the $HOME variable
                char *homedir;
                if ((homedir = getenv("HOME")) == NULL) {
                   fprintf(stderr, "Error! Please set the $HOME environment variable\n");
                   return 1;
                }
                if(chdir(homedir) < 0) {
                //we are borked
                fprintf(stderr, "Error changing directory. No such file or directory\n");
            }
            //Print the users new directory
            printf("%s\n", homedir);
        }

        return 1;
    }

    if(!(strcmp(tokens[0], "pwd"))) {
        //Print the working directory
        printf("Thank you for using our shell\n");
        //set a max buffer size. 
        char cwd[MAXBUFSIZE];
        //Check and make sure it is not empty
        if(getcwd(cwd, sizeof(cwd)) != NULL) {
            //Line is not empty, print line
            printf("%s\n", cwd);
        } else {
            //Somehow the CWD is in an error state. Print to STDERR
            fprintf(stderr, "Error getting current working derectory.\n");
        }
        return 1;
    }

    if(!(strcmp(tokens[0], "accnt"))) {
        //Get the accounting information for the shell
        printf("Please wait while we fetch the accounting data\n");
        if(getAccnt(RUSAGE_SELF, &usage) > 0) {
            //Accounting information returned -1. Must be borked. Print to STDERR
            fprintf(stderr, "Error getting usage. Must be borked\n");
        } else { 
            //Returned 1. Pointer is full. Print the rusage structure.
            printRusage(&usage);
        }
        return 1; //return to the caller complete.
    }

        if(!(strcmp(tokens[0], "ls"))) {
        //if someone types ls into the shell, we need to make sure that the ~/ variable is set properly.
            if(tokens[1] != NULL) {
            //Make sure to check if ~/ is used. If it is, replace it with home director
            
            //Check if the first two chars are ~/
            if(tokens[1][0] == '~' && tokens[1][1] == '/') {
                //If true, then we need the home directory. Hopefully it is set. If not give an error
                char *homedir;
                if ((homedir = getenv("HOME")) == NULL) {
                   fprintf(stderr, "Error! Please set the $HOME environment variable\n");
                   return 1;
                }
                //Move the memory over two spaces to delete the first two tokens.
                memmove(tokens[1], &tokens[1][2], strlen(tokens[1]) - 1);

                //manipulate the strings to get the desired output
                strcat(homedir, "/");
                strcat(homedir, tokens[1]);
                strcpy(tokens[1], homedir);
            }
            printf("%s\n", tokens[1]);
            if(chdir(tokens[1]) < 0) {
                //we are borked
                fprintf(stderr, "Error changing directory. No such file or directory\n");
            }

        }
    


        return -1; //return -1, but with the changed variable so the caller will issue like normal just with the changed variable.
    }

    return -1; //return so the caller knows we have not executed the command
} 
