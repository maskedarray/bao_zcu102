#!/bin/bash

echo "exporting ARM"

export ARCH=aarch64
export CROSS_COMPILE=/opt/arm/bin/aarch64-none-elf-
export RISCV=/opt/arm
export PATH=/opt/arm/bin:$PATH
export PLATFORM=zcu102

cd buildroot
export BAO_DEMOS_LINUX=/home/a26rahma/work/arm-bao/bao_zcu102/bao-demos/guests/linux
export PLATFORM=zcu102
export BAO_DEMOS_BUILDROOT_DEFCFG=/home/a26rahma/work/arm-bao/bao_zcu102/bao-demos/guests/linux/buildroot/aarch64.config
export BAO_DEMOS_LINUX_CFG_FRAG="/home/a26rahma/work/arm-bao/bao_zcu102/bao-demos/guests/linux/configs/aarch64.config 
/home/a26rahma/work/arm-bao/bao_zcu102/bao-demos/guests/linux/configs/base.config "
export LINUX_OVERRIDE_SRCDIR=/home/a26rahma/work/arm-bao/bao_zcu102/linux
make
dtc ../bao-demos/demos/linux+freertos/devicetrees/zcu102/linux.dts > ../bao-demos/demos/linux+freertos/devicetrees/zcu102/linux.dtb
make -C ../bao-demos/guests/linux/lloader ARCH=aarch64 IMAGE=$(pwd)/output/images/Image DTB=$(pwd)/../bao-demos/demos/linux+freertos/devicetrees/zcu102/linux.dtb TARGET=$(pwd)/../linux_zcu102
cd ..

cd bao-baremetal-guest
make clean
make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 SINGLE_CORE=y NAME=baremetal2 MEM_BASE=0x30000000 NONCUA=y #NONCUA_PRINT=y
cp build/zcu102/baremetal2.bin ./../output/
cp build/zcu102/baremetal2.elf ./../output/
cp build/zcu102/baremetal2.asm ./../output/
make clean
make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 SINGLE_CORE=y NAME=baremetal3 MEM_BASE=0x35000000 NONCUA=y
cp build/zcu102/baremetal3.bin ./../output/
cp build/zcu102/baremetal3.elf ./../output/
cp build/zcu102/baremetal3.asm ./../output/
make clean
make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 SINGLE_CORE=y NAME=baremetal4 MEM_BASE=0x3A000000 NONCUA=y
cp build/zcu102/baremetal4.bin ./../output/
cp build/zcu102/baremetal4.elf ./../output/
cp build/zcu102/baremetal4.asm ./../output/

cd ..
rm bao.img
cd zcu102-zynqmp
rm bao.img
cd ..
export CONFIG=linux
export CONFIG_BUILTIN=y


cd bao-hypervisor
# make2
make clean
make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 CONFIG=linux CONFIG_BUILTIN=y
cp -r ./bin/zcu102/linux/bao.elf ./../

cd ..

mkimage -n bao_uboot -A arm64 -O linux -C none -T kernel -a 0x200000\
    -e 0x200000 -d ./bao-hypervisor/bin/zcu102/linux/bao.bin ./bao.img

if [ $? -ne 0 ]; then
    # If mkimage failed, remove the output file
    echo "mkimage encountered an error. Deleting bao.img."
    rm -f ./bao.img
else
    echo "mkimage completed successfully. bao.img created."
fi
cp -r ./bao.img ./zcu102-zynqmp/
echo "fatload mmc 0 0x200000 bao.img; bootm start 0x200000; bootm loados; bootm go"

