/** 
 * Bao, a Lightweight Static Partitioning Hypervisor 
 *
 * Copyright (c) Bao Project (www.bao-project.org), 2019-
 *
 * Authors:
 *      Jose Martins <jose.martins@bao-project.org>
 *      Sandro Pinto <sandro.pinto@bao-project.org>
 *
 * Bao is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License version 2 as published by the Free
 * Software Foundation, with a special exception exempting guest code from such
 * license. See the COPYING file in the top-level directory for details. 
 *
 */

#include <core.h>
#include <stdlib.h>
#include <stdio.h>
#include <cpu.h>
#include <wfi.h>
#include <spinlock.h>
#include <plat.h>
#include <irq.h>
#include <uart.h>
#include <timer.h>
#include <arch/timer.h>

#define TIMER_INTERVAL (TIME_S(1))

spinlock_t print_lock = SPINLOCK_INITVAL;


#define POINTER_CHASING

void uart_rx_handler(){
    printf("cpu%d: %s\n",get_cpuid(), __func__);
    uart_clear_rxirq();
}

void ipi_handler(){
    printf("cpu%d: %s\n", get_cpuid(), __func__);
    irq_send_ipi(1ull << (get_cpuid() + 1));
}

void timer_handler(){
    printf("cpu%d: %s\n", get_cpuid(), __func__);
    timer_set(TIMER_INTERVAL);
    irq_send_ipi(1ull << (get_cpuid() + 1));
}


int eval_array[40] = {512, 1024, 2048, 4096, 5120, 6144, 7168, 8192, 9216, 10240, 11264, 12288, 13312, 14336, 15360, 16384, 20480, 24576, 28672, 32768, 36864, 40960, 45056, 49152, 53248, 57344, 61440, 65536, 131072, 262144, 524288, 1048576};

void shuffle(uint64_t *array, size_t n) {
    // Create an array of indices that will represent the 8th element positions.
    int num_elems = n / 8;
    int *indices = malloc(num_elems * sizeof(int));
    
    for (int i = 0; i < num_elems; ++i) {
        indices[i] = i * 8;
    }
    
    // Shuffle the indices using Fisher-Yates shuffle algorithm.
    srand(1);
    for (int i = num_elems - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        // Swap indices[i] and indices[j]
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }
    
    // Create a cycle using the shuffled indices.
    for (int i = 0; i < num_elems - 1; ++i) {
        array[indices[i]] = (uint64_t)&array[indices[i + 1]];
    }
    // Close the cycle
    array[indices[num_elems - 1]] = (uint64_t)&array[indices[0]];

    // Free the allocated memory for indices
    free(indices);
}
void main(void){

    static volatile bool master_done = false;

    #ifdef NONCUA
        #ifdef NONCUA_PRINT
            printf("This is non cua core\n");
        #endif
        // spin_lock(&print_lock);
        // printf("cpu %d up\n", get_cpuid());
        // spin_unlock(&print_lock);
        while(1){
            uint64_t data_array[1048576];
            uint64_t data_array_end = (uint64_t)&data_array[1048576-1];
            asm volatile ("interfering_cores:");
            for (int i = 0; i < 1048576; i=i+8){
                asm volatile ("non_interfering_core_prime:\n"
                        "mov x2, #0\n"
                    "repeat_loop_prime:\n"
                        "mov x0, %[array]\n"            // Load the base address of the array into register x0
                        "mov x1, %[array_end]\n"        // Load the end address of the array into register x1
                    "loop_start_prime:\n"
                        #ifdef NCUA_RD
                        "ldr x3, [x0], #64\n"            // Load and increment by 64 bytes
                        #endif
                        #ifdef NCUA_WR
                        "str x2, [x0], #64\n"
                        #endif
                        "cmp x0, x1\n"                  // Compare the current address with the end address
                        "b.lt loop_start_prime\n"             // Branch back to the start of the loop if less than
                        "add x2, x2, #1\n"
                        "cmp x2, #1000\n"               // cmp x2, #<number_of_reps>
                        "b.lt repeat_loop_prime\n"
                        : : [array] "r" (data_array), [array_end] "r" (data_array_end)
                        : "cc", "x0", "x1", "x2", "x3" 
                );
                // asm volatile (""
                //     #ifdef NCUA_RD
                //     "ldr %[value], [%[array], %[index], LSL #3]\n"
                //     #endif
                //     #ifdef NCUA_WR
                //     "str %[value], [%[array], %[index], LSL #3]\n"
                //     #endif
                //     : [value] "+r" (data_array[i])
                //     : [array] "r" (data_array), [index] "r" (i)
                //     : // No clobbered registers
                // );
            }
        }
        while(1){}

    #else
    
    
    if(cpu_is_master()){
        spin_lock(&print_lock);
        printf("Bao bare-metal test guest\n");
        spin_unlock(&print_lock);
        // uart_enable_rxirq();
        master_done = true;
    }
    while(!master_done);
 
    spin_lock(&print_lock);
    printf("cpu %d up\n", get_cpuid());
    spin_unlock(&print_lock);
    
    uint32_t miss_count =0;
    // asm volatile("MRS %0, PMEVCNTR0_EL0" : "=r"(miss_count));
    // printf("Cache miss counter %llu\r\n", miss_count);
    uint64_t data_array[1048576];
    #ifdef POINTER_CHASING
    for(int i=0; i<1048576; i+=8){
        data_array[i] = (uint64_t)&data_array[i];
    }
    shuffle(data_array, 1048576);
    #endif
    printf("The timer frequency is: %d\n", TIMER_FREQ);
    #ifdef ARRAY_SIZE
    int a_len = 30;     //eval_array[30] = 524288 = 65536 cache lines = 4194304 bytes
    #else
    for(int a_len = 1; a_len < 32; a_len++) { 
    #endif
        
        #ifdef POINTER_CHASING
        uint64_t data_array_end = 524288/8;     // will cause this number o 65536 misses.
        #else
        uint64_t data_array_end = (uint64_t)&data_array[eval_array[a_len]-1];
        #endif


        #ifdef CUA_RD
        asm volatile ("non_interfering_core_prime:\n"
                        "mov x2, #0\n"
                    "repeat_loop_prime:\n"
                        "mov x0, %[array]\n"            // Load the base address of the array into register x0
                        "mov x1, %[array_end]\n"        // Load the end address of the array into register x1
                        #ifdef POINTER_CHASING
                        "ldr x3, [x0], #64\n"            // Load and increment by 64 bytes
                        "mov x0, #1\n"
                        #endif
                    "loop_start_prime:\n"
                        #ifdef POINTER_CHASING
                        "ldr x3, [x3]\n"            // Load and increment by 64 bytes
                        "add x0, x0, #1\n"
                        #else
                        "ldr x3, [x0], #64\n"
                        #endif
                        "cmp x0, x1\n"                  // Compare the current address with the end address
                        "b.lt loop_start_prime\n"             // Branch back to the start of the loop if less than
                        "add x2, x2, #1\n"
                        "cmp x2, #1\n"               // cmp x2, #<number_of_reps>
                        "b.lt repeat_loop_prime\n"
                        : : [array] "r" (data_array), [array_end] "r" (data_array_end)
                        : "cc", "x0", "x1", "x2", "x3" 
        );
        #endif
        #ifdef CUA_WR
        asm volatile ("non_interfering_core_prime:\n"
                        "mov x2, #0\n"
                    "repeat_loop_prime:\n"
                        "mov x0, %[array]\n"            // Load the base address of the array into register x0
                        "mov x1, %[array_end]\n"        // Load the end address of the array into register x1
                    "loop_start_prime:\n"
                        "str x2, [x0], #64\n"            // Load data_array[i] into register x3 and increment the base address by 64 bytes
                        "cmp x0, x1\n"                  // Compare the current address with the end address
                        "b.lt loop_start_prime\n"             // Branch back to the start of the loop if less than
                        "add x2, x2, #1\n"
                        "cmp x2, #1\n"               // cmp x2, #<number_of_reps>
                        "b.lt repeat_loop_prime\n"
                        : : [array] "r" (data_array), [array_end] "r" (data_array_end)
                        : "cc", "x0", "x1", "x2", "x3" 
        );
        #endif

        // asm volatile("MRS %0, PMEVCNTR0_EL0" : "=r"(miss_count));
        // printf("Cache miss counter %llu\r\n", miss_count);
        int64_t start = timer_get();
  
        // for(int repeat=0; repeat <2; ++repeat){
            #ifdef CUA_RD
            asm volatile ("non_interfering_core:\n"
                            "mov x2, #0\n"
                        "repeat_loop:\n"
                            "mov x0, %[array]\n"            // Load the base address of the array into register x0
                            "mov x1, %[array_end]\n"        // Load the end address of the array into register x1
                            #ifdef POINTER_CHASING
                            "ldr x3, [x0], #64\n"            // Load and increment by 64 bytes
                            "mov x0, #1\n"
                            #endif
                        "loop_start:\n"
                            #ifdef POINTER_CHASING
                            "ldr x3, [x3]\n"            // Load and increment by 64 bytes
                            "add x0, x0, #1\n"
                            #else
                            "ldr x3, [x0], #64\n"
                            #endif
                            "cmp x0, x1\n"                  // Compare the current address with the end address
                            "b.lt loop_start\n"             // Branch back to the start of the loop if less than
                            "add x2, x2, #1\n"
                            "cmp x2, #100\n"               // cmp x2, #<number_of_reps>
                            "b.lt repeat_loop\n"
                            : : [array] "r" (data_array), [array_end] "r" (data_array_end)
                            : "cc", "x0", "x1", "x2", "x3" 
            );
            #endif
            #ifdef CUA_WR
            asm volatile ("non_interfering_core:\n"
                            "mov x2, #0\n"
                        "repeat_loop:\n"
                            "mov x0, %[array]\n"            // Load the base address of the array into register x0
                            "mov x1, %[array_end]\n"        // Load the end address of the array into register x1
                        "loop_start:\n"
                            "str x2, [x0], #64\n"            // Load data_array[i] into register x3 and increment the base address by 64 bytes
                            "cmp x0, x1\n"                  // Compare the current address with the end address
                            "b.lt loop_start\n"             // Branch back to the start of the loop if less than
                            "add x2, x2, #1\n"
                            "cmp x2, #100\n"               // cmp x2, #<number_of_reps>
                            "b.lt repeat_loop\n"
                            : : [array] "r" (data_array), [array_end] "r" (data_array_end)
                            : "cc", "x0", "x1", "x2", "x3" 
            );
            #endif
        // }
        
        int64_t end = timer_get();
        // *12: core clock is 1200mhz and counter clock is 100mhz
        // /8: eval_array[a_len] is skipped by 8 indices in assembly loop to make jump of 64 byte equal to cache line
        // divide result by Num_Reps to offset number of repetations
        // float time_per_rd = ((float)(end-start)*12)/((float)eval_array[a_len]/8);
        // time_per_rd = time_per_rd/NUM_REPS;
        spin_lock(&print_lock);
        //#warning: divisor should be set in shell print
        printf("Time spent for size: %0.0f , per read/wr @100mhz is: %0.2f\n", (int)(eval_array[a_len]*8.0)/1024.0, ((float)((end-start)*8)/((float)(100*eval_array[a_len]))));
        // printf("Time spent per iteration for size: %0.1f , is: %ld\n",(eval_array[a_len]*8.0)/1024.0), (float)((end-start)/10.0);
        // This is the total execution time instead of the time per instruction.
        printf("Time taken %llu\r\n", ((float)(end-start)));
        
        asm volatile("MRS %0, PMEVCNTR0_EL0" : "=r"(miss_count));
        printf("Cache miss counter %llu\r\n", miss_count);
        spin_unlock(&print_lock);
    #ifndef ARRAY_SIZE
    }
    #endif
    spin_lock(&print_lock);
     #ifdef CUA_RD
    printf("CUA set to read\n");
    #endif
    #ifdef CUA_WR 
    printf("CUA set to write\n");
    #endif
    spin_unlock(&print_lock);
    
    timer_wait(100000000);
    while(1) {};
    #endif
}
