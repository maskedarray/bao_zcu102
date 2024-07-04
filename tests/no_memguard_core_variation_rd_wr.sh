#!/bin/bash

# Function to run the sequence for a specific number of VMs
run_for_num_vms() {
    local num_vms=$1
    local output_file="../output${num_vms}.txt"
    
    bash reset_environment.sh
    sed -i "s/^#define __NUM_VMS.*/#define __NUM_VMS ${num_vms}/" ../common_defines.h
    EXTRA_FLAGS1="-DCUA_RD -DARRAY_SIZE" EXTRA_FLAGS2="-DNCUA_WR" bash period_variation_no_period_variation.sh
    sleep 5
    cat ../output.txt > "${output_file}"
}

# Run for 1 to 4 VMs
run_for_num_vms 1
run_for_num_vms 2
run_for_num_vms 3
run_for_num_vms 4

# Concatenate all output files into the final output.txt
cat ../output1.txt ../output2.txt ../output3.txt ../output4.txt > ../output.txt

# Clean up temporary files
rm ../output1.txt ../output2.txt ../output3.txt ../output4.txt
