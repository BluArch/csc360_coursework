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
    /*
    Function takes a one dimensional character array, a 2D array, an integer, and an array of integers as input.
    The string in the array "input" is tokenized with the contents being stored in the 2D at the given row index
    The number of arguments the current row contains is stored in num_arguments.
    */
    char *token;
    token = strtok(input, " ");
    int arguments = 0;

    while(token!=NULL){
        strcpy(commands[row][arguments], token);     // Store current token value into commands
        arguments++;
        token = strtok(NULL, " ");
    }

    num_arguments[row] = arguments;     // Number of arguments are stored to be refered to later
    strcpy(commands[row][arguments], "\0");
}

int get_command_inputs(char commands[MAX_NUM_INPUTS][MAX_NUM_TOKEN][MAX_LINE_LEN], int num_arguments[4]){
    /*
    Function takes a 2D array, a one dimensional character array, and an array of integers as input.
    Input is taken from the command line using read and sent to tokenize_input to be parsed into "commands".
    The number of rows read from the command line is then returned.
    */
    char input[MAX_LINE_LEN];
    int num_row = 0;

    for (int i = 0; i<MAX_NUM_INPUTS; i++){
        int num_chars_read = read(0, input, MAX_LINE_LEN);       // Take input from command line
        if (num_chars_read==1){
            break;
        } else {
            input[num_chars_read-1] = '\0';      // Replace newline character with terminating 0
            tokenize_input(input, commands, num_row, num_arguments);
            num_row++;
        }
    }
    return num_row;
}

int main() {
    /*
    Main function sends the 2D array "commands" to be filled by reading inputs from the command line. The number of lines of input is stored in "num_rows"
    with the number of arguments per row stored in "num_arguments". The command and parameters of each row is piped from one child to the next using the 
    file descriptors "fd_old" and "fd_new". Each iteration of the loop, the new file descriptor is swapped to become the old one. The function results in the piped commands
    being executed.
    */
    char commands[MAX_NUM_INPUTS][MAX_NUM_TOKEN][MAX_LINE_LEN];
    int num_rows;
    int num_arguments[MAX_NUM_INPUTS] = {0};
    
    num_rows = get_command_inputs(commands, num_arguments);
    
    char *envp[] = { 0 };
    int status;
    int pid;
    int fd_new[2], fd_old[2];


    for (int i = 0; i<num_rows; i++){
        pipe(fd_new);
        char *cur_commands[MAX_NUM_TOKEN];

        
        for (int j = 0; j<num_arguments[i]; j++){    // Move current row contents of commands into cur_commands
            cur_commands[j] = commands[i][j]; 
        }
        cur_commands[num_arguments[i]] = NULL;
        

        if((pid = fork())==0){
            if (i!=0){                  // On all iterations except the first, we redirect stdIn from the previous file descriptor
                dup2(fd_old[0], 0);
                close(fd_old[1]);
                close(fd_old[0]);
            }

            if(i!=(num_rows-1)){        // On all iterations except the last, re redirect stdOut to the new file descriptor
                dup2(fd_new[1], 1);     
                close(fd_new[1]);
                close(fd_new[0]);
            }
            execve(cur_commands[0], cur_commands, envp);         
        }
        
        if(i!=0){                      // On all iterations except the first, we close the old file descriptor
            close(fd_old[1]);
            close(fd_old[0]);
        }
        if(i!=(num_rows-1)){           // On all iterations except the last, the "new" file descriptor becomes the "old" file descriptor
            fd_old[0] = fd_new[0];         
            fd_old[1] = fd_new[1];
        }
    }
    close(fd_new[1]);                  // Close the new file descriptor
    close(fd_new[0]);
    
    while(wait(&status)>0);            // Wait until the children have all finished
    exit(0);
}
