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

#define INPUT_SIZE 1024
#define TOKEN_CAP sizeof(char *) * 8

regex_t reg;
void *null = NULL;
const char *prompt;
char input_buffer[INPUT_SIZE];

const char *st[4] = { "|" , ">>", ">" , "<" };

struct gbuf_t {
    size_t gb_s;
    size_t gb_c;
    char* gb_data;
};

struct gbuf_t b;

void gbuf_push(char* item)
{
    while (b.gb_s + sizeof(char*) > b.gb_c) {
        if ((b.gb_data = realloc(b.gb_data, b.gb_c * 2)) == NULL) {
            printf("Cannot realloc (%d)\n", errno);
            exit(-1);
        }
        b.gb_c *= 2;
    }

    memcpy(b.gb_data + b.gb_s, item, sizeof(void*));
    b.gb_s += sizeof(char*);
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
            if (strncmp(st[i], token, strlen(st[i])) == 0) {
                /* Add a null and assign the token */
                gbuf_push((void*) &null);
                token = st[i];
                /* Drop a null terminator */
                ptr[0] = '\0';
                break;
            }
        }

        /* Insert the token */
        gbuf_push((void*) &token);

        /* Null terminate the string */
        if (token[pmatch.rm_eo - pmatch.rm_so] == ' ')
            *(ptr++ + pmatch.rm_eo - pmatch.rm_so) = '\0';

        /* Move pointer */
        ptr += pmatch.rm_eo - pmatch.rm_so;
    }

    /* Double null terminate output */
    gbuf_push((void*) &null);
    gbuf_push((void*) &null);
}

void run_program()
{
    int in[2];
    int out[2];
    int n = 0;
    char** tokens;

    /* Setup */
    b.gb_s = 0;
    b.gb_c = TOKEN_CAP;
    if ((b.gb_data = malloc(TOKEN_CAP)) == NULL) {
        printf("Cannot malloc (%d)\n", errno);
        exit(-1);
    }

    /* Tokenize */
    tokenize();
    tokens = (char**) b.gb_data;

    /* Create some pipes then... */
    pipe(in);
    memcpy(&out, &in, sizeof(out));

    /* ...dup stdin/out */
    close(in[0]);
    dup(STDIN_FILENO);
    close(out[1]);
    dup(STDOUT_FILENO);

    while(tokens[0] != NULL) {
        int k = 0;

        /* Find next command */
        while (tokens[++k] != NULL) {}
        k++;
        
        /* Reset Stdout */
        if (tokens[k] == NULL) {
            close(out[1]);
            dup(STDOUT_FILENO);
        }

        /* In direction */
        if (tokens[k] != NULL && tokens[k][0] == '<') {
            close(in[0]);
            if((in[0] = open(tokens[k + 1], O_RDONLY)) == -1) {
                printf("Bad filename (%d): %s\n", errno, tokens[k + 1]);
                exit(-1);
            }
            k += 3;
        }

        /* Pipe */
        if (tokens[k] != NULL && tokens[k][0] == '|') {
            pipe(out);
            k++;
        }

        /* Out direction */
        if (tokens[k] != NULL && tokens[k][0] == '>') {
            close(out[1]);

            if (open(tokens[k + 1], O_CREAT | O_RDWR) == -1) {
                printf("Bad filename (%d): %s\n", errno, tokens[k + 1]);
                exit(-1);
            }

            if (tokens[k][1] == '>') {
                lseek(out[1], 0, SEEK_END);
            }

            k += 2;
        }

    if(internalCMD(tokens) == 1) {

    } else { //Then process must be external
        
        /* Fork and exec */
        switch (fork()) {
            case 0:
                // Child
                close(STDIN_FILENO);
                dup(in[0]);
                close(STDOUT_FILENO);
                dup(out[1]);

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
                in[0] = out[0];
                n++;
        };
    }
        tokens += k;
    }

    /* Wait for children to die */
    while (n > 0) {
        wait(NULL);
        n--;
    }

    free(b.gb_data);
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

    while (1) {
        const char *prompt;

        /* Setup */
        prompt = getenv("PS1");
        prompt = "prompt>";
        printf("%s ", prompt);

        /* Read in string */
        int i = 0;
        while ((input_buffer[i++] = getc(stdin)) != '\n') {
            if (i >= INPUT_SIZE) {
                printf("Input to large!\n");
                exit(-1);
            }
        }
        input_buffer[i - 1] = '\0';

        /* Run the program */
        run_program();
    }
}
