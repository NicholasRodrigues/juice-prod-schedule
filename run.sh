#!/bin/bash

# Define paths relative to the repository root
BUILD_DIR="build"         # Path to the build directory
DATA_DIR="data"           # Path to your data files
RESULTS_DIR="results"     # Directory to save the results

# Create build and results directories if they don't exist
mkdir -p "$BUILD_DIR"
mkdir -p "$RESULTS_DIR"

# Change to the build directory
cd "$BUILD_DIR"

# Run CMake and build the project
cmake ..
make -j$(nproc)

# Return to the repository root
cd ..

# Loop through each instance file in the data directory
for instance in "$DATA_DIR"/*.txt; do
    instance_name=$(basename "$instance" .txt)  # Get instance name without extension
    result_file="$RESULTS_DIR/${instance_name}_results.txt"

    echo "Processing $instance_name"

    # Run the program once for this instance
    echo "Run for $instance_name"

    # Run the main program and capture the output
    output=$(./build/juice_prod_schedule "$instance")

    # Extract values from the output
    penalty=$(echo "$output" | grep "TOTAL_PENALTY_OPTIMIZED" | awk '{print $NF}')
    gap=$(echo "$output" | grep "GAP_OPTIMIZED" | awk '{print $NF}' | sed 's/%//')
    execution_time=$(echo "$output" | grep "EXECUTION_TIME" | awk '{print $NF}')
    schedule=$(echo "$output" | grep "OPTIMIZED_SCHEDULE" | awk '{for (i=3; i<=NF; i++) printf $i " "; print ""}')

    # Save results to the instance result file
    echo "Penalty: $penalty" > "$result_file"
    echo "GAP: $gap%" >> "$result_file"
    echo "Execution Time: $execution_time seconds" >> "$result_file"
    echo "Schedule: $schedule" >> "$result_file"
    echo "--------------------------------" >> "$result_file"

    # Append the result to a summary file
    echo "$instance_name - Penalty: $penalty, GAP: $gap%, Execution Time: $execution_time seconds" >> "$RESULTS_DIR/summary.txt"
done

echo "All instances processed. Results saved in $RESULTS_DIR"
