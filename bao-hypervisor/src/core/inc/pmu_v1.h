#ifndef __PMU_V1_H__
#define __PMU_V1_H__

#include <bao.h>

#define PMU_V1_INTR_SRC1 142
#define PMU_V1_INTR_ID1 (PMU_V1_INTR_SRC1 + 1)

#define MEMGUARD_BUDGET 1294967285
#define MEMGUARD_BUDGET_CUA 4294965795
#define MEMGUARD_BUDGET_NCUA 4294967145
#define MEMGUARD_PERIOD 1400

typedef struct {
    uint32_t counters[8];
    uint32_t event_sel[4];
    uint32_t event_info[4];
    uint32_t init_budget[8];
    uint32_t period[2];
    uint32_t timer[2];
} __attribute__((__packed__, aligned(PAGE_SIZE))) pmu_v1_global_t;

// extern volatile pmu_v1_global_t pmu_v1_global
//     __attribute__((section(".devices")));




void pmu_v1_interrupt_handler();
void pmu_v1_run();



void pmu_v1_init();

#endif