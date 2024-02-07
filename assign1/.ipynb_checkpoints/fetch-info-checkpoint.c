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
 * Author: JJ Cruikshanks
 * Login:  jcruikshanks@uvic.ca
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Note: You are permitted, and even encouraged, to add other
 * support functions in order to reduce duplication of code, or
 * to increase the clarity of your solution, or both.
 */

void print_cpu_info(){
     /*
    Function opens the /proc/cpu directory and prints the line containing the model name and the line containing
    the number of cpu cores.
    */
    FILE *input = fopen("/proc/cpuinfo", "r");
    char line[200];

    while (fgets(line, sizeof(line), input)!=NULL){
        if (strstr(line, "model name")!=NULL || strstr(line, "cpu cores")){
            fprintf(stdout, "%s", line);
        }
        if (strcmp(line,"\n")==0){
            break;
        }
    }
    fclose(input);
}

void print_linux_version(){
    /*
    Function opens the /proc/version directory and prints the line containing the current version of Linux 
    of the system.
    */
    FILE *input = fopen("/proc/version", "r");
    char line[200];

    while (fgets(line, sizeof(line), input)!=NULL){
        fprintf(stdout, "%s", line);
    }
    fclose(input);
}

void print_memory_info(){
    /*
    Function opens the /proc/version directory and prints the line the total amount of memory of the system.
    */
    FILE *input = fopen("/proc/meminfo", "r");
    char line[200];

    while (fgets(line, sizeof(line), input)!=NULL){
        if (strstr(line, "MemTotal:")!=NULL){
            fprintf(stdout, "%s", line);
        }
    }
    fclose(input);
}

void print_uptime(){
    /*
    Function opens the /proc/uptime directory and converts the number of uptime seconds into days, hours,
    minutes, and seconds. The values are then printed
    */
    FILE *input = fopen("/proc/uptime", "r");
    char line[200];
    int uptime;

    int days;
    int hours;
    int minutes;
    int seconds;
    
    fgets(line, sizeof(line), input);
    char *token = strtok(line, " ");
    uptime = atoi(token);
    days = uptime/86400;
    uptime = uptime%86400;
    hours = uptime/3600;
    uptime = uptime%3600;
    minutes = uptime/60;
    uptime = uptime%60;
    seconds = uptime;

    fprintf(stdout, "Uptime: %d days, %d hours, %d minutes, %d seconds\n", days, hours, minutes, seconds);

    fclose(input);
}


void print_process_info(char * process_num) {
    /*
    Function take the process_num as input. It opens the /proc/process_num/status directory and prints the 
    name of the process, the console command that started the process if there is one, the number of threads 
    running in the process, and the total number of context switches that have occurred during the running of 
    the process.
    */
    int process = atoi(process_num);
    char path_name[30];
    sprintf(path_name, "/proc/%d/status", process);      // Add the process number to the filepath name
    FILE *input = fopen(path_name, "r");
    
    if (input!=NULL){
        char line[200];
        int total_switches = 0;
        fprintf(stdout, "Process number: %d\n", process);
        while (fgets(line, sizeof(line), input)!=NULL){
            if (strstr(line, "Name:")!=NULL){
                fprintf(stdout, "%s", line);              // Print the name of the process
                char *token = strtok(line, ":");
                token = strtok(NULL, ":");
                fprintf(stdout, "Filename (if any):%s", token);    // After tokenizing, add to filename string and print 
            } else if (strstr(line, "Threads:")!=NULL){
                fprintf(stdout, "%s", line);                       // Print the number of threads of the procecss
            } else if (strstr(line, "voluntary_ctxt_switches:")!=NULL || strstr(line, "nonvoluntary_ctxt_switches:")!=NULL){
                char *token = strtok(line, ":");
                token = strtok(NULL, ":");
                total_switches += atoi(token);                    // Add the number of voluntary context switches and nonvoluntary context switches
            }
        }
    fprintf(stdout, "Total context switches: %d\n", total_switches);     // Print total number of switches
    fclose(input);  
    } else {
        fprintf(stdout, "Process number %d not found.\n", process);
    }
} 


void print_full_info() {
    /*
    Function calls helper functions to print the model name of the CPU, the number of cores, the version of Linux running 
    in the environment, the total memory available to the system, and the amount of time the system has been running in terms 
    of days, hours, minutes and seconds.
    */
    print_cpu_info();
    print_linux_version();
    print_memory_info();
    print_uptime();
}


int main(int argc, char ** argv) {  
    /*
    Main function chooses with type of output to send based on the argument from the command line.
    */
    if (argc == 1) {
        print_full_info();
    } else {
        print_process_info(argv[1]);
    }
}
