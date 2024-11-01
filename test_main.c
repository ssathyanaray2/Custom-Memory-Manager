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
    void *ptr, *ptro;
    ptr= mem_mngr_alloc(12);
    for(int i=0; i<15; i++){
        mem_mngr_alloc(12);
    }
    mem_mngr_alloc(15);
    ptro = mem_mngr_alloc(100);
    mem_mngr_print_snapshot();

    mem_mngr_free(ptro);
    mem_mngr_print_snapshot();
    mem_mngr_free(ptr);

    mem_mngr_print_snapshot();
    mem_mngr_leave();

    return 0;
}


