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

void main(void){

    static volatile bool master_done = false;

    #ifdef NONCUA
        #ifdef NONCUA_PRINT
            printf("This is non cua core\n");
        #endif
        while(1){
            uint64_t data_array[262144];
            asm volatile ("interfering_cores:");
            for (int i = 0; i < 262144; i=i+8){
                asm volatile (""
                    #ifdef NCUA_RD
                    "ldr %[value], [%[array], %[index], LSL #3]\n"   // Load data_array[i] into x1
                    #endif
                    #ifdef NCUA_WR
                    "str %[value], [%[array], %[index], LSL #3]\n"   // Store the result back to data_array[i]
                    #endif
                    : [value] "+r" (data_array[i])
                    : [array] "r" (data_array), [index] "r" (i)
                    : // No clobbered registers
                );
            }
        }
        while(1){}

    #else
    
    
    if(cpu_is_master()){
        spin_lock(&print_lock);
        printf("Bao bare-metal test guest\n");

        spin_unlock(&print_lock);

        // irq_set_handler(UART_IRQ_ID, uart_rx_handler);
        // irq_set_handler(TIMER_IRQ_ID, timer_handler);
        // irq_set_handler(IPI_IRQ_ID, ipi_handler);

        uart_enable_rxirq();

        // timer_set(TIMER_INTERVAL);
        // irq_enable(TIMER_IRQ_ID);
        // irq_set_prio(TIMER_IRQ_ID, IRQ_MAX_PRIO);

        master_done = true;
    }

    // irq_enable(UART_IRQ_ID);
    // irq_set_prio(UART_IRQ_ID, IRQ_MAX_PRIO);
    // irq_enable(IPI_IRQ_ID);
    // irq_set_prio(IPI_IRQ_ID, IRQ_MAX_PRIO);
    
    while(!master_done);
    
    spin_lock(&print_lock);
    printf("cpu %d up\n", get_cpuid());
    spin_unlock(&print_lock);
    int eval_array[40] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 5120, 6144, 7168, 8192, 9216, 10240, 11264, 12288, 13312, 14336, 15360, 16384, 20480, 24576, 28672, 32768, 36864, 40960, 45056, 49152, 53248, 57344, 61440, 65536, 131072, 262144, 524288, 1048576};

    uint64_t data_array[262144];


    for(int a_len = 1; a_len < 36; a_len++) { 
        for (int i = 0; i < eval_array[a_len]; i=i+8){
            asm volatile (
                    #ifdef CUA_RD
                    "ldr %[value], [%[array], %[index], LSL #3]\n"   // Load data_array[i] into x1
                    #endif
                    #ifdef CUA_WR 
                    "str %[value], [%[array], %[index], LSL #3]\n"   // Store the result back to data_array[i]
                    #endif
                : [value] "+r" (data_array[i])
                : [array] "r" (data_array), [index] "r" (i)
                : // No clobbered registers
            );
        }

        int64_t start = timer_get();

        for(int repeat=0; repeat <100; ++repeat){
            asm volatile ("non_interfering_core:");
            for (int i = 0; i < eval_array[a_len]; i=i+8){
                asm volatile (
                    #ifdef CUA_RD
                    "ldr %[value], [%[array], %[index], LSL #3]\n"   // Load data_array[i] into x1
                    #endif
                    #ifdef CUA_WR 
                    "str %[value], [%[array], %[index], LSL #3]\n"   // Store the result back to data_array[i]
                    #endif
                    : [value] "+r" (data_array[i])
                    : [array] "r" (data_array), [index] "r" (i)
                    : // No clobbered registers
                );
            }
        }
        int64_t end = timer_get();

        spin_lock(&print_lock);
        #ifdef CUA_RD
        printf("CUA set to read");
        #endif
        #ifdef CUA_WR 
        printf("CUA set to write");
        #endif
        printf("Time spent by cpu %d per read/wr is: %0.1f\n", get_cpuid(), ((float)((end-start)*8)/((float)(100*eval_array[a_len]))));
        printf("Time spent by cpu %d per iteration is: %ld\n", get_cpuid(), (int)((end-start)/100));
        spin_unlock(&print_lock);
    }
    
    
    

    while(1) wfi();
    #endif
}
