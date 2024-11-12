
import os
import re

# Step into the parent directory
os.chdir("../")
counter = 10
# Function to read and process each file
def process_file(filename, bmark, append_mode=False):
    global counter
    budgets_pattern = re.compile(r"Budgets")
    sdvb_pattern = re.compile(rf"cycles")

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
                processed_lines.append(f"cycles: 0")

                
        else:
            processed_lines.append(line)
        i += 1

    # Extract columns
    budgets_values = []
    sdvb_bmark_values = []


    for line in processed_lines:
        if budgets_pattern.search(line):
            budgets_values.append(line.split(',')[1].strip())
        elif sdvb_pattern.search(line):
            sdvb_bmark_values.append(line.split(' ')[6].strip())
    
    # Write to CSV
    output_filename = f"output_processed.csv"
    with open(output_filename, 'a') as out_file:
        if not append_mode:  # Write header only once
            out_file.write("Colors,Budgets,synth_bmark\n")
            append_mode = True
        for budget, bmark1 in zip(budgets_values, sdvb_bmark_values):
            counter_temp = int(counter/10)
            out_file.write(f"{counter_temp},{budget},{bmark1}\n")
            counter = counter+1
    
    return append_mode

append_mode = False
for i in range(7):
    filename = f"output_{i}.txt"
    append_mode = process_file(filename, i, append_mode)
