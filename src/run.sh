#!/bin/bash

# Define paths
BUILD_DIR="../build"
DATA_DIR="../data"
RESULTS_DIR="../results"

# Create results directory if it doesn't exist
mkdir -p "$RESULTS_DIR"

# Loop through each instance file in the data directory
for instance in "$DATA_DIR"/*.txt; do
    instance_name=$(basename "$instance" .txt)
    result_file="$RESULTS_DIR/${instance_name}_results.txt"

    echo "Processing $instance_name"

    # Initialize best results for this instance
    best_penalty=1000000000
    best_gap=1000000000
    best_execution_time=1000000000
    best_schedule=""
    best_seed=""

    # Run the program 10 times for each instance
    for i in {1..10}; do
        echo "Run #$i for $instance_name"

        # Run the main program and capture the output
        output=$("$BUILD_DIR/juice_prod_schedule" "$instance")

        # Extract values from the output
        penalty=$(echo "$output" | grep "TOTAL_PENALTY_OPTIMIZED" | awk '{print $NF}')
        gap=$(echo "$output" | grep "GAP_OPTIMIZED" | awk '{print $NF}' | sed 's/%//')
        execution_time=$(echo "$output" | grep "EXECUTION_TIME" | awk '{print $NF}')
        schedule=$(echo "$output" | grep "OPTIMIZED_SCHEDULE" | awk '{for (i=3; i<=NF; i++) printf $i " "; print ""}')
        seed=$(echo "$output" | grep "SEED_USED" | awk '{print $NF}')

        # Append results to the instance result file
        echo "Run #$i - Penalty: $penalty, GAP: $gap%, Execution Time: $execution_time seconds, Seed: $seed" >> "$result_file"

        # Check if this run is the best one
        if (( $(echo "$penalty < $best_penalty" | bc -l) )); then
            best_penalty=$penalty
            best_gap=$gap
            best_execution_time=$execution_time
            best_schedule=$schedule
            best_seed=$seed
        fi
    done

    # Save the best result for this instance
    echo "Best result for $instance_name: " >> "$result_file"
    echo "Penalty: $best_penalty, GAP: $best_gap%, Execution Time: $best_execution_time seconds, Seed: $best_seed" >> "$result_file"
    echo "Schedule: $best_schedule" >> "$result_file"
    echo "--------------------------------" >> "$result_file"

    # Append the best result to a summary file
    echo "$instance_name - Best Penalty: $best_penalty, Best GAP: $best_gap%, Execution Time: $best_execution_time seconds, Seed: $best_seed" >> "$RESULTS_DIR/best_results_summary.txt"
done

echo "All instances processed. Best results saved in $RESULTS_DIR/best_results_summary.txt"
