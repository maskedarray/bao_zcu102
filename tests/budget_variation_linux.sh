#!/bin/bash
# DIR=../$(pwd)
# cd $DIR; ps | grep -v -e 'bash' -e 'PID' | awk '{print $1}' | xargs kill -9 ;(timeout 1000 stdbuf -oL cat /dev/ttyUSB2 > output.txt &); PERIOD=4
# 294967255; for((trial=0;trial<15;trial++)); do EXTRA_FLAGS1="-DCUA_RD -DARRAY_SIZE"; EXTRA_FLAGS2=-DNCUA_RD; echo $PERIOD; sed -i "s/^#define MEMGUARD_BUDGET .*/#define MEMGUARD_BUDGET 
# $PERIOD/" ./bao-hypervisor/src/core/inc/pmu_v1.h;source setup.sh>/dev/null;cd zcu102-zynqmp/; xsct semi_boot_script.tcl; cd ..; sleep 1; echo ";bootm start 0x200000; bootm loados; bootm
#  go" > /dev/ttyUSB2; sleep 3; if ((trial>4)); then ((PERIOD -= 20)); else ((PERIOD -= 10)); fi; done; 

# cat output.txt | grep "per read" | cut -d " " -f5,10 > output2.txt


cd ..

# ps | grep -v -e 'bash' -e 'PID' | awk '{print $1}' | xargs kill -9
SCRIPT_USB_DEVICE="/dev/ttyUSB5"
killall cat;
rm output.txt
(timeout 20000000 stdbuf -oL cat $SCRIPT_USB_DEVICE >> output.txt &)

NUM_VMS=4
NCUA_TEST_TYPE="NCUA_WR"

BUDGET=4294967195
MAX=4294967295
MAX_BW=665
SCRIPT_CUA_BUDGET=0
SCRIPT_NCUA_BUDGET=0
BUDGET_CUA_T=0


SCRIPT_MEMGUARD_PERIOD=2000

echo ""> ./test_defines.h
sed -i '1s/^.*$/#ifndef __TEST_DEFINES_H__\n#define __TEST_DEFINES_H__\n\n#endif/' ./test_defines.h

#############################################################################################
sed -i "/#endif/i #define TOTAL_VMS $NUM_VMS" ./test_defines.h

sed -i '/#endif/i #define BASE_ADDR_CORE0 0x20000000' ./test_defines.h
sed -i '/#endif/i #define MEM_SIZE_CORE0 0x4000000' ./test_defines.h
sed -i '/#endif/i #define COLORS_CORE0 0x03' ./test_defines.h

sed -i '/#endif/i #define BASE_ADDR_CORE1 0x30000000' ./test_defines.h
sed -i '/#endif/i #define MEM_SIZE_CORE1 0x4000000' ./test_defines.h
sed -i '/#endif/i #define COLORS_CORE1 0x0C' ./test_defines.h

sed -i '/#endif/i #define BASE_ADDR_CORE2 0x35000000' ./test_defines.h
sed -i '/#endif/i #define MEM_SIZE_CORE2 0x4000000' ./test_defines.h
sed -i '/#endif/i #define COLORS_CORE2 0x30' ./test_defines.h

sed -i '/#endif/i #define BASE_ADDR_CORE3 0x3A000000' ./test_defines.h
sed -i '/#endif/i #define MEM_SIZE_CORE3 0x4000000' ./test_defines.h
sed -i '/#endif/i #define COLORS_CORE3 0xC0' ./test_defines.h
#############################################################################################

sed -i '/#endif/i #define MEMGUARD_BUDGET_CUA '"$SCRIPT_CUA_BUDGET" ./test_defines.h
sed -i '/#endif/i #define MEMGUARD_BUDGET_NCUA '"$SCRIPT_NCUA_BUDGET" ./test_defines.h
sed -i '/#endif/i #define MEMGUARD_PERIOD '"$SCRIPT_MEMGUARD_PERIOD" ./test_defines.h

sed -i "/#endif/i #define $NCUA_TEST_TYPE" ./test_defines.h




for ((trial=0; trial<10; trial++)); do
  # EXTRA_FLAGS1="-DCUA_RD -DARRAY_SIZE"
  # EXTRA_FLAGS2="-DNCUA_RD"
  ((SCRIPT_CUA_BUDGET = BUDGET))
  ((BUDGET_CUA_T = MAX - BUDGET))
  ((SCRIPT_NCUA_BUDGET = MAX_BW - BUDGET_CUA_T))
  ((SCRIPT_NCUA_BUDGET = SCRIPT_NCUA_BUDGET/3))
  echo "Budgets = $SCRIPT_NCUA_BUDGET , $BUDGET_CUA_T"
  echo "Budgets = $SCRIPT_NCUA_BUDGET , $BUDGET_CUA_T" >> output.txt
  ((SCRIPT_NCUA_BUDGET = MAX - SCRIPT_NCUA_BUDGET))


  if ((SCRIPT_NCUA_BUDGET < 20)); then
    exit 1
  fi

  sed -i 's/^#define MEMGUARD_BUDGET_CUA .*/#define MEMGUARD_BUDGET_CUA '"$SCRIPT_CUA_BUDGET"'/' ./test_defines.h

  sed -i 's/^#define MEMGUARD_BUDGET_NCUA .*/#define MEMGUARD_BUDGET_NCUA '"$SCRIPT_NCUA_BUDGET"'/' ./test_defines.h

  rm -rf output
  mkdir output
  source setup_linux.sh > /dev/null
  cd zcu102-zynqmp/ || exit 1

  xsct semi_boot_script.tcl
  cd ..

  sleep 1
  echo ";bootm start 0x200000; bootm loados; bootm go" > $SCRIPT_USB_DEVICE
  sleep 120

  ((BUDGET -= 50))
  
done

cd tests


#cat output.txt | grep "per read" | cut -d " " -f10 > output2.txt