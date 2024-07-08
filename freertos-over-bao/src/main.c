/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "ff_headers.h"
#include "ff_stdio.h"
#include "ff_ramdisk.h"
#include "ff_sys.h"

#include <uart.h>
#include <irq.h>
#include <plat.h>
#include <timer.h>
#include <arch/timer.h>

#include "disparity_top.h"
#include "mser_top.h"
#include "pca_top.h"
#include "stitch_top.h"
#include "svm_top.h"
#include "localization_top.h"
#include "texture_synthesis_top.h"
#include "multi_ncut_top.h"
#include "tracking_top.h"


/*
 * Prototypes for the standard FreeRTOS callback/hook functions implemented
 * within this file.  See https://www.freertos.org/a00016.html
 */
void vApplicationMallocFailedHook(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
void vApplicationTickHook(void);

/*-----------------------------------------------------------*/

void vTask(void *pvParameters)
{
    unsigned long counter = 0;
    unsigned long id = (unsigned long)pvParameters;
    while (1)
    {
        printf("Task%d: %d\n", id, counter++);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void uart_rx_handler(){
    printf("%s\n", __func__);
    uart_clear_rxirq();
}

void DIRCommand( const char *pcDirectoryToScan )
{
    FF_FindData_t *pxFindStruct;
    const char  *pcAttrib,
                *pcWritableFile = "writable file",
                *pcReadOnlyFile = "read only file",
                *pcDirectory = "directory";

	printf("\r\n\n<===========================>\r\n");
    pxFindStruct = ( FF_FindData_t * ) pvPortMalloc( sizeof( FF_FindData_t ) );
    memset( pxFindStruct, 0x00, sizeof( FF_FindData_t ) );
    if( ff_findfirst( pcDirectoryToScan, pxFindStruct ) == 0 ) {
        do {
            if( ( pxFindStruct->ucAttributes & FF_FAT_ATTR_DIR ) != 0 ) {
                pcAttrib = pcDirectory;
            }
            else if( pxFindStruct->ucAttributes & FF_FAT_ATTR_READONLY ) {
                pcAttrib = pcReadOnlyFile;
            }
            else {
                pcAttrib = pcWritableFile;
            }
            printf("%s [%s] [size=%d] \r\n", pxFindStruct->pcFileName,
                                                  pcAttrib,
                                                  pxFindStruct->ulFileSize );
        } while( ff_findnext( pxFindStruct ) == 0 );
    }

	printf("<============================>\r\n\n");

    vPortFree( pxFindStruct );
}

#define mainRAM_DISK_SECTOR_SIZE     512UL                                                    /* Currently fixed! */
#define mainIO_MANAGER_CACHE_SIZE    ( 15UL * mainRAM_DISK_SECTOR_SIZE )
#define mainRAM_DISK_NAME            "/ram"

extern uint8_t my_fat_image_start;
extern uint8_t my_fat_image_end;
extern const int my_fat_image_size;

int main(void){

    printf("Bao FreeRTOS guest\n");

    FF_Disk_t * pxDisk;
	FF_FILE *pxSourceFile, *pxDestinationFile;

    /* Create the RAM disk. */
    #ifdef STATIC_FAT_IMG
    pxDisk = FF_RAMDiskInit2( mainRAM_DISK_NAME, 0x70000000, 102400, mainIO_MANAGER_CACHE_SIZE );
	#else
    pxDisk = FF_RAMDiskInit2( mainRAM_DISK_NAME, &my_fat_image_start, 102400, mainIO_MANAGER_CACHE_SIZE );
    #endif
    // FF_RAMDiskShowPartition(pxDisk);
	// DIRCommand("/ram/");

    // DIRCommand("/ram/disp");

    uint64_t time_start, time_end;

    char* arguments[] = {"script_disparity", "/ram/disp"};
    time_start = timer_get();
    sdvb_disparity(2, arguments);
    time_end = timer_get();
    printf("Time Taken (sdvb_disparity): %llu\n\n", time_end - time_start);

    char* arguments2[] = {"script_disparity", "/ram/mser"};
    time_start = timer_get();
    sdvb_mser(2, arguments2);
    time_end = timer_get();
    printf("Time Taken (sdvb_mser): %llu\n\n", time_end - time_start);

    // DIRCommand("/ram/svm");
    // char* arguments_svm[] = {"script_disparity", "/ram/svm"};
    // time_start = timer_get();
    // sdvb_svm(2, arguments_svm);
    // time_end = timer_get();
    // printf("Time Taken (sdvb_svm): %llu\n\n", time_end - time_start);

    // DIRCommand("/ram/loca");
    char* arguments_localization[] = {"script_disparity", "/ram/loca"};
    time_start = timer_get();
    sdvb_localization(2, arguments_localization);
    time_end = timer_get();
    printf("Time Taken (sdvb_localization): %llu\n\n", time_end - time_start);

    char* arguments_texture_synthesis[] = {"script_disparity", "/ram/text"};
    // time_start = timer_get();
    // sdvb_texture_synthesis(2, arguments_texture_synthesis);
    // time_end = timer_get();
    // printf("Time Taken (sdvb_texture_synthesis): %llu\n\n", time_end - time_start);

    // Not in image
    // DIRCommand("/ram/multi");
    // char* arguments_multi_ncut[] = {"script_disparity", "/ram/multi"};
    // time_start = timer_get();
    // sdvb_multi_ncut(2, arguments_multi_ncut);
    // time_end = timer_get();
    // printf("Time Taken (sdvb_multi_ncut): %llu\n\n", time_end - time_start);

    // Not in image
    // char* arguments_tracking[] = {"script_disparity", "/ram/tracking"};
    // time_start = timer_get();
    // sdvb_tracking(2, arguments_tracking);
    // time_end = timer_get();
    // printf("Time Taken (sdvb_tracking): %llu\n\n", time_end - time_start);


    
    // Too many prints
    // char* arguments3[] = {"script_disparity", "/ram/pca/sat.trn", "4435", "37", "R"};
    // time_start = timer_get();
	// sdvb_pca(5, arguments3);
    // time_end = timer_get();
    // printf("Time Taken: %llu\n\n", time_end - time_start);




    uart_enable_rxirq();
    irq_set_handler(UART_IRQ_ID, uart_rx_handler);
    irq_set_prio(UART_IRQ_ID, IRQ_MAX_PRIO);
    irq_enable(UART_IRQ_ID);

    // xTaskCreate(
    //     vTask,
    //     "Task1",
    //     configMINIMAL_STACK_SIZE,
    //     (void *)1,
    //     tskIDLE_PRIORITY + 1,
    //     NULL);

    // xTaskCreate(
    //     vTask,
    //     "Task2",
    //     configMINIMAL_STACK_SIZE,
    //     (void *)2,
    //     tskIDLE_PRIORITY + 1,
    //     NULL);

    vTaskStartScheduler();
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{
    /* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();
    for (;;)
        ;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)pxTask;

    /* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for (;;)
        ;
}
/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
}
/*-----------------------------------------------------------*/

void vAssertCalled(void)
{
    volatile uint32_t ulSetTo1ToExitFunction = 0;

    taskDISABLE_INTERRUPTS();
    while (ulSetTo1ToExitFunction != 1)
    {
        __asm volatile("NOP");
    }
}
/*-----------------------------------------------------------*/

/* This version of vApplicationAssert() is declared as a weak symbol to allow it
to be overridden by a version implemented within the application that is using
this BSP. */
void vApplicationAssert( const char *pcFileName, uint32_t ulLine )
{
volatile uint32_t ul = 0;
volatile const char *pcLocalFileName = pcFileName; /* To prevent pcFileName being optimized away. */
volatile uint32_t ulLocalLine = ulLine; /* To prevent ulLine being optimized away. */

	/* Prevent compile warnings about the following two variables being set but
	not referenced.  They are intended for viewing in the debugger. */
	( void ) pcLocalFileName;
	( void ) ulLocalLine;

	printf( "Assert failed in file %s, line %lu\r\n", pcLocalFileName, ulLocalLine );

	/* If this function is entered then a call to configASSERT() failed in the
	FreeRTOS code because of a fatal error.  The pcFileName and ulLine
	parameters hold the file name and line number in that file of the assert
	that failed.  Additionally, if using the debugger, the function call stack
	can be viewed to find which line failed its configASSERT() test.  Finally,
	the debugger can be used to set ul to a non-zero value, then step out of
	this function to find where the assert function was entered. */
	taskENTER_CRITICAL();
	{
		while( ul == 0 )
		{
			__asm volatile( "NOP" );
		}
	}
	taskEXIT_CRITICAL();
}
