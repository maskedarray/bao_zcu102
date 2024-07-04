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

# Default value for EXTRA_FLAGS
DEFAULT_EXTRA_FLAGS1="-DCUA_RD"

# Check if EXTRA_FLAGS is provided externally, otherwise use the default value
if [ -z "$EXTRA_FLAGS1" ]; then
    EXTRA_FLAGS1=$DEFAULT_EXTRA_FLAGS1
fi
DEFAULT_EXTRA_FLAGS2="-DNCUA_RD"

# Check if EXTRA_FLAGS is provided externally, otherwise use the default value
if [ -z "$EXTRA_FLAGS2" ]; then
    EXTRA_FLAGS2=$DEFAULT_EXTRA_FLAGS2
fi

DEFAULT_HYP_FLAGS=""

# Check if EXTRA_FLAGS is provided externally, otherwise use the default value
if [ -z "$HYP_FLAGS" ]; then
    HYP_FLAGS=$DEFAULT_HYP_FLAGS
fi

rm -rf output/*
cd bao-baremetal-guest
# make1
echo "Extra flags are $EXTRA_FLAGS1"
make clean
make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 NAME=baremetal1 EXTRA_FLAGS="$EXTRA_FLAGS1"
cp build/zcu102/baremetal1.bin ./../output/
cp build/zcu102/baremetal1.elf ./../output/
cp build/zcu102/baremetal1.asm ./../output/
make clean
make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 SINGLE_CORE=y NAME=baremetal2 EXTRA_FLAGS=$EXTRA_FLAGS2 MEM_BASE=0x30000000 NONCUA=y #NONCUA_PRINT=y
cp build/zcu102/baremetal2.bin ./../output/
cp build/zcu102/baremetal2.elf ./../output/
cp build/zcu102/baremetal2.asm ./../output/
make clean
make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 SINGLE_CORE=y NAME=baremetal3 MEM_BASE=0x35000000 NONCUA=y EXTRA_FLAGS=$EXTRA_FLAGS2
cp build/zcu102/baremetal3.bin ./../output/
cp build/zcu102/baremetal3.elf ./../output/
cp build/zcu102/baremetal3.asm ./../output/
make clean
make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 SINGLE_CORE=y NAME=baremetal4 MEM_BASE=0x3A000000 NONCUA=y EXTRA_FLAGS=$EXTRA_FLAGS2
cp build/zcu102/baremetal4.bin ./../output/
cp build/zcu102/baremetal4.elf ./../output/
cp build/zcu102/baremetal4.asm ./../output/

cd ..
export CONFIG=baremetal
export CONFIG_BUILTIN=y


# alias make2='make clean;make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 CONFIG=baremetal CONFIG_BUILTIN=y'

cd bao-hypervisor
# make2
make clean
make CROSS_COMPILE=aarch64-none-elf- PLATFORM=zcu102 CONFIG=baremetal CONFIG_BUILTIN=y EXTRA_FLAGS=$HYP_FLAGS
cp -r ./bin/zcu102/baremetal/bao.elf ./../output/

cd ..

mkimage -n bao_uboot -A arm64 -O linux -C none -T kernel -a 0x200000\
    -e 0x200000 -d ./bao-hypervisor/bin/zcu102/baremetal/bao.bin ./output/bao.img

if [ $? -ne 0 ]; then
    # If mkimage failed, remove the output file
    echo "mkimage encountered an ERROR! Deleting bao.img."
    rm -f ./output/bao.img
else
    echo "mkimage completed successfully. bao.img created."
fi

echo "fatload mmc 0 0x200000 bao.img; bootm start 0x200000; bootm loados; bootm go"

