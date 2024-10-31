/*
 * CS 551 Project "Memory manager".
 * This file needs to be turned in.	
 */


#include "memory_manager.h"

static STRU_MEM_LIST * mem_pool = NULL;

/*
 * Print out the current status of the memory manager.
 * Reading this function may help you understand how the memory manager organizes the memory.
 * Do not change the implementation of this function. It will be used to help the grading.
 */
void mem_mngr_print_snapshot(void)
{
    STRU_MEM_LIST * mem_list = NULL;

    printf("============== Memory snapshot ===============\n");

    mem_list = mem_pool; // Get the first memory list
    while(NULL != mem_list)
    {
        STRU_MEM_BATCH * mem_batch = mem_list->first_batch; // Get the first mem batch from the list 

        printf("mem_list %p slot_size %d batch_count %d free_slot_bitmap %p\n", 
                   mem_list, mem_list->slot_size, mem_list->batch_count, mem_list->free_slots_bitmap);
        bitmap_print_bitmap(mem_list->free_slots_bitmap, mem_list->bitmap_size);

        while (NULL != mem_batch)
        {
            printf("\t mem_batch %p batch_mem %p\n", mem_batch, mem_batch->batch_mem);
            mem_batch = mem_batch->next_batch; // get next mem batch
        }

        mem_list = mem_list->next_list;
    }

    printf("==============================================\n");
}

/*
 * Initialize the memory manager with 16 bytes(defined by the macro MEM_ALIGNMENT_BOUNDARY) slot size mem_list.
 * Initialize this list with 1 batch of slots.
 */
void mem_mngr_init(void)
{
    mem_pool = (STRU_MEM_LIST *)malloc(sizeof(STRU_MEM_LIST));
    if (mem_pool == NULL) {
        fprintf(stderr, "Failed to allocate memory for the memory manager structure.\n");
        return; 
    }

   mem_pool->slot_size = SLOT_ALLINED_SIZE(16);
   mem_pool->batch_count = 1;
   mem_pool->bitmap_size = MEM_BATCH_SLOT_COUNT/8;
   mem_pool->free_slots_bitmap = (unsigned char *)calloc(mem_pool->bitmap_size, sizeof(unsigned char));

   if (mem_pool->free_slots_bitmap == NULL) {
        fprintf(stderr, "Failed to allocate memory for the free slots bitmap.\n");
        free(mem_pool); 
        return; 
    }

    STRU_MEM_BATCH *new_batch = (STRU_MEM_BATCH *)malloc(sizeof(STRU_MEM_BATCH));
    if (new_batch == NULL) {
        fprintf(stderr, "Failed to allocate memory for the first batch.\n");
        free(mem_pool->free_slots_bitmap); 
        free(mem_pool); 
        return; 
    }

    new_batch->batch_mem = malloc((MEM_BATCH_SLOT_COUNT/8) * mem_pool->slot_size);
    if (new_batch->batch_mem == NULL) {
        fprintf(stderr, "Failed to allocate memory for the batch memory.\n");
        free(new_batch); 
        free(mem_pool->free_slots_bitmap); 
        free(mem_pool); 
        return; 
    }

    new_batch->next_batch = NULL;
    mem_pool->first_batch = new_batch;
    mem_pool->next_list = NULL;

    // for (int i = 0; i < MEM_BATCH_SLOT_COUNT; i++) {
    //     bitmap_set_bit(mem_pool->free_slots_bitmap, mem_pool->bitmap_size, i);
    // }
}

/*
 * Clean up the memory manager (e.g., release all the memory allocated)
 */
void mem_mngr_leave(void)
{
    STRU_MEM_LIST *current_list = mem_pool;

    while (current_list) {
        STRU_MEM_LIST *next_list = current_list->next_list;

        STRU_MEM_BATCH *current_batch = current_list->first_batch;
        while (current_batch) {
            STRU_MEM_BATCH *next_batch = current_batch->next_batch;
            free(current_batch->batch_mem); 
            free(current_batch); 
            current_batch = next_batch;
        }

        free(current_list->free_slots_bitmap);
        free(current_list);
        current_list = next_list;
    }

    mem_pool = NULL;
}

/*
 * Allocate a chunk of memory 	
 * @param size: size in bytes to be allocated
 * @return: the pointer to the allocated memory slot
 */
void * mem_mngr_alloc(size_t size)
{
	STRU_MEM_LIST *current_list = mem_pool;
    STRU_MEM_LIST *prev_list = mem_pool;

    while (current_list) {
        if (current_list->slot_size == SLOT_ALLINED_SIZE(size)) break;
        prev_list = current_list;
        current_list = current_list->next_list;
    }

    if (!current_list) {
        current_list = (STRU_MEM_LIST *)malloc(sizeof(STRU_MEM_LIST));
        if (!current_list) {
            fprintf(stderr, "Failed to allocate memory for a new batch.\n");
            return NULL; 
        }

        current_list->slot_size = SLOT_ALLINED_SIZE(size);
        current_list->batch_count = 1;
        current_list->bitmap_size = (MEM_BATCH_SLOT_COUNT/8);
        current_list->free_slots_bitmap = (unsigned char *)calloc(current_list->bitmap_size, sizeof(unsigned char));


        if (current_list->free_slots_bitmap == NULL) {
            fprintf(stderr, "Failed to allocate memory for the free slots bitmap.\n");
            free(current_list); 
            return NULL; 
        }

        STRU_MEM_BATCH *new_batch = (STRU_MEM_BATCH *)malloc(sizeof(STRU_MEM_BATCH));
        if (new_batch == NULL) {
            fprintf(stderr, "Failed to allocate memory for the first batch.\n");
            free(current_list->free_slots_bitmap); 
            free(current_list); 
            return NULL; 
        }

        new_batch->batch_mem = malloc((MEM_BATCH_SLOT_COUNT/8) * current_list->slot_size);
        if (new_batch->batch_mem == NULL) {
            fprintf(stderr, "Failed to allocate memory for the batch memory.\n");
            free(new_batch); 
            free(current_list->free_slots_bitmap); 
            free(current_list); 
            return NULL; 
        }

        new_batch->next_batch = NULL;
        current_list->first_batch = new_batch;
        prev_list->next_list = current_list;

        // for (int i = 0; i < (MEM_BATCH_SLOT_COUNT/8); i++) {
        //     bitmap_set_bit(current_list->free_slots_bitmap, current_list->bitmap_size, i);
        // }

        bitmap_set_bit(current_list->free_slots_bitmap, current_list->bitmap_size, 0);

        void *allocated_memory = (void *)((unsigned char *)current_list->first_batch->batch_mem);
        return allocated_memory;
 
    }

    else{

        STRU_MEM_BATCH *batch = current_list->first_batch;
        STRU_MEM_BATCH *prev_batch = current_list->first_batch;
        int free_slot_pos = -1;


        while (batch) {
                free_slot_pos = bitmap_find_first_bit(current_list->free_slots_bitmap, current_list->bitmap_size, 0);
                if (free_slot_pos == -2) {
                    fprintf(stderr, "bitmap operator error.\n");
                    return NULL;
                }
                else if (free_slot_pos != -1){
                    break;
                }
                prev_batch = batch;
                batch = batch->next_batch;
        }

        // printf("current_list %d\n", free_slot_pos);
        // bitmap_print_bitmap(current_list->free_slots_bitmap, current_list->bitmap_size);

        if (free_slot_pos == -1){

            batch = (STRU_MEM_BATCH *)malloc(sizeof(STRU_MEM_BATCH));
            if(batch==NULL){
                fprintf(stderr, "Failed to allocate memory for the free slots.\n");
                return NULL;
            }

            batch->batch_mem = malloc(SLOT_ALLINED_SIZE(size) * (MEM_BATCH_SLOT_COUNT/8));
            if (!batch->batch_mem) {
                free(batch);
                return NULL; 
            }

            prev_batch->next_batch = batch;
            batch->next_batch = NULL;
            // batch->next_batch = current_list->first_batch;
            // current_list->first_batch = batch;
            current_list->batch_count++;

            int new_bitmap_size = current_list->batch_count * (MEM_BATCH_SLOT_COUNT/8);
            unsigned char *new_bitmap = (unsigned char *)realloc(current_list->free_slots_bitmap, new_bitmap_size);

            // memset(new_bitmap + current_list->bitmap_size, 0, new_bitmap_size - current_list->bitmap_size);
            for (int i = current_list->bitmap_size; i < new_bitmap_size; i++) {
                bitmap_set_bit(new_bitmap, new_bitmap_size, i);
            }
            current_list->free_slots_bitmap = new_bitmap;
            current_list->bitmap_size = new_bitmap_size;


            free_slot_pos = bitmap_find_first_bit(current_list->free_slots_bitmap, current_list->bitmap_size, 0);

        }

        bitmap_set_bit(current_list->free_slots_bitmap, current_list->bitmap_size, free_slot_pos);
        void *allocated_memory = (void *)((unsigned char *)current_list->first_batch->batch_mem + free_slot_pos * SLOT_ALLINED_SIZE(size));
        return allocated_memory;
    }


}

/*
 * Free a chunk of memory pointed by ptr
 * Print out any error messages
 * @param: the pointer to the allocated memory slot
 */
void mem_mngr_free(void * ptr)
{
	if (!ptr) {
        printf("Error: NULL pointer provided.\n");
        return;
    }

    STRU_MEM_LIST *current_list = mem_pool;

    while(current_list) {

        STRU_MEM_BATCH *batch = current_list->first_batch;

        while (batch) {
            
            if (ptr >= batch->batch_mem && ptr < (void *)((unsigned char *)batch->batch_mem + current_list->slot_size * (MEM_BATCH_SLOT_COUNT/8))) {
                size_t offset = (unsigned char *)ptr - (unsigned char *)batch->batch_mem;
                
                if (offset % current_list->slot_size != 0) {
                    printf("Error: Pointer is not aligned to the start of any slot.\n");
                    return;
                }


                int slot_index = offset / current_list->slot_size;

                if (bitmap_bit_is_set(current_list->free_slots_bitmap, current_list->bitmap_size, slot_index) == 0) {
                    printf("Error: Double free detected.\n");
                    return;
                }

                bitmap_clear_bit(current_list->free_slots_bitmap, current_list->bitmap_size, slot_index);
                return;
            }

            batch = batch->next_batch;
        }

        current_list = current_list->next_list;

    }

    printf("Error: Pointer is outside of managed memory.\n");

}