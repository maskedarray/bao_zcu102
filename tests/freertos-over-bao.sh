#ps | grep -v -e 'bash' -e 'PID' | awk '{print $1}' | xargs kill -9 ;(timeout 1000 stdbuf -oL cat /dev/ttyUSB2 > output.txt &);


cd ..
SCRIPT_USB_DEVICE="/dev/ttyUSB3"
killall cat; killall timeout;(timeout 1000 stdbuf -oL cat $SCRIPT_USB_DEVICE > output.txt &);

# EXTRA_FLAGS1="-DCUA_RD -DSTATIC_FAT_IMG"; EXTRA_FLAGS2="-DNCUA_RD"; 
# HYP_FLAGS="-DPERIOD_VARIATION_NO_PERIOD -DSTATIC_FAT_IMG"; 

NUM_VMS=1
NCUA_TEST_TYPE="NCUA_WR"

SCRIPT_CUA_BUDGET=1
SCRIPT_NCUA_BUDGET=1
SCRIPT_MEMGUARD_PERIOD=1400

echo ""> ./test_defines.h
sed -i '1s/^.*$/#ifndef __TEST_DEFINES_H__\n#define __TEST_DEFINES_H__\n\n#endif/' ./test_defines.h

#############################################################################################
sed -i "/#endif/i #define TOTAL_VMS $NUM_VMS" ./test_defines.h

sed -i '/#endif/i #define BASE_ADDR_CORE0 0x20000000' ./test_defines.h
sed -i '/#endif/i #define MEM_SIZE_CORE0 0x4000000' ./test_defines.h
sed -i '/#endif/i #define COLORS_CORE0 0x03' ./test_defines.h

sed -i '/#endif/i #define BASE_ADDR_CORE1 0x30000000' ./test_defines.h
sed -i '/#endif/i #define MEM_SIZE_CORE1 0x4000000' ./test_defines.h
sed -i '/#endif/i #define COLORS_CORE1 0xFC' ./test_defines.h

sed -i '/#endif/i #define BASE_ADDR_CORE2 0x35000000' ./test_defines.h
sed -i '/#endif/i #define MEM_SIZE_CORE2 0x4000000' ./test_defines.h
sed -i '/#endif/i #define COLORS_CORE2 0xFC' ./test_defines.h

sed -i '/#endif/i #define BASE_ADDR_CORE3 0x3A000000' ./test_defines.h
sed -i '/#endif/i #define MEM_SIZE_CORE3 0x4000000' ./test_defines.h
sed -i '/#endif/i #define COLORS_CORE3 0xFC' ./test_defines.h
#############################################################################################

sed -i '/#endif/i #define MEMGUARD_BUDGET_CUA '"$SCRIPT_CUA_BUDGET" ./test_defines.h
sed -i '/#endif/i #define MEMGUARD_BUDGET_NCUA '"$SCRIPT_NCUA_BUDGET" ./test_defines.h
sed -i '/#endif/i #define MEMGUARD_PERIOD '"$SCRIPT_MEMGUARD_PERIOD" ./test_defines.h

sed -i "/#endif/i #define $NCUA_TEST_TYPE" ./test_defines.h
sed -i '/#endif/i #define DEF_NO_MEMGUARD_HYP' ./test_defines.h



source setup_freertos.sh > /dev/null
cd zcu102-zynqmp/
xsct semi_boot_script.tcl
cd ..
sleep 1
echo ";bootm start 0x200000; bootm loados; bootm go" > $SCRIPT_USB_DEVICE

cd tests
