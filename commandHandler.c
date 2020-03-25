#include "commandHanlder.h"
//this function handles internal commands.

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
        if(tokens[1] && tokens[2] != NULL) { 
            //do things here to set the variable
            if(setenv(tokens[1], tokens[2],1) < 0) {
                    fprintf(stderr, "Error setting variable.\n");
            }

            
        }else {
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

        }else {
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
            }
                //Move the memory over two spaces to delete the first two tokens.
                memmove(tokens[1], &tokens[1][2], strlen(tokens[1]));
                
                
                strcat(homedir, "/");
                strcat(homedir, tokens[1]);
                strcpy(tokens[1], homedir);
                
            }
            printf("%s\n", tokens[1]);
            if(chdir(tokens[1]) < 0) { 
                //were borked
                fprintf(stderr, "Error changing directory. No such file or directory\n");
            }

        }else {
            printf("Please use: cd 'dir'\n");
        }

        return 1;

    }

    if(!(strcmp(tokens[0], "pwd"))) {
        //Print the working directory
        printf("Thank you for using our shell\n");
        char cwd[1024];
        if(getcwd(cwd, sizeof(cwd)) != NULL) {
                //Check and make sure it is not empty
            printf("%s\n", cwd);
        } else {
            fprintf(stderr, "Error getting current working derectory.\n");
        }
        return 1;
    }

    if(!(strcmp(tokens[0], "accnt"))) {
        //Get the accounting information for the shell
        printf("Please wait while we fetch the accounting data\n");
        if(getAccnt(RUSAGE_SELF, &usage) > 0) {
            fprintf(stderr, "Error getting usage. Must be borked\n");
        } else {
         printRusage(&usage);
        }
        return 1;
    }



    return -1; //return so the caller knows we have not executed the command
} 
