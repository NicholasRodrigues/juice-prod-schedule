#!/bin/bash

# Define paths
BUILD_DIR="../build"
DATA_DIR="../data"
RESULTS_DIR="../results"

# Create results directory if it doesn't exist
mkdir -p "$RESULTS_DIR"

# Initialize log file
LOG_FILE="$RESULTS_DIR/processing_log.txt"
echo "Processing Log - $(date)" > "$LOG_FILE"

# Function to process a single run
process_run() {
    local instance="$1"
    local instance_name="$2"
    local run_id="$3"
    local tmp_dir="$4"

    echo "$(date +'%Y-%m-%d %H:%M:%S') - Run #$run_id for $instance_name" >> "$LOG_FILE"

    # Run the main program and capture the output and errors
    output=$("$BUILD_DIR/juice_prod_schedule" "$instance" 2>&1)
    exit_code=$?

    # Save output to a temporary file
    echo "$output" > "$tmp_dir/run_${run_id}.out"

    # If the program failed, log the error
    if [[ $exit_code -ne 0 ]]; then
        echo "$(date +'%Y-%m-%d %H:%M:%S') - Run #$run_id for $instance_name failed with exit code $exit_code." >> "$LOG_FILE"
    fi
}

export -f process_run
export BUILD_DIR
export LOG_FILE

# Loop through each instance file in the data directory
for instance in "$DATA_DIR"/*.txt; do
    instance_name=$(basename "$instance" .txt)
    result_file="$RESULTS_DIR/${instance_name}_results.txt"

    echo "$(date +'%Y-%m-%d %H:%M:%S') - Processing $instance_name" | tee -a "$LOG_FILE"

    # Initialize associative arrays to store metrics for each heuristic
    declare -A penalties_construction=()
    declare -A times_construction=()
    declare -A gaps_construction=()
    declare -A schedules_construction=()
    declare -A seeds_construction=()

    declare -A penalties_rvnd=()
    declare -A times_rvnd=()
    declare -A gaps_rvnd=()
    declare -A schedules_rvnd=()
    declare -A seeds_rvnd=()

    declare -A penalties_ils_grasp=()
    declare -A times_ils_grasp=()
    declare -A gaps_ils_grasp=()
    declare -A schedules_ils_grasp=()
    declare -A seeds_ils_grasp=()

    # Declare global associative arrays for best metrics
    declare -gA penalties=()
    declare -gA times=()
    declare -gA gaps=()
    declare -gA schedules=()
    declare -gA seeds=()

    # Create a temporary directory to store run outputs
    tmp_dir=$(mktemp -d)
    echo "$(date +'%Y-%m-%d %H:%M:%S') - Temporary directory created at $tmp_dir" | tee -a "$LOG_FILE"

    # Ensure temporary directory is removed on script exit or interruption
    trap 'rm -rf "$tmp_dir"; echo "$(date +'%Y-%m-%d %H:%M:%S') - Temporary directory $tmp_dir removed due to interruption." | tee -a "$LOG_FILE"; exit 1' INT TERM

    # Run 10 runs in parallel using GNU Parallel
    # Ensure GNU Parallel is installed: sudo apt-get install parallel
    parallel --jobs 10 process_run "$instance" "$instance_name" {1} "$tmp_dir" ::: {1..10}

    # After all runs complete, process the outputs
    for i in {1..10}; do
        output_file="$tmp_dir/run_${i}.out"

        if [[ ! -f "$output_file" ]]; then
            echo "Run #$i for $instance_name did not produce an output file. Skipping this run." >> "$result_file"
            echo "$(date +'%Y-%m-%d %H:%M:%S') - Run #$i for $instance_name did not produce an output file." >> "$LOG_FILE"
            continue
        fi

        output=$(cat "$output_file")

        # Check if the output contains 'CONSTRUCTION_ERROR', 'RVND_ERROR', or 'ILS_GRASP_ERROR'
        if echo "$output" | grep -q "CONSTRUCTION_ERROR\|RVND_ERROR\|ILS_GRASP_ERROR"; then
            echo "Run #$i for $instance_name encountered an error. Skipping this run." >> "$result_file"
            echo "$(date +'%Y-%m-%d %H:%M:%S') - Run #$i for $instance_name encountered an error." >> "$LOG_FILE"
            continue
        fi

        # Extract values from the output using grep and awk
        penalty_construction=$(echo "$output" | grep "CONSTRUCTION_PENALTY:" | awk '{print $2}')
        time_construction=$(echo "$output" | grep "CONSTRUCTION_TIME:" | awk '{print $2}')
        gap_construction=$(echo "$output" | grep "CONSTRUCTION_GAP:" | awk '{print $2}' | sed 's/%//')
        schedule_construction=$(echo "$output" | grep "CONSTRUCTION_SCHEDULE:" | awk -F': ' '{print $2}' | tr ' ' ',')
        seed_used=$(echo "$output" | grep "SEED_USED:" | awk '{print $2}')

        penalty_rvnd=$(echo "$output" | grep "RVND_PENALTY:" | awk '{print $2}')
        time_rvnd=$(echo "$output" | grep "RVND_TIME:" | awk '{print $2}')
        gap_rvnd=$(echo "$output" | grep "RVND_GAP:" | awk '{print $2}' | sed 's/%//')
        schedule_rvnd=$(echo "$output" | grep "RVND_SCHEDULE:" | awk -F': ' '{print $2}' | tr ' ' ',')

        penalty_ils_grasp=$(echo "$output" | grep "ILS_GRASP_PENALTY:" | awk '{print $2}')
        time_ils_grasp=$(echo "$output" | grep "ILS_GRASP_TIME:" | awk '{print $2}')
        gap_ils_grasp=$(echo "$output" | grep "ILS_GRASP_GAP:" | awk '{print $2}' | sed 's/%//')
        schedule_ils_grasp=$(echo "$output" | grep "ILS_GRASP_SCHEDULE:" | awk -F': ' '{print $2}' | tr ' ' ',')

        # Append results to associative arrays
        penalties_construction[$i]=$penalty_construction
        times_construction[$i]=$time_construction
        gaps_construction[$i]=$gap_construction
        schedules_construction[$i]=$schedule_construction
        seeds_construction[$i]=$seed_used

        penalties_rvnd[$i]=$penalty_rvnd
        times_rvnd[$i]=$time_rvnd
        gaps_rvnd[$i]=$gap_rvnd
        schedules_rvnd[$i]=$schedule_rvnd
        seeds_rvnd[$i]=$seed_used

        penalties_ils_grasp[$i]=$penalty_ils_grasp
        times_ils_grasp[$i]=$time_ils_grasp
        gaps_ils_grasp[$i]=$gap_ils_grasp
        schedules_ils_grasp[$i]=$schedule_ils_grasp
        seeds_ils_grasp[$i]=$seed_used
    done

    # Function to calculate average and find best penalty, schedule, and seed
    calculate_metrics() {
        local -n penalties_input=$1
        local -n times_input=$2
        local -n gaps_input=$3
        local -n schedules_input=$4
        local -n seeds_input=$5
        local heuristic_name=$6

        local total_penalty=0
        local total_time=0
        local total_gap=0
        local best_penalty=1000000000
        local best_schedule="N/A"
        local best_seed="N/A"
        local valid_runs=0

        for i in {1..10}; do
            penalty=${penalties_input[$i]}
            time=${times_input[$i]}
            gap=${gaps_input[$i]}
            schedule=${schedules_input[$i]}
            seed=${seeds_input[$i]}

            if [[ -z "$penalty" || -z "$time" || -z "$gap" || -z "$schedule" || -z "$seed" ]]; then
                continue
            fi

            # Ensure that penalty, time, gap are valid numbers
            if ! [[ "$penalty" =~ ^-?[0-9]+(\.[0-9]+)?$ ]] || ! [[ "$time" =~ ^-?[0-9]+(\.[0-9]+)?$ ]] || ! [[ "$gap" =~ ^-?[0-9]+(\.[0-9]+)?$ ]]; then
                continue
            fi

            total_penalty=$(echo "$total_penalty + $penalty" | bc)
            total_time=$(echo "$total_time + $time" | bc)
            total_gap=$(echo "$total_gap + $gap" | bc)

            valid_runs=$((valid_runs + 1))

            if (( $(echo "$penalty < $best_penalty" | bc -l) )); then
                best_penalty=$penalty
                best_schedule=$schedule
                best_seed=$seed
            fi
        done

        if (( valid_runs > 0 )); then
            average_penalty=$(echo "scale=6; $total_penalty / $valid_runs" | bc)
            average_time=$(echo "scale=6; $total_time / $valid_runs" | bc)
            average_gap=$(echo "scale=6; $total_gap / $valid_runs" | bc)
        else
            average_penalty="N/A"
            average_time="N/A"
            average_gap="N/A"
            best_penalty="N/A"
            best_schedule="N/A"
            best_seed="N/A"
        fi

        # Assign values to global associative arrays
        penalties[$heuristic_name]=$best_penalty
        times[$heuristic_name]=$average_time
        gaps[$heuristic_name]=$average_gap
        schedules[$heuristic_name]=$best_schedule
        seeds[$heuristic_name]=$best_seed
    }

    # Calculate metrics for each heuristic
    calculate_metrics penalties_construction times_construction gaps_construction schedules_construction seeds_construction "Construction"
    calculate_metrics penalties_rvnd times_rvnd gaps_rvnd schedules_rvnd seeds_rvnd "RVND"
    calculate_metrics penalties_ils_grasp times_ils_grasp gaps_ils_grasp schedules_ils_grasp seeds_ils_grasp "ILS+GRASP"

    # Append the run results and best results to the instance result file
    # Start with a separator line
    echo "--------------------------------" > "$result_file"

    # Iterate over each run and append its results
    for i in {1..10}; do
        echo "Run #$i - Construction - Penalty: ${penalties_construction[$i]}, GAP: ${gaps_construction[$i]}%, Execution Time: ${times_construction[$i]} seconds, Seed: ${seeds_construction[$i]}" >> "$result_file"
        echo "Run #$i - RVND - Penalty: ${penalties_rvnd[$i]}, GAP: ${gaps_rvnd[$i]}%, Execution Time: ${times_rvnd[$i]} seconds, Seed: ${seeds_rvnd[$i]}" >> "$result_file"
        echo "Run #$i - ILS+GRASP - Penalty: ${penalties_ils_grasp[$i]}, GAP: ${gaps_ils_grasp[$i]}%, Execution Time: ${times_ils_grasp[$i]} seconds, Seed: ${seeds_ils_grasp[$i]}" >> "$result_file"
    done

    # Append the best results for each heuristic
    echo "Best result for $instance_name - Construction:" >> "$result_file"
    echo "Penalty: ${penalties["Construction"]}, GAP: ${gaps["Construction"]}%, Execution Time: ${times["Construction"]} seconds, Seed: ${seeds["Construction"]}" >> "$result_file"
    echo "Schedule: ${schedules["Construction"]}" >> "$result_file"
    echo "" >> "$result_file"  # Add an empty line for readability

    echo "Best result for $instance_name - RVND:" >> "$result_file"
    echo "Penalty: ${penalties["RVND"]}, GAP: ${gaps["RVND"]}%, Execution Time: ${times["RVND"]} seconds, Seed: ${seeds["RVND"]}" >> "$result_file"
    echo "Schedule: ${schedules["RVND"]}" >> "$result_file"
    echo "" >> "$result_file"  # Add an empty line for readability

    echo "Best result for $instance_name - ILS+GRASP:" >> "$result_file"
    echo "Penalty: ${penalties["ILS+GRASP"]}, GAP: ${gaps["ILS+GRASP"]}%, Execution Time: ${times["ILS+GRASP"]} seconds, Seed: ${seeds["ILS+GRASP"]}" >> "$result_file"
    echo "Schedule: ${schedules["ILS+GRASP"]}" >> "$result_file"

    # End with a separator line
    echo "--------------------------------" >> "$result_file"

    # Remove the trap for the current temporary directory
    trap - INT TERM EXIT

    # Clean up temporary directory
    rm -rf "$tmp_dir"
    echo "$(date +'%Y-%m-%d %H:%M:%S') - Temporary directory $tmp_dir removed." | tee -a "$LOG_FILE"
done

echo "All instances processed. Best results saved in individual instance result files within $RESULTS_DIR and a processing log in $LOG_FILE." | tee -a "$LOG_FILE"
