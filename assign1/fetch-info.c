/* getstats.c 
 *
 * CSC 360, Spring 2024
 *
 * - If run without an argument, prints information about 
 *   the computer to stdout.
 *
 * - If run with a process number created by the current user, 
 *   prints information about that process to stdout.
 *
 * Please change the following before submission:
 *
 * Author: Din Grogu
 * Login:  babyyoda@uvic.ca 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Note: You are permitted, and even encouraged, to add other
 * support functions in order to reduce duplication of code, or
 * to increase the clarity of your solution, or both.
 */

void print_process_info(char * process_num) {
    printf("No soup for you....\n");
} 


void print_full_info() {
    printf("No soup for you...\n");
}


int main(int argc, char ** argv) {  
    if (argc == 1) {
        print_full_info();
    } else {
        print_process_info(argv[1]);
    }
}
