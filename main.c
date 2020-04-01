#include <stdio.h>
#include <stdlib.h> 
#include <regex.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gbuf.c"
#include "accnt.h"
#include "commandHandler.c"

#define INPUT_SIZE  1024
#define TOKEN_CAP   sizeof(char *) * 8
#define EXTRA_CAP   sizeof(char *) * 8

#define CHECK(i, msg, ...) \
    if (i == -1) {\
        fprintf(stderr, msg, __VA_ARGS__); \
        exit(-1); }

regex_t reg;
void *null = NULL;
const char *prompt;
char input_buffer[INPUT_SIZE];

const char *st[4] = { "|" , ">>", ">" , "<" };

struct gbuf_t tok_buf;
struct gbuf_t ext_buf;

volatile int r = 0;

void signal_handle(int sig)
{
    if (sig & SIGINT) {
        if (r--)
            return;

        kill(0, sig);
    } else if (sig & SIGCHLD) {
        wait(NULL);
    } else if (sig & SIGUSR1) {
        printRusage(RUSAGE_SELF);
    } else if (sig & SIGUSR2) {
        printRusage(RUSAGE_SELF); // IDK what to use here
    }
}

void tokenize()
{
    char* ptr = input_buffer;
    while (1) {
        regmatch_t pmatch;
        const char* token;

        /* Match a token */
        if (regexec(&reg, ptr, 1, (regmatch_t *) &pmatch, 0))
            break;

        /* Move pointer */
        ptr += pmatch.rm_so;
        token = ptr;

        /* Handle special tokens */
        for (int i = 0; i < 4; i++) {
            /* Because of how this if statement works
             * "<<" must be before "<" in "st"
             */
            if (strncmp(st[i], token, strlen(st[i])) == 0) {
                /* Add a null and assign the token */
                gbuf_push(&tok_buf, (void*) &null, sizeof(char*));
                token = st[i];
                /* Drop a null terminator */
                ptr[0] = '\0';
                break;
            }
        }

        /* Insert the token */
        gbuf_push(&tok_buf, (void*) &token, sizeof(char*));

        /* Null terminate the string */
        if (token[pmatch.rm_eo - pmatch.rm_so] == ' ')
            *(ptr++ + pmatch.rm_eo - pmatch.rm_so) = '\0';

        /* Move pointer */
        ptr += pmatch.rm_eo - pmatch.rm_so;
    }

    /* Double null terminate output */
    gbuf_push(&tok_buf, (void*) &null, sizeof(char*));
    gbuf_push(&tok_buf, (void*) &null, sizeof(char*));
}

void replace_home_dir(char** tokens)
{
    const char* home;

    if ((home = getenv("HOME")) == NULL) {
        /* Just print a message and fall through */
        fprintf(stderr, "Home directory is not set\n");
        return;
    }

    while (1) {
        /* Loop until the double NULL */
        if (tokens[0] == NULL) {
            if (tokens[1] == NULL) break;
            /* My only goto statement I sware */
            goto loop;
        }

        /* Ignore any quoted entries */
        if (tokens[0][0] == '"')
            /* I lied */
            goto loop;

        for (char* ptr = tokens[0]; ptr[0] != '\0'; ptr++) {
            if (ptr[0] == '~') {
                /* Split the string at the ~ */
                ptr[0] = '\0';

                /* Copy the first part */
                tokens[0] = gbuf_push_2(&ext_buf, tokens[0], strlen(tokens[0]));
                /* Copy the home dir */
                            gbuf_push(&ext_buf, home, strlen(home));
                /* Then the rest of the string */
                ptr =       gbuf_push_2(&ext_buf, ptr + 1, strlen(ptr + 1));
            }
        }
loop:
        /* Go to the next word */
        tokens++;
    }
}

void run_program()
{
    int n = 0;
    int next_fd = -1;
    int fd[2];
    char** tokens;

    /* Setup */
    gbuf_setup(&tok_buf, TOKEN_CAP);
    gbuf_setup(&ext_buf, EXTRA_CAP);

    /* Tokenize */
    tokenize();
    tokens = (char**) tok_buf.gb_data;

    /* Replace ~ the home dir */
    replace_home_dir(tokens);

    /* Create some pipes then... */
    pipe(fd);

    /* Dup stdin to one and close fd[0] */
    dup2(STDIN_FILENO, fd[0]);
    close(fd[1]);

    while(tokens[0] != NULL) {
        pid_t pid;
        int k = 0;

        /* Find next command */
        while (tokens[++k] != NULL) { }
        k++;
        
        /* Reset Stdout */
        if (tokens[k] == NULL)
            dup2(STDOUT_FILENO, fd[1]);

        /* In direction */
        if (tokens[k] != NULL && tokens[k][0] == '<') {
            close(fd[0]);

            CHECK((fd[0] = open(tokens[k + 1], O_RDONLY)),
                    "Bad filename (%d): %s\n", errno, tokens[k + 1])

            k += 3;
        }

        /* Pipe */
        if (tokens[k] != NULL && tokens[k][0] == '|') {
            int p[2];
            pipe(p);
            next_fd = p[0];
            fd[1] = p[1];

            k++;
        }

        /* Out direction */
        if (tokens[k] != NULL && tokens[k][0] == '>') {
            if (fd[1] != STDOUT_FILENO) close(fd[1]);

            CHECK((fd[1] = open(tokens[k + 1], O_CREAT | O_RDWR)),
                "Bad filename (%d): %s\n", errno, tokens[k + 1])

            /* Append */
            if (tokens[k][1] == '>')
                lseek(fd[1], 0, SEEK_END);

            k += 2;
        }

        if (is_internal(tokens)) {
            internalCMD(tokens);
            tokens += k;
            continue;
        }

        /* Fork and exec */
        switch ((pid = fork())) {
            case 0:
                // Child
                dup2(fd[0], STDIN_FILENO);
                dup2(fd[1], STDOUT_FILENO);

                execvp(tokens[0], (char * const*) tokens);
                printf("Could not exec your command: %s\n", tokens[0]);
                exit(-1);
                break;
            case -1:
                // Fail
                printf("Could not fork\n");
                exit(-1);
                break;
            default:
                // Parent
                close(fd[0]);
                close(fd[1]);
                fd[0] = next_fd;
                n++;
        };
        tokens += k;
    }

    /* Wait for children to die */
    while (n-- > 0)
        wait(NULL);

    free(tok_buf.gb_data);
    free(ext_buf.gb_data);
}

void read_line(int fd)
{
    size_t i = 0;
    char c = '\0';

    do {
        if (read(fd, &c, sizeof(char)) != sizeof(char))
            exit(-1);
        input_buffer[i++] = c;
    } while (c != '\n' && i < INPUT_SIZE);

    input_buffer[i - 1] = '\0';
}

int readSushrc() {

    /* this function will read all of the lines from the file and print them */
    char homedir[INPUT_SIZE];
    char *homeenv; 
        if ((homeenv = getenv("HOME")) == NULL) {
            //If the home variable is not set, we cannot check for the .sushrc file
            fprintf(stderr, "Error! Please set the $HOME environment variable\n");
           return -1; //failed to read file
        }

        //Make sure to copy the content of the pointer, not the pointer
        strcpy(homedir, homeenv);
        strcat(homedir,"/.sushrc");
        
    FILE *fid = fopen(homedir,"r");
    if(fid == NULL) {
        printf("%s not found\n", homedir);
        return -1; //failed to read file
    }


    char buf[INPUT_SIZE];
    // reads text in from .sushrc file
    while (fgets(buf, sizeof(buf), fid)) {
        printf("%s", buf);
        buf[(strlen(buf)) -1] = '\0'; //set a terminate token at the end of the string.
        strcpy(input_buffer, buf);
        run_program();
    }
    fclose(fid);

    

return 1; // read file success
}

int main(int argc, const char** argv)
{

    /* Compile regex */
    regcomp(&reg, "(\"[^\"]+\"|<|>|>>|\\||[^ <>(>>)\\|]+)", REG_EXTENDED);

    /* Register signal handers */
    setsid();
    signal(SIGINT, signal_handle);
    signal(SIGCHLD, signal_handle);
    signal(SIGUSR1, signal_handle);
    signal(SIGUSR2, signal_handle);

    /* 
     * Read .sushrc if exists
     * line-by-line send to command handler
     */
    readSushrc();


    while (1) {
        /* Setup */
        if ((prompt = getenv("PS1")) == NULL)
            prompt = "#";

        prompt = "#";
        /* Print prompt */
        printf("%s ", prompt);
        fflush(stdout);

        /* Read in string */
        read_line(STDIN_FILENO);

        /* Run the program */
        run_program();
    }
}
