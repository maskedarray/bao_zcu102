#include <config.h>

VM_IMAGE(baremetal_image1, XSTR(/home/a26rahma/work/arm-bao/bao_zcu102/baremetal1.bin));
VM_IMAGE(baremetal_image2, XSTR(/home/a26rahma/work/arm-bao/bao_zcu102/baremetal2.bin));

struct config config = {
    
    CONFIG_HEADER
    
    .vmlist_size = 2,
    .vmlist = {
        { 
            .image = {
                .base_addr = 0x20000000,
                .load_addr = VM_IMAGE_OFFSET(baremetal_image1),
                .size = VM_IMAGE_SIZE(baremetal_image1)
            },

            .entry = 0x20000000,
            .cpu_affinity = 0x1,
            .colors = 0x0,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x20000000,
                        .size = 0x4000000
                    }
                },

                .dev_num = 1,
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
                .base_addr = 0x30000000,
                .load_addr = VM_IMAGE_OFFSET(baremetal_image2),
                .size = VM_IMAGE_SIZE(baremetal_image2)
            },

            .entry = 0x30000000,
            .cpu_affinity = 0x2,

            .platform = {
                .cpu_num = 1,
                
                .region_num = 1,
                .regions =  (struct vm_mem_region[]) {
                    {
                        .base = 0x30000000,
                        .size = 0x4000000
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
        }
    },
};
