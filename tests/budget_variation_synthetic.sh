#!/bin/bash
# This file is different from parent budget_variation.sh because this is customized to run synthetic tests with color variation to generate 3D graph. The loop bounds, starting/ending budgets and increments are different. 


cd ..

# ps | grep -v -e 'bash' -e 'PID' | awk '{print $1}' | xargs kill -9
killall cat;
rm output.txt
(timeout 2000000 stdbuf -oL cat /dev/ttyUSB3 >> output.txt &)
sed -i "s/^#define MEMGUARD_PERIOD .*/#define MEMGUARD_PERIOD 2000/" ./bao-hypervisor/src/core/inc/pmu_v1.h;
BUDGET=4294967195
MAX=4294967295
MAX_BW=665
BUDGET_CUA=0
BUDGET_NCUA=0
BUDGET_CUA_T=0

# Set a default value if EXTRA_FLAGS2 is not provided
EXTRA_FLAGS1=${EXTRA_FLAGS1:-"-DCUA_RD -DARRAY_SIZE"}

# Now you can use EXTRA_FLAGS2 in your script as needed
echo "For CUA: $EXTRA_FLAGS1"
# Set a default value if EXTRA_FLAGS2 is not provided
EXTRA_FLAGS2=${EXTRA_FLAGS2:-"-DNCUA_WR"}

# Now you can use EXTRA_FLAGS2 in your script as needed
echo "For NCUA: $EXTRA_FLAGS2"
echo "For HYP: $HYP_FLAGS"

for ((trial=0; trial<10; trial++)); do
  # EXTRA_FLAGS1="-DCUA_RD -DARRAY_SIZE"
  # EXTRA_FLAGS2="-DNCUA_RD"
  ((BUDGET_CUA = BUDGET))
  ((BUDGET_CUA_T = MAX - BUDGET))
  ((BUDGET_NCUA = MAX_BW - BUDGET_CUA_T))
  ((BUDGET_NCUA = BUDGET_NCUA/3))
  echo "Budgets = $BUDGET_NCUA , $BUDGET_CUA_T"
  echo "Budgets = $BUDGET_NCUA , $BUDGET_CUA_T" >> output.txt
  ((BUDGET_NCUA = MAX - BUDGET_NCUA))


  if ((BUDGET_NCUA < 20)); then
    exit 1
  fi

  sed -i "s/^#define MEMGUARD_BUDGET_CUA .*/#define MEMGUARD_BUDGET_CUA $BUDGET_CUA/" ./bao-hypervisor/src/core/inc/pmu_v1.h
  sed -i "s/^#define MEMGUARD_BUDGET_NCUA .*/#define MEMGUARD_BUDGET_NCUA $BUDGET_NCUA/" ./bao-hypervisor/src/core/inc/pmu_v1.h

  rm -rf output
  mkdir output
  source setup.sh > /dev/null
  cd zcu102-zynqmp/ || exit 1

  xsct semi_boot_script.tcl
  cd ..

  sleep 1
  echo ";bootm start 0x200000; bootm loados; bootm go" > /dev/ttyUSB3
  sleep 20

  ((BUDGET -= 50))
  
done

cd tests


#cat output.txt | grep "per read" | cut -d " " -f10 > output2.txt