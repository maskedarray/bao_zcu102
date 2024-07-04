
#include <bao.h>
#include <pmu_v1.h>

#include <platform.h>
#include <cpu.h>
#include <mem.h>
#include <fences.h>
#include <spinlock.h>
#include <printk.h>


#define read_32b(addr)         (*(volatile uint32_t *)(long)(addr))
#define write_32b(addr, val_)  (*(volatile uint32_t *)(long)(addr) = val_)




void read_32b_regs(uint32_t num_regs, uint64_t base_addr) {
  uint32_t val;
  for (uint32_t i=0; i<num_regs; i++) {
    val = read_32b(base_addr);
    printk("Read: %x: %x\n", base_addr, val);
    
    base_addr += 4;
  }
}

void write_32b_regs(uint32_t num_regs, uint64_t base_addr, uint32_t val[]) {
  for (uint32_t i=0; i<num_regs; i++) {
    write_32b(base_addr, val[i]);
    printk("Write: %x: %x\n", base_addr, val[i]);
    
    base_addr += 4;
  }
}

void read_64b_regs(uint32_t num_regs, uint64_t base_addr) {
  uint32_t val_l, val_h;
  uint64_t val;
  for (uint32_t i=0; i<num_regs; i++) {
    val_l = read_32b(base_addr);
    val_h = read_32b(base_addr+4);
    val = (uint32_t)((uint64_t)val_h << 32) | val_l;
    printk("Read: %x: %x\n", base_addr, val);
    
    base_addr += 8;
  }
}

void write_64b_regs(uint32_t num_regs, uint64_t base_addr, uint64_t val[]) {
  uint32_t val_l, val_h;
  for (uint32_t i=0; i<num_regs; i++) {
    val_l = val[i] & 0xFFFFFFFF;
    val_h = val[i] >> 32;
    write_32b(base_addr, val_l);
    write_32b(base_addr+4, val_h);
    printk("Write: %x: %x\n", base_addr, val[i]);
    
    base_addr += 8;
  }
} 


void pmu_v1_init(){

    uint64_t pmcr;
    asm volatile("MRS %0, PMCR_EL0" : "=r"(pmcr));

    // Set the "Enable" bit (bit 0)
    pmcr |= 0x1;

    // Write the modified value back to PMCR_EL0
    asm volatile("MSR PMCR_EL0, %0" :: "r"(pmcr));

    // Configure PMXEVTYPER to count L1 data cache misses (event code 0x03)
    // uint32_t event_type = 0x03;  // Event code for L1 data cache misses
    uint32_t event_type = 0x17;  // Event code for L2 Refill

    asm volatile("MSR PMEVTYPER0_EL0, %0" :: "r"(event_type));

    //Set initial budget of counter 0
    uint32_t value;
    // value = MEMGUARD_BUDGET;
    if(cpu()->id == 0){
      value = MEMGUARD_BUDGET_CUA;
    } else
      value = MEMGUARD_BUDGET_NCUA;
    asm volatile("MSR PMEVCNTR0_EL0, %0" :: "r"(value));

    //enable overflow interrupt
    value =  (1ULL);  // Set bit 31 for PMCCNTR overflow interrupt
    asm volatile("MSR PMINTENSET_EL1, %0" :: "r"(value));

    #if defined(PERIOD_VARIATION_NO_PERIOD)
    // This is added to enable the cache miss counter even when pmu_run function is not called.
     value = 1;  // Set bit 0 to enable PMCCNTR enabling counter 0
    asm volatile("MSR PMCNTENSET_EL0, %0" :: "r"(value));
    #endif
    // volatile uint32_t comp_array[100] = {0};
    // for (uint32_t i=0; i<100; i++) {
    //     comp_array[i] = comp_array[i] + i;
    // }
    // printk("Array traversed!\n");
    // asm volatile("MRS %0, PMCCNTR_EL0" : "=r"(pmcr));
    // printk("value after array: %x\n", pmcr);
    

}

void pmu_v1_run(){

    uint64_t pmcr = MEMGUARD_PERIOD;
    asm volatile("MSR CNTP_TVAL_EL0, %0" :: "r"(pmcr));

    pmcr = 0x1;
    asm volatile("MSR CNTP_CTL_EL0, %0" :: "r"(pmcr));

    uint32_t value = 1;  // Set bit 0 to enable PMCCNTR enabling counter 0
    asm volatile("MSR PMCNTENSET_EL0, %0" :: "r"(value));


  
    // volatile uint32_t comp_array[100] = {0};
    // for (uint32_t i=0; i<100; i++) {
    //     comp_array[i] = comp_array[i] + i;
    // }
    // printk("Array traversed!\n");
    // asm volatile("MRS %0, PMCCNTR_EL0" : "=r"(pmcr));
    // printk("value after array: %x\n", pmcr);

}

void pmu_v1_interrupt_handler(){

  uint32_t value = 1;  // Clear overflow interrupt flag
	asm volatile("MSR PMOVSCLR_EL0, %0" :: "r"(value));
  // reload counter  -----------------stop the core --------------------------
  // value = MEMGUARD_BUDGET;
  if(cpu()->id == 0){
    value = MEMGUARD_BUDGET_CUA;
  } else
    value = MEMGUARD_BUDGET_NCUA;
  asm volatile("MSR PMEVCNTR0_EL0, %0" :: "r"(value));

  uint64_t init_val, final_val;
  asm volatile("MRS %0, CNTPCT_EL0" : "=r"(init_val));
  asm volatile("MRS %0, CNTP_CVAL_EL0" : "=r"(final_val));
  // printk("pmu %lu, %lu\n", init_val, final_val);
  printk("pmu interrupt\n" );
  while(final_val > init_val){
    
    asm volatile("MRS %0, CNTPCT_EL0" : "=r"(init_val));
    asm volatile("MRS %0, CNTP_CVAL_EL0" : "=r"(final_val));
    
    // printk("Hyp Interrupt occurred2 %lu, %lu\n", init_val, final_val);
    }
    
}






