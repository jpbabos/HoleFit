#include <stdio.h>
#include <stdlib.h>
// declare global variables including linked list of blocks containing id, starting address, ending address, link to next node in list
struct node {
    int id;
    int start_addr;
    int end_addr;
    struct node *link;
} *block_list = NULL;

typedef struct node block_type;

//global variables
int pm_size, remaining;

/********************************************************************/
void enter_params(){
    printf("Enter size of physical memory: ");
    scanf("%d", &pm_size);

    remaining = pm_size;

    // create "dummy" block with id -1, staring/ending address of 0 and link of NULL
    block_list = (block_type *)malloc(sizeof(block_type));
    block_list->id = -1;
    block_list->start_addr = 0;
    block_list->end_addr = 0;
    block_list->link = NULL;
}

/********************************************************************/
void print_table(){
    block_type *current_block;
    current_block = block_list;

    //header of table
    printf("ID \t Start \t End\n");

    while(current_block != NULL){

        //contents of table
        printf("%d\t%d\t%d\n", current_block->id,current_block->start_addr, current_block->end_addr);
        current_block = current_block->link;
    }
}

/********************************************************************/
void first_fit() {
    int id, block_size, hole_start, hole_end, hole_size;
    block_type *new_block, *current_block;

    printf("Enter block ID: ");
    scanf("%d", &id);
    printf("Enter block size: ");
    scanf("%d", &block_size);

    // if block size is larger than remaining memory, print message, return
    if (block_size > remaining) {
        printf("Block size is larger than remaining memory");
        return;
    }

    // if block list is "empty", allocate new block, set fields for new block, link to block list, reduce remaining memory, print allocation table, return
    if (block_list->link == NULL) {
        new_block = (block_type *) malloc(sizeof(block_type));
        new_block->id = id;
        new_block->start_addr = block_list->end_addr;
        new_block->end_addr = new_block->start_addr + block_size;
        new_block->link = NULL;
        block_list->link = new_block;
        remaining = remaining - block_size;
        print_table();
        return;
    }

    // initialize "current block" to compare hole
    current_block = block_list;
    while (current_block != NULL) {
        if (current_block->id == id) {
            printf("Duplicate found\n");
            return;
        }
        current_block = current_block->link;
    }

    // initialize "current block" to compare hole
    current_block = block_list;
    while (current_block != NULL){
        hole_start = current_block->end_addr;
        if(current_block->link != NULL){
            hole_end = current_block->link->start_addr;
        }
        else{
            hole_end = pm_size;
        }

        hole_size = hole_end - hole_start;
        // if block fits in hole, allocate new block, set fields for new block, link to block list, reduce remaining memory, print allocation table, return
        if(block_size <= hole_size){
            new_block = (block_type *)malloc(sizeof(block_type));
            new_block->id = id;
            new_block->start_addr = hole_start;
            new_block->end_addr = new_block->start_addr + block_size;
            new_block->link = current_block->link;
            current_block->link = new_block;
            remaining = remaining - block_size;
            print_table();
            return;
        }
        current_block = current_block->link;
    }
    // if end of list reached, print message no fitting hole
    if(current_block == NULL){
        printf("There is no fitting hole");
    }
}

/********************************************************************/
void best_fit(){
    int at_least_one = 0;
    int best_so_far = pm_size;
    int best_start;
    int block_size;
    int hole_size;
    int id;
    int hole_start;
    int hole_end;
    block_type *best_block;
    block_type *new_block, *current_block;

    printf("Enter block ID: ");
    scanf("%d", &id);
    printf("Enter block size: ");
    scanf("%d", &block_size);

    if(block_size > remaining){
        printf("Block size is larger than remaining memory");
        return;
    }

    if(block_list->link == NULL){
        new_block = (block_type *)malloc(sizeof(block_type));
        new_block->id = id;
        new_block->start_addr = block_list->end_addr;
        new_block->end_addr = new_block->start_addr + block_size;
        new_block->link = NULL;
        block_list->link = new_block;
        remaining = remaining - block_size;
        print_table();
        return;
    }

    current_block = block_list;
    while(current_block != NULL){
        if(current_block->id == id){
            printf("Duplicate found");
            return;
        }
        current_block = current_block->link;
    }

    current_block = block_list;
    while (current_block != NULL){
        hole_start = current_block->end_addr;
        if(current_block->link != NULL){
            hole_end = current_block->link->start_addr;
        }
        else{
            hole_end = pm_size;
        }
        hole_size = hole_end - hole_start;
        if(block_size <= hole_size) {
            at_least_one = 1; //set flag "at least one" fitting hole found
            if(hole_size < best_so_far){
                // set new value for "best so far", "best start", copy "best block" to current block
                best_so_far = hole_size;
                best_start = hole_start;
                best_block = current_block;
            }
        }
        current_block = current_block->link;
    }

    if(at_least_one == 0){
        printf("There was no hole large enough");
        return;
    }

    new_block = (block_type *)malloc(sizeof(block_type));
    new_block->id = id;
    new_block->start_addr = best_start;
    new_block->end_addr = new_block->start_addr + block_size;
    new_block->link = best_block->link;
    best_block->link = new_block;
    remaining = remaining - block_size;
    print_table();
}

/********************************************************************/
void deallocate(){
    block_type *current_block;
    block_type *previous_block;
    int id;

    printf("Enter block ID: ");
    scanf("%d", &id);
    current_block = block_list;

    // while id not found and end of block list not reached
    while((current_block != NULL) && (current_block->id != id)){
        previous_block = current_block;
        current_block = current_block->link;
    }

    // if "current block" is NULL, print message id not found
    if(current_block == NULL){
        printf("ID not found");
    }

    // else remove block, adjusting "previous block" pointer around it, increment remaining memory with block size, free block with matching id
    previous_block->link = current_block->link;
    remaining += current_block->end_addr - current_block->start_addr;
    free(current_block);
    print_table();
}

/********************************************************************/
void defragment(){
    int block_size;
    block_type *current_block;
    int previous_end = 0;

    current_block = block_list;
    while(current_block != NULL){
        // adjust start and end fields of each block, compacting together
        block_size = current_block->end_addr - current_block->start_addr;
        current_block->start_addr = previous_end;
        current_block->end_addr = current_block->start_addr + block_size;
        previous_end = current_block->end_addr;
        current_block = current_block->link;
    }
    print_table();
}

/********************************************************************/
void quit(block_type *node){
    if(node == NULL){
        return;
    }
    else{
        quit(node->link);
        free(node);
    }
}

/********************************************************************/
int main(){
    int choice = 0;

    while(choice != 6){
        printf("*********************************************************\n");
        printf("Welcome to Hole fit!\n\n");
        printf("Please choose one of the options below:\n");
        printf("1. Enter Parameters\n");
        printf("2. Allocate memory for a block using first-fit\n");
        printf("3. Allocate memory for a block using best-fit\n");
        printf("4. Deallocate memory for a block\n");
        printf("5. Deframgent memory\n");
        printf("6. Quit\n");

        printf("Selection:");
        scanf("%d", &choice);
        switch(choice){
            case 1:
                enter_params();
                break;
            case 2:
                first_fit();
                break;
            case 3:
                best_fit();
                break;
            case 4:
                deallocate();
                break;
            case 5:
                defragment();
                break;

        }
    }
    quit(block_list);
    printf("Have a great day!");
}