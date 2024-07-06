#include <timer.h>
#include <arch/timer.h>
#define arm___
#ifdef arm___
#define magic_timing_begin(cycleLo, cycleHi) {\
    uint64_t cntvct; \
    asm volatile("mrs %0, cntvct_el0" : "=r"(cntvct)); \
    cycleLo = (uint32_t)(cntvct & 0xFFFFFFFF); \
    cycleHi = (uint32_t)(cntvct >> 32); \
}
 
#define magic_timing_end(cycleLo, cycleHi) {\
    uint64_t tempCntvct; \
    asm volatile("mrs %0, cntvct_el0" : "=r"(tempCntvct)); \
    uint32_t tempCycleLo = (uint32_t)(tempCntvct & 0xFFFFFFFF); \
    uint32_t tempCycleHi = (uint32_t)(tempCntvct >> 32); \
    cycleLo = tempCycleLo - cycleLo; \
    cycleHi = tempCycleHi - cycleHi; \
}
 
 
#define magic_timing_report(cycleLo, cycleHi) {\
    printf("Timing report: %d %d\n", cycleLo, cycleHi); \
}\
 
#else
#define magic_timing_begin(cycleLo, cycleHi) {\
     asm volatile( "rdtsc": "=a" (cycleLo), "=d" (cycleHi)); \
    /*    asm volatile ("rdcycle %0" : "=r" (cycleLo));*/ \
}\
 
#define magic_timing_end(cycleLo, cycleHi) {\
    unsigned tempCycleLo, tempCycleHi; \
    asm volatile( "rdtsc": "=a" (tempCycleLo), "=d" (tempCycleHi)); \
    /*    asm volatile ("rdcycle %0" : "=r" (tempcycleLo)); */\
    cycleLo = tempCycleLo-cycleLo;\
    cycleHi = tempCycleHi - cycleHi;\
}\
 
 
#define magic_timing_report(cycleLo, cycleHi) {\
    printf("Timing report: %d %d\n", cycleLo, cycleHi); \
}\
 
#endif

#ifdef METRO
 
#define magic_timing_begin(cycleLo, cycleHi) {\
    asm volatile( "mfsr $8, CYCLE_LO\n\t"  \
        "mfsr $9, CYCLE_HI\n\t"  \
        "addu %0, $8, $0\n\t"  \
        "addu %1, $9, $0\n\t"  \
        :"=r" (cycleLo), "=r" (cycleHi) \
        : \
        :"$8", "$9"\
    );\
}

#define magic_timing_end(cycleLo, cycleHi) {\
    asm volatile( \
        "mfsr $8, CYCLE_LO\n\t"  \
        "mfsr $9, CYCLE_HI\n\t" \
        "subu %0, $8, %0\n\t" \
        "subu %1, $9, %1\n\t" \
        :"=r" (cycleLo), "=r" (cycleHi) \
        : \
        :"$8", "$9"\
    ); \
}

#define magic_timing_report(cycleLo, cycleHi) {\
    asm volatile( "addu $8, %0, $0\n\t" \
        "mtsr PASS $8\n\t" \
        "mtsr PASS $9\n\t" \
        : \
        :"r" (cycleLo), "r" (cycleHi) \
        : "$8", "$9" \
    );\
}

//#define metro_magic_timing_report(cycleLo, cycleHi) {\
//    asm volatile( "nop\n\t");\
//}

#endif

#ifdef BTL

#include "/u/kvs/raw/rawlib/archlib/include/raw.h"

#define magic_timing_begin(cycleLo, cycleHi) {\
    raw_magic_timing_report_begin();\
}

#define magic_timing_end(cycleLo, cycleHi) {\
    raw_magic_timing_report_end(); \
}

#define magic_timing_report(cycleLo, cycleHi) {\
    raw_magic_timing_report_print(); \
}

    
//
//void metro_magic_timing_begin(int cycleLo, int cycleHi)
//{
//    raw_magic_timing_report_begin();
//}
//
//void metro_magic_timing_end(int cycleLo, int cycleHi)
//{
//    raw_magic_timing_report_end();
//}
//
//void metro_magic_timing_report(int cycleLo, int cycleHi)
//{
//    raw_magic_timing_report_print();
//    return;
//}

#endif
