#!/bin/bash

# Define paths
BUILD_DIR="../build"
DATA_DIR="../data"
RESULTS_DIR="../results"

# Create results directory if it doesn't exist
mkdir -p "$RESULTS_DIR"

# Initialize the summary file
SUMMARY_FILE="$RESULTS_DIR/best_results_summary.txt"
> "$SUMMARY_FILE"

# Function to process a single run
process_run() {
    local instance="$1"
    local instance_name="$2"
    local run_id="$3"
    local tmp_dir="$4"

    echo "Run #$run_id for $instance_name"

    # Run the main program and capture the output
    output=$("$BUILD_DIR/juice_prod_schedule" "$instance")

    # Save output to a temporary file
    echo "$output" > "$tmp_dir/run_${run_id}.out"
}

export -f process_run
export BUILD_DIR

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

    # Create a temporary directory to store run outputs
    tmp_dir=$(mktemp -d)

    # Run 10 runs in parallel using GNU Parallel
    parallel --jobs 10 process_run "$instance" "$instance_name" {#} "$tmp_dir" ::: {1..10}

    # After all runs complete, process the outputs
    for i in {1..10}; do
        output=$(cat "$tmp_dir/run_${i}.out")

        # Extract values from the output
        penalty=$(echo "$output" | grep "TOTAL_PENALTY_OPTIMIZED" | awk '{print $NF}')
        gap=$(echo "$output" | grep "GAP_OPTIMIZED" | awk '{print $NF}' | sed 's/%//')
        execution_time=$(echo "$output" | grep "EXECUTION_TIME" | awk '{print $NF}')
        schedule=$(echo "$output" | grep "OPTIMIZED_SCHEDULE" | awk '{for (j=3; j<=NF; j++) printf $j " "; print ""}')
        seed=$(echo "$output" | grep "SEED_USED" | awk '{print $NF}')

        # Append results to the instance result file
        echo "Run #$i - Penalty: $penalty, GAP: $gap%, Execution Time: $execution_time seconds, Seed: $seed" >> "$result_file"

        # Check if this run is the best one
        # Using awk for floating-point comparison
        is_better=$(awk -v pen="$penalty" -v best="$best_penalty" 'BEGIN { if (pen < best) print 1; else print 0 }')
        if [ "$is_better" -eq 1 ]; then
            best_penalty=$penalty
            best_gap=$gap
            best_execution_time=$execution_time
            best_schedule=$schedule
            best_seed=$seed
        fi
    done

    # Save the best result for this instance
    {
        echo "Best result for $instance_name:"
        echo "Penalty: $best_penalty, GAP: $best_gap%, Execution Time: $best_execution_time seconds, Seed: $best_seed"
        echo "Schedule: $best_schedule"
        echo "--------------------------------"
    } >> "$result_file"

    # Append the best result to the summary file
    echo "$instance_name - Best Penalty: $best_penalty, Best GAP: $best_gap%, Execution Time: $best_execution_time seconds, Seed: $best_seed" >> "$SUMMARY_FILE"

    # Clean up temporary directory
    rm -rf "$tmp_dir"

done

echo "All instances processed. Best results saved in $RESULTS_DIR/best_results_summary.txt"
