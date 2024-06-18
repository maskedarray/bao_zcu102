#ps | grep -v -e 'bash' -e 'PID' | awk '{print $1}' | xargs kill -9 ;(timeout 1000 stdbuf -oL cat /dev/ttyUSB2 > output.txt &);

PERIOD=1400;
cd ..
killall cat; killall timeout;(timeout 1000 stdbuf -oL cat /dev/ttyUSB2 > output.txt &);
for((trial=0;trial<20;trial++)); do EXTRA_FLAGS1="-DCUA_RD -DARRAY_SIZE"; EXTRA_FLAGS2=-DNCUA_RD; sed -i "s/^#define MEMGUARD_PERIOD .*/#define MEMGUARD_PERIOD $PERIOD/" ./bao-hypervisor/src/core/inc/pmu_v1.h;source setup.sh>/dev/null;cd zcu102-zynqmp/; xsct semi_boot_script.tcl; cd ..; sleep 1; echo ";bootm start 0x200000; bootm loados; bootm go" > /dev/ttyUSB2; sleep 5; if (($PERIOD < 1000)); then ((PERIOD += 100)); else ((PERIOD += 200)); fi; echo $PERIOD; done; 
cat output.txt | grep "per read" | cut -d " " -f5,10 > output2.txt
python3 ./../plotbars.py -i ./output2.txt --legend-include --x-title "Timer Period" --y-title "Cycles Per Load @ 100MHz";