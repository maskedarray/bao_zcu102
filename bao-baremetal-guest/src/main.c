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

#define MEMGUARD_BUDGET 0xfffffff0
#define MEMGUARD_PERIOD 0x0fffffff

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
            uint64_t data_array[262144];
            uint64_t data_array_end = (uint64_t)&data_array[262144-1];
            asm volatile ("interfering_cores:");
            for (int i = 0; i < 262144; i=i+8){
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
                        "cmp x2, #1\n"               // cmp x2, #<number_of_reps>
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
    
    
    uint64_t data_array[1048576];
    printf("The timer frequency is: %d\n", TIMER_FREQ);
    #ifdef ARRAY_SIZE
    int a_len = 30;
    #else
    for(int a_len = 1; a_len < 32; a_len++) { 
    #endif
        
        uint64_t data_array_end = (uint64_t)&data_array[eval_array[a_len]-1];

        #ifdef CUA_RD
        asm volatile ("non_interfering_core_prime:\n"
                        "mov x2, #0\n"
                    "repeat_loop_prime:\n"
                        "mov x0, %[array]\n"            // Load the base address of the array into register x0
                        "mov x1, %[array_end]\n"        // Load the end address of the array into register x1
                    "loop_start_prime:\n"
                        "ldr x3, [x0], #64\n"            // Load and increment by 64 bytes
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



        int64_t start = timer_get();
  
        // for(int repeat=0; repeat <20; ++repeat){
            #ifdef CUA_RD
            asm volatile ("non_interfering_core:\n"
                            "mov x2, #0\n"
                        "repeat_loop:\n"
                            "mov x0, %[array]\n"            // Load the base address of the array into register x0
                            "mov x1, %[array_end]\n"        // Load the end address of the array into register x1
                        "loop_start:\n"
                            "ldr x3, [x0], #64\n"            // Load and increment by 64 bytes
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

        spin_lock(&print_lock);
        //#warning: divisor should be set in shell print
        printf("Time spent for size: %0.0f , per read/wr is: %0.2f\n", (int)(eval_array[a_len]*8.0)/1024.0, ((float)((end-start)*8)/((float)(100*eval_array[a_len]))));
        // printf("Time spent per iteration for size: %0.1f , is: %ld\n",(eval_array[a_len]*8.0)/1024.0), (float)((end-start)/10.0);
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
