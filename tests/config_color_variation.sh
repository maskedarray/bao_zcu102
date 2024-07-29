#!/bin/bash

# Arrays
arr1=("0x1" "0x3" "0x7" "0xf" "0x1f" "0x3f" "0x7f")
arr2=("0xfe" "0xfc" "0xf8" "0xf0" "0xe0" "0xc0" "0x80")
lines=(23 82 121 152)

EXTRA_FLAGS1="-DCUA_RD" 
EXTRA_FLAGS2="-DNCUA_WR" 
HYP_FLAGS=""

for i in {0..6}; do

    sed -i "${lines[0]}s/.colors = [^;]*/.colors = ${arr1[$i]},/" ../bao-hypervisor/configs/baremetal/config.c
    sed -i "${lines[1]}s/.colors = [^;]*/.colors = ${arr2[$i]},/" ../bao-hypervisor/configs/baremetal/config.c
    sed -i "${lines[2]}s/.colors = [^;]*/.colors = ${arr2[$i]},/" ../bao-hypervisor/configs/baremetal/config.c
    sed -i "${lines[3]}s/.colors = [^;]*/.colors = ${arr2[$i]},/" ../bao-hypervisor/configs/baremetal/config.c

    source budget_variation_freertos.sh
    cp ../output.txt ../output_$i.txt
done
