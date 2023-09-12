#!/bin/bash

echo "exporting ARM"

export ARCH=aarch64
export CROSS_COMPILE=aarch64-none-elf-
export RISCV=/opt/arm
export PATH=/opt/arm/bin:$PATH
export PLATFORM=zcu102

alias make1='make clean;make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102'

cd bao-baremetal-guest
make1



export CONFIG=baremetal
export CONFIG_BUILTIN=y


alias make2='make clean;make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 CONFIG=baremetal CONFIG_BUILTIN=y'

cd ../bao-hypervisor
make2
cd ..

mkimage -n bao_uboot -A arm64 -O linux -C none -T kernel -a 0x200000\
    -e 0x200000 -d ./bao-hypervisor/bin/zcu102/baremetal/bao.bin ./bao.img
    
echo "fatload mmc 0 0x200000 bao.img; bootm start 0x200000; bootm loados; bootm go"

