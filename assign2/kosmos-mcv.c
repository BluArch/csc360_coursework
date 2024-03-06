/*
 * kosmos-mcv.c (mutexes & condition variables)
 *
 * For UVic CSC 360, Spring 2024
 *
 * Here is some code from which to start.
 *
 * PLEASE FOLLOW THE INSTRUCTIONS REGARDING WHERE YOU ARE PERMITTED
 * TO ADD OR CHANGE THIS CODE. Read from line 133 onwards for
 * this information.
 */

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "logging.h"


/* Random # below threshold that particular atom creation. 
 * This code is a bit fragile as it depends upon knowledge
 * of the ordering of the labels.  For now, the labels 
 * are in alphabetical order, which also matches the values
 * of the thresholds.
 */

#define C_THRESHOLD 0.2
#define H_THRESHOLD 0.8
#define O_THRESHOLD 1.0
#define DEFAULT_NUM_ATOMS 40

#define MAX_ATOM_NAME_LEN 10
#define MAX_KOSMOS_SECONDS 5

/* Global / shared variables */
int  cNum = 0, hNum = 0, oNum = 0;
long numAtoms;


/* Function prototypes */
void kosmos_init(void);
void *c_ready(void *);
void *h_ready(void *);
void *o_ready(void *);
void make_radical(int, int, int, int, int, char *);
void wait_to_terminate(int);


/* Needed to pass legit copy of an integer argument to a pthread */
int *dupInt( int i )
{
	int *pi = (int *)malloc(sizeof(int));
	assert( pi != NULL);
	*pi = i;
	return pi;
}




int main(int argc, char *argv[])
{
	long seed;
	numAtoms = DEFAULT_NUM_ATOMS;
	pthread_t **atom;
	int i;
	int status;
    double random_value;

	if ( argc < 2 ) {
		fprintf(stderr, "usage: %s <seed> [<num atoms>]\n", argv[0]);
		exit(1);
	}

	if ( argc >= 2) {
		seed = atoi(argv[1]);
	}

	if (argc == 3) {
		numAtoms = atoi(argv[2]);
		if (numAtoms < 0) {
			fprintf(stderr, "%ld is not a valid number of atoms\n",
				numAtoms);
			exit(1);
		}
	}

    kosmos_log_init();
	kosmos_init();

	srand(seed);
	atom = (pthread_t **)malloc(numAtoms * sizeof(pthread_t *));
	assert (atom != NULL);
	for (i = 0; i < numAtoms; i++) {
		atom[i] = (pthread_t *)malloc(sizeof(pthread_t));
        random_value = (double)rand() / (double)RAND_MAX;

		if ( random_value <= C_THRESHOLD ) {
			cNum++;
			status = pthread_create (
					atom[i], NULL, c_ready,
					(void *)dupInt(cNum)
				);
		} else if (random_value <= H_THRESHOLD ) {
			hNum++;
			status = pthread_create (
					atom[i], NULL, h_ready,
					(void *)dupInt(hNum)
				);
		} else if (random_value <= O_THRESHOLD) {
			oNum++;
			status = pthread_create (
					atom[i], NULL, o_ready,
					(void *)dupInt(oNum)
				);
        } else {
            fprintf(stderr, "SOMETHING HORRIBLY WRONG WITH ATOM GENERATION\n");
            exit(1);
        }

		if (status != 0) {
			fprintf(stderr, "Error creating atom thread\n");
			exit(1);
		}
	}

    /* Determining the maximum number of ethynyl radicals is fairly
     * straightforward -- it will be the minimum of the number of
     * cNum, oNum, and hNum / 3.
     */
    int max_radicals = 0;

    if (cNum < oNum && cNum < hNum / 3) {
        max_radicals = cNum;
    } else if (oNum < cNum && oNum < hNum / 3) {
        max_radicals = oNum;
    } else {
        max_radicals = (int)(hNum / 3);
    }
#ifdef VERBOSE
    printf("Maximum # of radicals expected: %d\n", max_radicals);
#endif

    wait_to_terminate(max_radicals);
}

/*
* Now the tricky bit begins....  All the atoms are allowed
* to go their own way, but how does the Kosmos ethynyl-radical
* problem terminate? There is a non-zero probability that
* some atoms will not become part of a radical; that is,
* many atoms may be blocked on some semaphore of our own
* devising. How do we ensure the program ends when
* (a) all possible radicals have been created and (b) all
* remaining atoms are blocked (i.e., not on the ready queue)?
*/



/*
 * ^^^^^^^
 * DO NOT MODIFY CODE ABOVE THIS POINT.
 *
 *************************************
 *************************************
 *
 * ALL STUDENT WORK MUST APPEAR BELOW.
 * vvvvvvvv
 */

// Variables to be the max number of each atom type in a radical and the total number of atoms in a radical
#define MAX_OXYGEN 1
#define MAX_HYDROGEN 3
#define MAX_CARBON 1
#define SIZE_OF_RADICAL 5

// Variables to keep track of number of each atom currently active, and id numbers of atoms
int radicals;
int num_carbon;
int num_hydrogens;
int num_oxygen;
int num_total_atoms;

int combining_c;
int combining_o;
int combining_h[MAX_HYDROGEN] = {0};

// Combiner holds the atom that initiates the creation of a radical
char combiner[MAX_ATOM_NAME_LEN];

// Semiphores
pthread_mutex_t m;
pthread_cond_t queue;


void check_reaction(char *name){
    /*
    Function takes the name of an atom and checks if the current atom is the 5th atom of the radical.
    If the atom is not, it will suspend until the next generation of atoms begin. If the atom is the fifth,
    It will call make_radical, reset num_total_atoms, and broadcast all suspended threads.
    */
    if(num_total_atoms<SIZE_OF_RADICAL){
        int current_gen = radicals;
        while(current_gen==radicals){
            pthread_cond_wait(&queue, &m);
        }
    }else{
        num_total_atoms=0;
        make_radical(combining_c, combining_o, combining_h[0], combining_h[1], combining_h[2], name);
        pthread_cond_broadcast(&queue);
    }
}
void kosmos_init() {
    /*
     Function initiates variable values and semiphores. Returns nothing.
    */
    radicals = 1;
    num_oxygen = 0;
    num_carbon = 0;
    num_hydrogens = 0;
    num_total_atoms = 0;
    
    combining_c = 0;
    combining_o = 0;

    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&queue, NULL);
}


void *h_ready( void *arg ){
    /*
    Function handles the addition of hydrogen atoms to a radical, all threads that get here after 3 threads have gone through
    the while loop check must wait until a new radical is ready to be created. Returns NULL once finished.
    */
	int id = *((int *)arg);
    char name[MAX_ATOM_NAME_LEN];

    sprintf(name, "h%03d", id);
    pthread_mutex_lock(&m);
    // All hydrogen atoms that arrive after the first three will suspend
    while(num_hydrogens==MAX_HYDROGEN){
        pthread_cond_wait(&queue, &m);
    }
    num_hydrogens++;
    combining_h[num_hydrogens-1] = id;
    num_total_atoms++;
    // All atoms that aren't the one to create the reaction will suspend until radical is made and broadcasts
    check_reaction(name);
    pthread_mutex_unlock(&m);
	return NULL;
}


void *c_ready( void *arg ){
    /*
    Function handles the addition of carbon atoms to a radical, all threads that get here after 1 thread have gone through
    the while loop check must wait until a new radical is ready to be created. Returns NULL once finished.
    */
	int id = *((int *)arg);
    char name[MAX_ATOM_NAME_LEN];

    sprintf(name, "c%03d", id);
    pthread_mutex_lock(&m);
    // All carbon atoms that arrive after the first one
    while(num_carbon==MAX_CARBON){
        pthread_cond_wait(&queue, &m);
    }
	num_carbon++;
    combining_c = id;
    num_total_atoms++;
    // All atoms that aren't the one to create the reaction will suspend until radical is made and broadcasts
    check_reaction(name);
    pthread_mutex_unlock(&m);
	return NULL;
}


void *o_ready( void *arg ){
    /*
    Function handles the addition of oxygen atoms to a radical, all threads that get here after 1 thread have gone through
    the while loop check must wait until a new radical is ready to be created. Returns NULL once finished.
    */
	int id = *((int *)arg);
    char name[MAX_ATOM_NAME_LEN];

    sprintf(name, "o%03d", id);
    // All carbon atoms that arrive after the first one
    pthread_mutex_lock(&m);
    while(num_oxygen==MAX_OXYGEN){
        pthread_cond_wait(&queue, &m);
    }
    num_oxygen++;
    combining_o = id;
    num_total_atoms++;
    // All atoms that aren't the one to create the reaction will suspend until radical is made and broadcasts
    check_reaction(name);
	pthread_mutex_unlock(&m);
	return NULL;
}


/* 
 * Note: The function below need not be used, as the code for making radicals
 * could be located with c_ready, h_ready, or o_ready. However, it is
 * perfectly possible that you have a solution which depends on such a
 * function having a purpose as intended by the function's name.
 */
void make_radical(int c, int o, int h1, int h2, int h3, char *maker){
    /*
    Function handles the addition the creation of a radical. Takes the name of the atom that "makes" the reaction, and takes the id numbers of one carbon, one oxygen, 
    and three hydrogen atoms. The radical and the atoms in the radical are sent to the log entry function. Function finishes by resetting number counts for each atom type,
    and increases radical integer.
    */
    kosmos_log_add_entry(radicals, c, o, h1, h2, h3, maker);
    num_oxygen=0;
    num_hydrogens=0;
    num_carbon=0;
    radicals++;
}


void wait_to_terminate(int expected_num_radicals) {
    /*
    Function handles the termination of the program. 
    The function sleeps for a given amount of seconds, then terminates the program
    */
    sleep(MAX_KOSMOS_SECONDS);
    kosmos_log_dump();
    exit(0);
}
