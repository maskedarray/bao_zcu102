
import os
import re

# Step into the parent directory
os.chdir("../")

# Function to read and process each file
def process_file(filename, bmark, append_mode=False):
    budgets_pattern = re.compile(r"Budgets")
    sdvb_pattern = re.compile(rf"sdvb_disparity")

    with open(filename, 'r') as file:
        lines = file.readlines()

    # Insert sdvb_$bmark: 0 where necessary
    processed_lines = []
    i = 0
    while i < len(lines):
        line = lines[i].strip()
        if budgets_pattern.search(line):
            processed_lines.append(line)
            
            found_sdvb = False
            j = i + 1
            while j < len(lines):
                next_line = lines[j].strip()
                if budgets_pattern.search(next_line):
                    break
                if sdvb_pattern.search(next_line):
                    found_sdvb = True
                    break
                j += 1
            
            if not found_sdvb:
                processed_lines.append(f"sdvb_disparity: 0")
                processed_lines.append(f"sdvb_mser: 0")
                processed_lines.append(f"sdvb_localization: 0")
                
        else:
            processed_lines.append(line)
        i += 1

    # Extract columns
    budgets_values = []
    sdvb_bmark_values = []
    sdvb_bmark2_values = []
    sdvb_bmark3_values = []

    for line in processed_lines:
        if budgets_pattern.search(line):
            budgets_values.append(line.split(',')[1].strip())
        elif sdvb_pattern.search(line):
            sdvb_bmark_values.append(line.split(':')[1].strip())
        elif re.search(r'sdvb_mser', line):
            sdvb_bmark2_values.append(line.split(':')[1].strip())
        elif re.search(r'sdvb_localization', line):
            sdvb_bmark3_values.append(line.split(':')[1].strip())

    # Write to CSV
    output_filename = f"output_processed.csv"
    with open(output_filename, 'a') as out_file:
        if not append_mode:  # Write header only once
            out_file.write("Budgets,sdvb_bmark,sdvb_bmark2,sdvb_bmark3\n")
            append_mode = True
        for budget, bmark1, bmark2, bmark3 in zip(budgets_values, sdvb_bmark_values, sdvb_bmark2_values, sdvb_bmark3_values):
            out_file.write(f"{budget},{bmark1},{bmark2},{bmark3}\n")
    
    return append_mode

append_mode = False
for i in range(7):
    filename = f"output_{i}.txt"
    append_mode = process_file(filename, i, append_mode)
