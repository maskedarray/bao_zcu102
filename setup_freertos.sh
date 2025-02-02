#!/bin/bash

echo "exporting ARM"

export ARCH=aarch64
export CROSS_COMPILE=aarch64-none-elf-
export RISCV=/opt/arm
export PATH=/opt/arm/bin:$PATH
export PLATFORM=zcu102

alias supersetup='source setup.sh > /dev/null; cd zcu102-zynqmp/; xsct xsct_script.tcl; cd .. ; (timeout 10 cat /dev/ttyUSB3 &) ;sleep 1; echo ";bootm start 0x200000; bootm loados; bootm go" > /dev/ttyUSB3; echo "echoed data to uart"'
#flags1=("-DCUA_RD" "-DCUA_WR" "-DCUA_RD" "-DCUA_WR" );flags2=("-DNCUA_RD" "-DNCUA_RD" "-DNCUA_WR" "-DNCUA_WR" );killall cat;(timeout 120 cat /dev/ttyUSB1  &) && for((trial=0;trial<4;trial++)); do EXTRA_FLAGS1=${flags1[trial]}; EXTRA_FLAGS2=${flags2[trial]}; source setup.sh>/dev/null;cd zcu102-zynqmp/; xsct semi_boot_script.tcl; cd ..; sleep 1; echo ";bootm start 0x200000; bootm loados; bootm go" > /dev/ttyUSB1; sleep 3; done

#flags1=("-DCUA_RD" "-DCUA_WR" "-DCUA_RD" "-DCUA_WR" );flags2=("-DNCUA_RD" "-DNCUA_RD" "-DNCUA_WR" "-DNCUA_WR" );ps | grep -v -e 'bash' -e 'PID' | awk '{print $1}' | xargs kill -9 ;(timeout 120 stdbuf -oL cat /dev/ttyUSB0  > output.txt &); for((trial=0;trial<4;trial++)); do EXTRA_FLAGS1=${flags1[trial]}; EXTRA_FLAGS2=${flags2[trial]}; source setup.sh>/dev/null;cd zcu102-zynqmp/; xsct semi_boot_script.tcl; cd ..; sleep 1; echo ";bootm start 0x200000; bootm loados; bootm go" > /dev/ttyUSB0; sleep 3; done
#cat output.txt | grep "per read" | cut -d " " -f5,10 | awk '{print (NR < 32? "RD_RD": (NR < 63? "WR_RD": (NR<94? "RD_WR" : "WR_WR"))), $0}' > output2.txt

# alias make1='make clean;make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102'

cd freertos-over-bao
# make1

# No need to clean everytime
make clean
make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 NAME=baremetal1 BMARK=disparity
cp build/zcu102/baremetal1.bin ./../output/
cp build/zcu102/baremetal1.elf ./../output/
cp build/zcu102/baremetal1.asm ./../output/

cd ../bao-baremetal-guest
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
export CONFIG=baremetal
export CONFIG_BUILTIN=y


# alias make2='make clean;make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 CONFIG=baremetal CONFIG_BUILTIN=y'

cd bao-hypervisor
# make2
make clean
make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 CONFIG=baremetal CONFIG_BUILTIN=y
cp -r ./bin/zcu102/baremetal/bao.elf ./../

cd ..

mkimage -n bao_uboot -A arm64 -O linux -C none -T kernel -a 0x200000\
    -e 0x200000 -d ./bao-hypervisor/bin/zcu102/baremetal/bao.bin ./bao.img

if [ $? -ne 0 ]; then
    # If mkimage failed, remove the output file
    echo "mkimage encountered an error. Deleting bao.img."
    rm -f ./bao.img
else
    echo "mkimage completed successfully. bao.img created."
fi
cp -r ./bao.img ./zcu102-zynqmp/
echo "fatload mmc 0 0x200000 bao.img; bootm start 0x200000; bootm loados; bootm go"

