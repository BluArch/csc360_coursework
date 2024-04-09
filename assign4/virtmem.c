/*
 * Skeleton code for CSC 360, Spring 2024,  Assignment #4
 *
 * Prepared by: Michael Zastre (University of Victoria) 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * Some compile-time constants.
 */

#define REPLACE_NONE 0
#define REPLACE_FIFO 1
#define REPLACE_LRU  2
#define REPLACE_CLOCK 3
#define REPLACE_OPTIMAL 4


#define TRUE 1
#define FALSE 0
#define PROGRESS_BAR_WIDTH 60
#define MAX_LINE_LEN 100


/*
 * Some function prototypes to keep the compiler happy.
 */
int setup(void);
int teardown(void);
int output_report(void);
long resolve_address(long, int);
void error_resolve_address(long, int);
long handle_fifo(long, long, int);
long handle_lru(long, long, int);
long handle_clock(long, long, int);

/*
 * Variables used to keep track of the number of memory-system events
 * that are simulated.
 */
int page_faults = 0;
int mem_refs    = 0;
int swap_outs   = 0;
int swap_ins    = 0;

/*
 * Variables used to keep track of Clock hand and FIFO indexing
 */
int fifo_index = 0;
int clock_hand = 0;

/*
 * Page-table information. You are permitted to modify this in order to
 * implement schemes such as CLOCK. However, you are not required
 * to do so.
 */
struct page_table_entry *page_table = NULL;
struct page_table_entry {
    long page_num;
    int use;
    int dirty;
    int free;
    long frame;

    // For linked list
    struct page_table_entry *next;
    struct page_table_entry *prev;
};

// Variables to keep track of both ends of the linked list of page table entries
struct page_table_entry *start_of_queue = NULL;
struct page_table_entry *end_of_queue = NULL;

/*
 * These global variables will be set in the main() function. The default
 * values here are non-sensical, but it is safer to zero out a variable
 * rather than trust to random data that might be stored in it -- this
 * helps with debugging (i.e., eliminates a possible source of randomness
 * in misbehaving programs).
 */

int size_of_frame = 0;  /* power of 2 */
int size_of_memory = 0; /* number of frames */
int page_replacement_scheme = REPLACE_NONE;

/*
 * Function that takes a page_table_entry as input and places it in the
 * back of the linked list, end_of_queue and start_of_queue are updated
 * as needed.
 */
void add_to_back(struct page_table_entry *page)
{
    // If the list is currently empty
    if(start_of_queue == NULL && end_of_queue == NULL){
        end_of_queue = page;
        start_of_queue = page;
        return;
    // Add to end of list
    }else{
        end_of_queue->prev = page;
        page->next = end_of_queue;
        end_of_queue = page;
    }
}

/*
 * Function that takes a page_table_entry as input and removes it from it's
 * current position in the linked list and places it at the end of the linked
 * list, end_of_queue and start_of_queue are updated as needed.
 */
void swap_to_back(struct page_table_entry *page)
{
    // If the list only has "page" in it
    if(start_of_queue != page && end_of_queue != page){
        page->prev->next = page->next;
        page->next->prev = page->prev;

        end_of_queue->prev = page;
        page->next = end_of_queue;
        page->prev = NULL;
        end_of_queue = page;
    // If page is at the start of the lsit
    } else if(start_of_queue == page){
        start_of_queue = page->prev;
        page->prev->next = NULL;

        add_to_back(page);
    }
}

/*
 * Function to convert a logical address into its corresponding 
 * physical address. The value returned by this function is the
 * physical address (or -1 if no physical address can exist for
 * the logical address given the current page-allocation state.
 */

long resolve_address(long logical, int memwrite)
{
    int i;
    long page, frame;
    long offset;
    long mask = 0;
    long effective;

    /* Get the page and offset */
    page = (logical >> size_of_frame);

    for (i=0; i<size_of_frame; i++) {
        mask = mask << 1;
        mask |= 1;
    }
    offset = logical & mask;

    /* Find page in the inverted page table. */
    frame = -1;
    for ( i = 0; i < size_of_memory; i++ ) {
        if (!page_table[i].free && page_table[i].page_num == page) {
            frame = i;
            break;
        }
    }

    /* If frame is not -1, then we can successfully resolve the
     * address and return the result. */
    if (frame != -1) {
        effective = (frame << size_of_frame) | offset;

        // If we are perforing a memwrite update dirty bit
        if(memwrite == TRUE){
            page_table[i].dirty = TRUE;
        }
        // Update use bit
        page_table[i].use = TRUE;
        
        // Store current frame value into page_table_entry
        page_table[i].frame = i;

        // Frame is most recently used, move to back of queue
        swap_to_back(&page_table[i]);
        return effective;
    }


    /* If we reach this point, there was a page fault. Find
     * a free frame. */
    page_faults++;

    for ( i = 0; i < size_of_memory; i++) {
        if (page_table[i].free) {
            frame = i;
            break;
        }
    }

    /* If we found a free frame, then patch up the
     * page table entry and compute the effective
     * address. Otherwise return -1.
     */
    if (frame != -1) {
        page_table[frame].page_num = page;
        page_table[i].free = FALSE;
        swap_ins++;
        effective = (frame << size_of_frame) | offset;

        // If we are performing a memwrite, update dirty
        if(memwrite == TRUE){
            page_table[frame].dirty = TRUE;
        }

        // Frame is most recently used, move to back of queue
        add_to_back(&page_table[i]);
        return effective;
    } else {
        if(page_replacement_scheme == REPLACE_FIFO){
            return handle_fifo(page, offset, memwrite);
        }else if(page_replacement_scheme == REPLACE_LRU){
            return handle_lru(page, offset, memwrite);
        }else{
            return handle_clock(page, offset, memwrite);
        }
    }
}

/*
 * Function performs page replacement based on fifo algorithm.
 * Swap is performed and fifo index is updated. Dirty and use
 * bits are updated as needed. Swap-ins and swap-outs updated as 
 * needed. Returns effective.
 */
long handle_fifo(long new_page, long offset, int memwrite)
{
    // Handle fifo_index so it doesn't go out of bounds
    fifo_index = fifo_index % size_of_memory;
    
    long frame = page_table[fifo_index].frame;
    page_table[fifo_index].page_num = new_page;

    // Check if bit was dirty
    if(page_table[fifo_index].dirty == TRUE){
        swap_outs++;
        page_table[fifo_index].dirty = FALSE;
    }
    swap_ins++;
    
    // Check if we are performing a memwrite
    if(memwrite == TRUE){
        page_table[fifo_index].dirty = TRUE;
    }
    // Update fifo_index
    fifo_index = (fifo_index+1) % size_of_memory;
    return (frame << size_of_frame) | offset;
}

/*
 * Function performs page replacement based on lru algorithm.
 * Frame is moved to the back of queue and it's page number
 * is updated. Dirty and use bits are updated as needed. Returns
 * effective.
 */
long handle_lru(long new_page, long offset, int memwrite)
{
    // Move start frame to back of queue and update it's page number
    swap_to_back(start_of_queue);
    end_of_queue->page_num = new_page;

    // Check if bit was dirty
    if(end_of_queue->dirty == TRUE){
        swap_outs++;
        end_of_queue->dirty = FALSE;
    }
    swap_ins++;

    // Check if we are performing a memwrite
    if(memwrite == TRUE){
        end_of_queue->dirty = TRUE;
    }
    long frame = end_of_queue->frame;
    return (frame << size_of_frame) | offset;
}

/*
 * Function performs page replacement based on clock algorithm.
 * Clock hand will move around page_table until finding a use
 * bit set to FALSE, setting use bits to FALSE along the way. 
 * Clock hand starts at index zero .Dirty and use bits are updated 
 * as needed. Returns effective
 */
long handle_clock(long new_page, long offset, int memwrite){

    // Loop until we find a FALSE use bit
    while(page_table[clock_hand].use==TRUE){
        page_table[clock_hand].use = FALSE;
        clock_hand = (clock_hand+1) % size_of_memory;
    }

    // Update page number
    page_table[clock_hand].page_num = new_page;

    // Check if bit was dirty
    if(page_table[clock_hand].dirty == TRUE){
        swap_outs++;
        page_table[clock_hand].dirty = FALSE;
    }
    swap_ins++;
    
    // Check if we are performing a memwrite
    if(memwrite == TRUE){
        page_table[clock_hand].dirty = TRUE;
    }

    long frame = clock_hand;
    // Update clock hand
    clock_hand = (clock_hand+1) % size_of_memory;
    return (frame << size_of_frame) | offset;
}
/*
 * Super-simple progress bar.
 */
void display_progress(int percent)
{
    int to_date = PROGRESS_BAR_WIDTH * percent / 100;
    static int last_to_date = 0;
    int i;

    if (last_to_date < to_date) {
        last_to_date = to_date;
    } else {
        return;
    }

    printf("Progress [");
    for (i=0; i<to_date; i++) {
        printf(".");
    }
    for (; i<PROGRESS_BAR_WIDTH; i++) {
        printf(" ");
    }
    printf("] %3d%%", percent);
    printf("\r");
    fflush(stdout);
}


int setup()
{
    int i;

    page_table = (struct page_table_entry *)malloc(
        sizeof(struct page_table_entry) * size_of_memory
    );

    if (page_table == NULL) {
        fprintf(stderr,
            "Simulator error: cannot allocate memory for page table.\n");
        exit(1);
    }

    for (i=0; i<size_of_memory; i++) {
        // Initialize values
        page_table[i].page_num = 0;
        page_table[i].use = FALSE;
        page_table[i].dirty = FALSE;
        page_table[i].free = TRUE;
        page_table[i].frame = 0;

        page_table[i].next = NULL;
        page_table[i].prev = NULL;
    }
    return -1;
}


int teardown()
{

    return -1;
}


void error_resolve_address(long a, int l)
{
    fprintf(stderr, "\n");
    fprintf(stderr, 
        "Simulator error: cannot resolve address 0x%lx at line %d\n",
        a, l
    );
    exit(1);
}


int output_report()
{
    printf("\n");
    printf("Memory references: %d\n", mem_refs);
    printf("Page faults: %d\n", page_faults);
    printf("Swap ins: %d\n", swap_ins);
    printf("Swap outs: %d\n", swap_outs);

    return -1;
}


int main(int argc, char **argv)
{
    /* For working with command-line arguments. */
    int i;
    char *s;

    /* For working with input file. */
    FILE *infile = NULL;
    char *infile_name = NULL;
    struct stat infile_stat;
    int  line_num = 0;
    int infile_size = 0;

    /* For processing each individual line in the input file. */
    char buffer[MAX_LINE_LEN];
    long addr;
    char addr_type;
    int  is_write;

    /* For making visible the work being done by the simulator. */
    int show_progress = FALSE;

    /* Process the command-line parameters. Note that the
     * REPLACE_OPTIMAL scheme is not required for A#3.
     */
    for (i=1; i < argc; i++) {
        if (strncmp(argv[i], "--replace=", 9) == 0) {
            s = strstr(argv[i], "=") + 1;
            if (strcmp(s, "fifo") == 0) {
                page_replacement_scheme = REPLACE_FIFO;
            } else if (strcmp(s, "lru") == 0) {
                page_replacement_scheme = REPLACE_LRU;
            } else if (strcmp(s, "clock") == 0) {
                page_replacement_scheme = REPLACE_CLOCK;
            } else if (strcmp(s, "optimal") == 0) {
                page_replacement_scheme = REPLACE_OPTIMAL;
            } else {
                page_replacement_scheme = REPLACE_NONE;
            }
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            infile_name = strstr(argv[i], "=") + 1;
        } else if (strncmp(argv[i], "--framesize=", 12) == 0) {
            s = strstr(argv[i], "=") + 1;
            size_of_frame = atoi(s);
        } else if (strncmp(argv[i], "--numframes=", 12) == 0) {
            s = strstr(argv[i], "=") + 1;
            size_of_memory = atoi(s);
        } else if (strcmp(argv[i], "--progress") == 0) {
            show_progress = TRUE;
        }
    }

    if (infile_name == NULL) {
        infile = stdin;
    } else if (stat(infile_name, &infile_stat) == 0) {
        infile_size = (int)(infile_stat.st_size);
        /* If this fails, infile will be null */
        infile = fopen(infile_name, "r");  
    }


    if (page_replacement_scheme == REPLACE_NONE ||
        size_of_frame <= 0 ||
        size_of_memory <= 0 ||
        infile == NULL)
    {
        fprintf(stderr, 
            "usage: %s --framesize=<m> --numframes=<n>", argv[0]);
        fprintf(stderr, 
            " --replace={fifo|lru|optimal} [--file=<filename>]\n");
        exit(1);
    }


    setup();

    while (fgets(buffer, MAX_LINE_LEN-1, infile)) {
        line_num++;
        if (strstr(buffer, ":")) {
            sscanf(buffer, "%c: %lx", &addr_type, &addr);
            if (addr_type == 'W') {
                is_write = TRUE;
            } else {
                is_write = FALSE;
            }

            if (resolve_address(addr, is_write) == -1) {
                error_resolve_address(addr, line_num);
            }
            mem_refs++;
        } 

        if (show_progress) {
            display_progress(ftell(infile) * 100 / infile_size);
        }
    }
    

    teardown();
    output_report();

    fclose(infile);

    exit(0);
}
