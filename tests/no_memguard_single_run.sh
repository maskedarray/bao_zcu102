#ps | grep -v -e 'bash' -e 'PID' | awk '{print $1}' | xargs kill -9 ;(timeout 1000 stdbuf -oL cat /dev/ttyUSB2 > output.txt &);


SCRIPT_USB_DEVICE="/dev/ttyUSB3"

cd ..

killall cat
killall timeout
(timeout 1000 stdbuf -oL cat $SCRIPT_USB_DEVICE > output.txt &)

NUM_VMS=2
CUA_TEST_TYPE="CUA_WR"
NCUA_TEST_TYPE="NCUA_RD"

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

sed -i "/#endif/i #define $CUA_TEST_TYPE" ./test_defines.h
sed -i "/#endif/i #define $NCUA_TEST_TYPE" ./test_defines.h
sed -i '/#endif/i #define ARRAY_SIZE' ./test_defines.h
sed -i '/#endif/i #define DEF_NO_MEMGUARD_HYP' ./test_defines.h


source setup.sh > /dev/null;
cd zcu102-zynqmp/;
xsct semi_boot_script.tcl;
cd ..;
sleep 1;
echo ";bootm start 0x200000; bootm loados; bootm go" > $SCRIPT_USB_DEVICE
sleep 5;


cd tests
