# Using $(...) syntax for command substitution
total_counter_difference=$(cat ../output.txt | grep "Total Counter Difference" | cut -d " " -f5)
time_taken=$(cat ../output.txt | grep "Time taken" | cut -d " " -f3)

result=$(python3 -c "print(($total_counter_difference / $time_taken) * 6103.515625)")

# Print the result
echo "Result: $result"
