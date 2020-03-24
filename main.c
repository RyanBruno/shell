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
#include "gbuf.c"
#include "commandHandler.h"

#define INPUT_SIZE  1024
#define TOKEN_CAP   sizeof(char *) * 8
#define PID_CAP     sizeof(pid_t) * 8

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
struct gbuf_t pid_buf;

void signal_handle(int sig)
{
    if (!(sig & SIGCHLD)) {
        pid_t* p = (pid_t*) (pid_buf.gb_data);
        if (p == NULL) return;

        for (int i = pid_buf.gb_s / sizeof(pid_t); i > 0; i--)
            kill(pid_buf.gb_data[i - 1], sig);
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
            /* Because of how this if statement works "<<" must
             * be before "<" in "st"
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

void run_program()
{
    int n = 0;
    int next_fd = -1;
    int fd[2];
    char** tokens;

    /* Setup */
    gbuf_setup(&tok_buf, TOKEN_CAP);
    gbuf_setup(&pid_buf, PID_CAP);

    /* Tokenize */
    tokenize();
    tokens = (char**) tok_buf.gb_data;

    /* Create some pipes then... */
    pipe(fd);

    /* Dup stdin to one and close fd[0] */
    dup2(STDIN_FILENO, fd[0]);
    close(fd[1]);

    while(tokens[0] != NULL) {
        pid_t pid;
        int k = 0;

        /* Find next command */
        while (tokens[++k] != NULL) {}
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

        if(internalCMD(tokens) == 1) {
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
                gbuf_push(&pid_buf, (char*) &pid, sizeof(pid_t));
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
    free(pid_buf.gb_data);
    pid_buf.gb_data = NULL;
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

int main(int argc, const char** argv)
{
    /* Compile regex */
    regcomp(&reg, "(\"[^\"]+\"|<|>|>>|\\||[^ <>(>>)\\|]+)", REG_EXTENDED);

    /* 
     * Read .sushrc if exists
     * line-by-line send to command handler
     */

    /* Register signal handers */
    pid_buf.gb_data = NULL;
    signal(SIGINT | SIGCHLD | SIGUSR1 | SIGUSR2, signal_handle);

    while (1) {
        const char *prompt;

        /* Setup */
        prompt = getenv("PS1");
        prompt = "prompt>";
        printf("%s ", prompt);
        fflush(stdout);

        /* Read in string */
        read_line(STDIN_FILENO);
        

        /* Run the program */
        run_program();
    }
}
