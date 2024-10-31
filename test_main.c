/*
 * CS 551 Project "Memory manager".
 * You do not need to turn in this file.	
 */

#include "memory_manager.h"
#include "interposition.h"

int main(int argc, char * argv[])
{
    mem_mngr_init();

    mem_mngr_print_snapshot();
    for(int i=0; i<8; i++){
        mem_mngr_alloc(12);
    }
    void *ptr = mem_mngr_alloc(28);
    mem_mngr_alloc(16);
    mem_mngr_print_snapshot();

    mem_mngr_free(ptr);
    mem_mngr_print_snapshot();
    mem_mngr_free(ptr);

    mem_mngr_leave();

    return 0;
}


