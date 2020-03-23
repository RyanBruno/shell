/*this function handles internal commands.
Return -1 if command is not internal
If command is internal, handle it and return 1; */
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
        exit(1); //this needs to be running in the parent process. This will kill all children.

    }

    if(!(strcmp("", "setenv"))) {
        //takes input like: "setenv var value"
        if(1 == 2) { 



        }else {
            printf("Please use: setenv var value\n");
        }

        return 1;

    }

    return -1; //return so the caller knows we have not executed the command
}
