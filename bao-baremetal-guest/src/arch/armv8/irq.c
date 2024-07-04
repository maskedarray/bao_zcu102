#include <irq.h>
#include <cpu.h>
#include <gic.h>
#include <stdio.h>

#ifndef GIC_VERSION
#error "GIC_VERSION not defined for this platform"
#endif

void irq_enable(unsigned id) {
   gic_set_enable(id, true); 
   if(GIC_VERSION == GICV2) {
       gic_set_trgt(id, gic_get_trgt(id) | (1 << get_cpuid()));
   } else {
       gic_set_route(id, get_cpuid());
   }
}

void error_handler(){
        uint64_t esr, elr, far;

    // Read ESR_EL1
    asm volatile ("mrs %0, esr_el1" : "=r" (esr));
    // Read ELR_EL1
    asm volatile ("mrs %0, elr_el1" : "=r" (elr));
    // Read FAR_EL1
    asm volatile ("mrs %0, far_el1" : "=r" (far));

    printf("ESR_EL1: 0x%llx\n", esr);
    printf("ELR_EL1: 0x%llx\n", elr);
    printf("FAR_EL1: 0x%llx\n", far);
    printf("hello\n");
}

void irq_set_prio(unsigned id, unsigned prio){
    gic_set_prio(id, (uint8_t) prio);
}

void irq_send_ipi(unsigned long target_cpu_mask) {
    for(int i = 0; i < sizeof(target_cpu_mask)*8; i++) {
        if(target_cpu_mask & (1ull << i)) {
            gic_send_sgi(i, IPI_IRQ_ID);
        }
    }
}
