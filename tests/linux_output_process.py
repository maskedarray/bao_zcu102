import re
import pandas as pd
import sys

def parse_output_file(file_path):
    results = []
    set_data = None
    within_set = False  # Track if we're within a "set" (between Budgets and tracking completed)
    buffer = ""  # Accumulate lines for multiline matching

    with open(file_path, 'r') as file:
        for line in file:
            buffer += line  # Add each line to the buffer

            # Match "Budgets = " anywhere in the line
            budget_match = re.search(r"Budgets\s*=\s*(\d+)\s*,\s*(\d+)", line)
            if budget_match:
                if set_data:
                    results.append(set_data)  # Save the previous set
                set_data = {
                    "budget_intf": int(budget_match.group(1)),
                    "budget_cua": int(budget_match.group(2)),
                    "benchmarks": {}
                }
                within_set = True  # We're now inside a set

            # Match the "Running ... Cycles elapsed" pattern in the accumulated buffer
            if within_set:
                cycles_match = re.search(
                    r"Running\s+/home/sdvb/([^/]+)/.*?Cycles elapsed\s+-\s+(\d+)",
                    buffer,
                    re.DOTALL
                )
                if cycles_match and set_data is not None:
                    benchmark_name = cycles_match.group(1)  # Extract the benchmark name
                    cycles_elapsed = int(cycles_match.group(2))
                    set_data["benchmarks"][benchmark_name] = cycles_elapsed
                    buffer = ""  # Clear the buffer after a successful match

            # Match "tracking completed" and close the current set
            if "tracking completed" in line:
                if set_data:
                    results.append(set_data)  # Save the set
                set_data = None
                within_set = False  # We're no longer inside a set
                buffer = ""  # Clear the buffer

    # Add any remaining set data
    if set_data:
        results.append(set_data)
    
    return results

def convert_to_table(parsed_data):
    # Create a DataFrame where rows are CUA budgets and columns are benchmarks
    rows = []
    for data_set in parsed_data:
        row = {"CUA Budget": data_set["budget_cua"]}
        row.update(data_set["benchmarks"])
        rows.append(row)

    df = pd.DataFrame(rows)
    df.set_index("CUA Budget", inplace=True)  # Set CUA Budget as the index
    return df

if __name__ == "__main__":
    # Check if the input file argument is provided
    if len(sys.argv) < 2:
        print("Usage: python script.py <input_file>")
        sys.exit(1)

    input_file = sys.argv[1]

    try:
        # Parse the file
        parsed_data = parse_output_file(input_file)

        # Convert the parsed data into a table
        df = convert_to_table(parsed_data)

        # Convert the DataFrame to CSV with spaces after commas
        csv_output = df.to_csv(index=True).replace(",", ", ")
        print(csv_output)
    except FileNotFoundError:
        print(f"Error: File '{input_file}' not found.")
        sys.exit(1)
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)
