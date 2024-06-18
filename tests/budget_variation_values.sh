#!/bin/bash
# DIR=../$(pwd)
# cd $DIR; ps | grep -v -e 'bash' -e 'PID' | awk '{print $1}' | xargs kill -9 ;(timeout 1000 stdbuf -oL cat /dev/ttyUSB2 > output.txt &); PERIOD=4
# 294967255; for((trial=0;trial<15;trial++)); do EXTRA_FLAGS1="-DCUA_RD -DARRAY_SIZE"; EXTRA_FLAGS2=-DNCUA_RD; echo $PERIOD; sed -i "s/^#define MEMGUARD_BUDGET .*/#define MEMGUARD_BUDGET 
# $PERIOD/" ./bao-hypervisor/src/core/inc/pmu_v1.h;source setup.sh>/dev/null;cd zcu102-zynqmp/; xsct semi_boot_script.tcl; cd ..; sleep 1; echo ";bootm start 0x200000; bootm loados; bootm
#  go" > /dev/ttyUSB2; sleep 3; if ((trial>4)); then ((PERIOD -= 20)); else ((PERIOD -= 10)); fi; done; 

# cat output.txt | grep "per read" | cut -d " " -f5,10 > output2.txt




# ps | grep -v -e 'bash' -e 'PID' | awk '{print $1}' | xargs kill -9

BUDGET=4294967255
MAX=4294967295
MAX_BW=1950
BUDGET_CUA=0
BUDGET_NCUA=0


for ((trial=0; trial<40; trial++)); do
  EXTRA_FLAGS1="-DCUA_RD -DARRAY_SIZE"
  EXTRA_FLAGS2="-DNCUA_RD"
  ((BUDGET_CUA = BUDGET))
  ((BUDGET_NCUA = MAX - BUDGET))
  ((BUDGET_NCUA = MAX_BW - BUDGET_NCUA))
  ((BUDGET_NCUA = BUDGET_NCUA/3))
  DIFFERENCE=$((MAX - BUDGET))
  echo "Budgets = $BUDGET_NCUA , $DIFFERENCE"
  ((BUDGET_NCUA = MAX - BUDGET_NCUA))
#   echo "Budgets = $BUDGET_NCUA , $BUDGET_CUA"

  if ((BUDGET_NCUA < 20)); then
    exit 1
  fi


  if ((trial > 4)); then
    ((BUDGET -= 40))
  else
    ((BUDGET -= 20))
  fi
done




#cat output.txt | grep "per read" | cut -d " " -f5,10 > output2.txt