#!/bin/bash
# DIR=../$(pwd)
# cd $DIR; ps | grep -v -e 'bash' -e 'PID' | awk '{print $1}' | xargs kill -9 ;(timeout 1000 stdbuf -oL cat /dev/ttyUSB2 > output.txt &); PERIOD=4
# 294967255; for((trial=0;trial<15;trial++)); do EXTRA_FLAGS1="-DCUA_RD -DARRAY_SIZE"; EXTRA_FLAGS2=-DNCUA_RD; echo $PERIOD; sed -i "s/^#define MEMGUARD_BUDGET .*/#define MEMGUARD_BUDGET 
# $PERIOD/" ./bao-hypervisor/src/core/inc/pmu_v1.h;source setup.sh>/dev/null;cd zcu102-zynqmp/; xsct semi_boot_script.tcl; cd ..; sleep 1; echo ";bootm start 0x200000; bootm loados; bootm
#  go" > /dev/ttyUSB2; sleep 3; if ((trial>4)); then ((PERIOD -= 20)); else ((PERIOD -= 10)); fi; done; 

# cat output.txt | grep "per read" | cut -d " " -f5,10 > output2.txt


cd ..

# ps | grep -v -e 'bash' -e 'PID' | awk '{print $1}' | xargs kill -9
killall cat;
rm output.txt
(timeout 200000 stdbuf -oL cat /dev/ttyUSB2 >> output.txt &)

# Set a default value if EXTRA_FLAGS2 is not provided
EXTRA_FLAGS1=${EXTRA_FLAGS1:-"-DCUA_RD"}

# Now you can use EXTRA_FLAGS2 in your script as needed
echo "For CUA: $EXTRA_FLAGS1"
# Set a default value if EXTRA_FLAGS2 is not provided
EXTRA_FLAGS2=${EXTRA_FLAGS2:-"-DNCUA_WR"}

# Now you can use EXTRA_FLAGS2 in your script as needed
echo "For NCUA: $EXTRA_FLAGS2"

for ((trial=0; trial<1; trial++)); do

  rm -rf output
  mkdir output
  source setup_freertos.sh > /dev/null
  cd zcu102-zynqmp/ || exit 1

  xsct semi_boot_script.tcl
  cd ..

  sleep 1
  echo ";bootm start 0x200000; bootm loados; bootm go" > /dev/ttyUSB2
  sleep 20

done

cd tests


#cat output.txt | grep "per read" | cut -d " " -f10 > output2.txt