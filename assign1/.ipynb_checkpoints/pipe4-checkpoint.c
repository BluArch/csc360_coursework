/* gopipe.c
 *
 * CSC 360, Spring 2024
 *
 * Execute up to four instructions, piping the output of each into the
 * input of the next.
 *
 * Please change the following before submission:
 *
 * Author: JJ Cruikshanks
 * Login:  jcruikshanks@uvic.ca
 */


/* Note: The following are the **ONLY** header files you are
 * permitted to use for this assignment! */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>

#define MAX_LINE_LEN 80
#define MAX_NUM_TOKEN 8
#define MAX_NUM_INPUTS 4

void tokenize_input(char input[MAX_LINE_LEN], char commands[MAX_NUM_INPUTS][MAX_NUM_TOKEN][MAX_LINE_LEN], int row, int num_arguments[4]){
    char *token;
    token = strtok(input, " ");
    int index = 0;
    int arguments = 0;

    while(token!=NULL){
        strcpy(commands[row][arguments], token);
        arguments++;
        index++;
        token = strtok(NULL, " ");
    }
    num_arguments[row] = arguments;
    strcpy(commands[row][index], "\0");
}

int get_command_inputs(char commands[MAX_NUM_INPUTS][MAX_NUM_TOKEN][MAX_LINE_LEN], char input[MAX_LINE_LEN], int num_arguments[4]){
    /*
    Function takes input from the command line for commands and arguments, store given input into a given 3D array base on where 
    */
    int num_row = 0;

    for (int i = 0; i<MAX_NUM_INPUTS; i++){
        int num_chars_read = read(0, input, MAX_LINE_LEN);
        if (num_chars_read==1){
            break;
        } else {
            input[num_chars_read-1] = '\0';
            tokenize_input(input, commands, num_row, num_arguments);
            num_row++;
        }
    }
    return num_row;
}

int main() {
    char commands[MAX_NUM_INPUTS][MAX_NUM_TOKEN][MAX_LINE_LEN];
    char input[MAX_LINE_LEN];
    int num_arguments[MAX_NUM_INPUTS] = {0};
    int num_rows;
    
    num_rows = get_command_inputs(commands, input, num_arguments);
    
    char *envp[] = { 0 };
    int status;
    int pid;
    int fd_new[2], fd_old[2];


    for (int i = 0; i<num_rows; i++){
        pipe(fd_new);
        char *cur_commands[MAX_NUM_TOKEN];

        // Create pointer array for curr_command line to execute
        for (int j = 0; j<num_arguments[i]; j++){
            cur_commands[j] = commands[i][j]; 
        }
        cur_commands[num_arguments[i]] = NULL;
        

        if((pid = fork())==0){
            if (i!=0){
                dup2(fd_old[0], 0);
                close(fd_old[1]);
                close(fd_old[0]);
            }

            if(i!=(num_rows-1)){
                dup2(fd_new[1], 1);
                close(fd_new[1]);
                close(fd_new[0]);
            }
            execve(cur_commands[0], cur_commands, envp);         
        }
        
        if(i!=0){
            close(fd_old[1]);
            close(fd_old[0]);
        }
        fd_old[0] = fd_new[0];
        fd_old[1] = fd_new[1];
    }
    close(fd_new[1]);
    close(fd_new[0]);
    
    while(wait(&status)>0);
    exit(0);
}
