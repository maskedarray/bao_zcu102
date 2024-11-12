#include <config.h>
#include "../../../test_defines.h"

VM_IMAGE(baremetal_image1, XSTR(/home/a26rahma/work/arm-bao/bao_zcu102/output/baremetal1.bin));
VM_IMAGE(baremetal_image2, XSTR(/home/a26rahma/work/arm-bao/bao_zcu102/output/baremetal2.bin));
VM_IMAGE(baremetal_image3, XSTR(/home/a26rahma/work/arm-bao/bao_zcu102/output/baremetal3.bin));
VM_IMAGE(baremetal_image4, XSTR(/home/a26rahma/work/arm-bao/bao_zcu102/output/baremetal4.bin));

struct config config = {
    
    CONFIG_HEADER
    
    .vmlist_size = TOTAL_VMS,
    .vmlist = {
        { 
            .image = {
                .base_addr = BASE_ADDR_CORE0,
                .load_addr = VM_IMAGE_OFFSET(baremetal_image1),
                .size = VM_IMAGE_SIZE(baremetal_image1)
            },

            .entry = BASE_ADDR_CORE0,
            .cpu_affinity = 0x1,
            .colors = COLORS_CORE0,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = BASE_ADDR_CORE0,
                        .size = MEM_SIZE_CORE0
                    }
                },
                #ifdef STATIC_FAT_IMG
                .dev_num = 3,
                #else
                .dev_num = 2,
                #endif
                .devs =  (struct vm_dev_region[]) {
                    {   
                        /* UART0 */
                        .pa = 0xFF000000,
                        .va = 0xFF000000,
                        .size = 0x10000,
                        .interrupt_num = 1,
                        .interrupts = 
                            (irqid_t[]) {53}                         
                    },
                    #ifdef STATIC_FAT_IMG
                    {   
                        .pa = 0x70000000,
                        .va = 0x70000000,
                        .size = 0x3200000                      
                    },
                    #endif
                    {   
                        /* Arch timer interrupt */
                        .interrupt_num = 1,
                        .interrupts = 
                            (irqid_t[]) {27}                         
                    }
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0xF9010000,
                        .gicc_addr = 0xF9020000,
                    }
                }
            },
        },
        { 
            .image = {
                .base_addr = BASE_ADDR_CORE1,
                .load_addr = VM_IMAGE_OFFSET(baremetal_image2),
                .size = VM_IMAGE_SIZE(baremetal_image2)
            },

            .entry = BASE_ADDR_CORE1,
            .cpu_affinity = 0x2,
            .colors = COLORS_CORE1,
            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = BASE_ADDR_CORE1,
                        .size = MEM_SIZE_CORE1
                    }
                },

                .dev_num = 1,
                .devs =  (struct vm_dev_region[]) {
                    {   
                        /* UART0 */
                        .pa = 0xFF010000,
                        .va = 0xFF010000,
                        .size = 0x1000                       
                    }
                },

                .arch = {
                    .gic = {
                        .gicd_addr = 0xF9010000,
                        .gicc_addr = 0xF9020000,
                    }
                }
            },
        },
        { 
            .image = {
                .base_addr = BASE_ADDR_CORE2,
                .load_addr = VM_IMAGE_OFFSET(baremetal_image3),
                .size = VM_IMAGE_SIZE(baremetal_image3)
            },

            .entry = BASE_ADDR_CORE2,
            .cpu_affinity = 0x4,
            .colors = COLORS_CORE2,
            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = BASE_ADDR_CORE2,
                        .size = MEM_SIZE_CORE2
                    }
                },

                

                .arch = {
                    .gic = {
                        .gicd_addr = 0xF9010000,
                        .gicc_addr = 0xF9020000,
                    }
                }
            },
        },
        { 
            .image = {
                .base_addr = BASE_ADDR_CORE3,
                .load_addr = VM_IMAGE_OFFSET(baremetal_image4),
                .size = VM_IMAGE_SIZE(baremetal_image4)
            },

            .entry = BASE_ADDR_CORE3,
            .cpu_affinity = 0x8,
            .colors = COLORS_CORE3,
            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = BASE_ADDR_CORE3,
                        .size = MEM_SIZE_CORE3
                    }
                },

                

                .arch = {
                    .gic = {
                        .gicd_addr = 0xF9010000,
                        .gicc_addr = 0xF9020000,
                    }
                }
            },
        }
    },
};
