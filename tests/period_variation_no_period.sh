#ps | grep -v -e 'bash' -e 'PID' | awk '{print $1}' | xargs kill -9 ;(timeout 1000 stdbuf -oL cat /dev/ttyUSB2 > output.txt &);


cd ..
killall cat; killall timeout;(timeout 1000 stdbuf -oL cat /dev/ttyUSB2 > output.txt &);
sed -i "s/^#define MEMGUARD_BUDGET_CUA .*/#define MEMGUARD_BUDGET_CUA 1/" ./bao-hypervisor/src/core/inc/pmu_v1.h
sed -i "s/^#define MEMGUARD_BUDGET_NCUA .*/#define MEMGUARD_BUDGET_NCUA 1/" ./bao-hypervisor/src/core/inc/pmu_v1.h
EXTRA_FLAGS1="-DCUA_RD -DARRAY_SIZE"; EXTRA_FLAGS2=-DNCUA_RD; HYP_FLAGS=-DPERIOD_VARIATION_NO_PERIOD; sed -i "s/^#define MEMGUARD_PERIOD .*/#define MEMGUARD_PERIOD 900000000/" ./bao-hypervisor/src/core/inc/pmu_v1.h;source setup.sh>/dev/null;cd zcu102-zynqmp/; xsct semi_boot_script.tcl; cd ..; sleep 1; echo ";bootm start 0x200000; bootm loados; bootm go" > /dev/ttyUSB2; sleep 5; 
cd tests
