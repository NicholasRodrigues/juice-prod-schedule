// algorithm.cpp

#include "algorithm.h"
#include "neighborhoods.h"
#include <iostream>
#include <chrono>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <functional>
#include <set>
#include <unordered_set>
#include <deque>
#include <queue>

// Improvement counters
int swap_improvement_count = 0;
int block_exchange_improvement_count = 0;
int two_opt_improvement_count = 0;
int block_shift_improvement_count = 0;

/**
 * Prints the improvement statistics for the run.
 */
void printImprovementStatistics() {
    std::cout << "=============================================" << std::endl;
    std::cout << "Improvement Statistics for this run:" << std::endl;
    std::cout << "Swap Neighborhood: " << swap_improvement_count << " improvements" << std::endl;
    std::cout << "Block Exchange Neighborhood: " << block_exchange_improvement_count << " improvements" << std::endl;
    std::cout << "2-Opt Neighborhood: " << two_opt_improvement_count << " improvements" << std::endl;
    std::cout << "Block Shift Neighborhood: " << block_shift_improvement_count << " improvements" << std::endl;
    std::cout << "=============================================" << std::endl;
}

/**
 * Calculates the priority of an order.
 *
 * @param order       The order for which to calculate priority.
 * @param setupTime   The setup time associated with the order.
 * @return            The calculated priority.
 */
double calculatePriority(const Order &order, int setupTime)
{
    // Higher penalty rate and earlier due date should increase priority,
    // while processing time and setup time decrease priority.
    return (order.penaltyRate / (order.processingTime + setupTime)) * (1.0 / (order.dueTime));
}

/**
 * Calculates the total penalty for a given schedule.
 *
 * @param scheduleData       Reference to the schedule data to populate.
 * @param orders             Vector of orders.
 * @param setupTimes         Matrix of setup times between tasks.
 * @param initialSetupTimes  Vector of initial setup times.
 */
void calculateTotalPenalty(ScheduleData &scheduleData, const std::vector<Order> &orders,
                           const std::vector<std::vector<int>> &setupTimes,
                           const std::vector<int> &initialSetupTimes)
{
    double totalPenaltyCost = 0.0;
    long long int currentTime = 0;
    int currentTask = -1;

    int n = scheduleData.schedule.size();
    scheduleData.arrivalTimes.resize(n);
    scheduleData.penalties.resize(n);

    for (size_t i = 0; i < n; ++i)
    {
        const int taskId = scheduleData.schedule[i];
        const Order &order = orders[taskId];

        int setupTime = 0;
        if (currentTask >= 0)
        {
            setupTime = setupTimes[currentTask][taskId];
        }
        else
        {
            setupTime = initialSetupTimes[taskId];
        }
        currentTime += setupTime + order.processingTime;

        scheduleData.arrivalTimes[i] = currentTime;

        double penalty = 0.0;
        if (currentTime > order.dueTime)
        {
            penalty = order.penaltyRate * (currentTime - order.dueTime);
        }
        scheduleData.penalties[i] = penalty;
        totalPenaltyCost += penalty;

        currentTask = taskId;
    }

    scheduleData.totalPenalty = totalPenaltyCost;
}

/**
 * Constructs a schedule using a greedy approach.
 *
 * @param orders             Vector of orders.
 * @param setupTimes         Matrix of setup times between tasks.
 * @param initialSetupTimes  Vector of initial setup times.
 * @param useRCL             Flag indicating whether to use RCL-based selection.
 * @param rclSize            Size of the Restricted Candidate List (used if useRCL is true).
 * @param rng                Pointer to a random number generator (used if useRCL is true).
 * @return                   Constructed schedule as a vector of task IDs.
 */
std::vector<int> greedyConstruction(const std::vector<Order> &orders,
                                    const std::vector<std::vector<int>> &setupTimes,
                                    const std::vector<int> &initialSetupTimes,
                                    double alpha,
                                    std::mt19937* rng)
{
    const int n = orders.size();
    std::vector<int> schedule;
    std::vector<bool> scheduled(n, false);
    int currentTask = -1;

    // Initialize a list of unscheduled tasks with their initial priorities
    std::vector<TaskPriority> unscheduledTasks;
    unscheduledTasks.reserve(n);
    for (int i = 0; i < n; ++i)
    {
        const double priority = calculatePriority(orders[i], initialSetupTimes[i]);
        unscheduledTasks.push_back(TaskPriority{i, priority});
    }

    while (!unscheduledTasks.empty())
    {
        std::vector<TaskPriority> rcl;



        // Find the top rclSize tasks
        std::sort(unscheduledTasks.begin(),unscheduledTasks.end(),
                          [](const TaskPriority& a, const TaskPriority& b) {
                              return a.priority > b.priority; // Descending order
                          });

        // Determine the actual RCL size based on remaining tasks
        const int actualRCLSize = alpha < 0.001 ? 1 : std::ceil(alpha * unscheduledTasks.size());


        // Build the Restricted Candidate List (RCL)
        for(int i = 0; i < actualRCLSize; ++i)
        {
            rcl.push_back(unscheduledTasks[i]);
        }
        int chosenTask =  rand() % actualRCLSize;

        int selectedTaskId = rcl[chosenTask].taskId;

        // Add the selected task to the schedule
        schedule.push_back(selectedTaskId);
        scheduled[selectedTaskId] = true;

        // Remove the selected task from unscheduledTasks
        unscheduledTasks.erase(std::remove_if(unscheduledTasks.begin(), unscheduledTasks.end(),
                                              [&](const TaskPriority& tp) { return tp.taskId == selectedTaskId; }),
                               unscheduledTasks.end());

        // Update currentTask
        currentTask = selectedTaskId;

        // Recalculate priorities for the remaining unscheduled tasks
        for(auto &tp : unscheduledTasks)
        {
            const int newSetupTime = setupTimes[currentTask][tp.taskId];
            tp.priority = calculatePriority(orders[tp.taskId], newSetupTime);
        }
    }

    return schedule;
}


/**
 * Implements the GRASP metaheuristic for scheduling.
 *
 * @param orders             Vector of orders.
 * @param setupTimes         Matrix of setup times between tasks.
 * @param initialSetupTimes  Vector of initial setup times.
 * @param totalPenaltyCost   Reference to store the best total penalty cost found.
 * @param rng                Random number generator.
 * @return                   Best schedule found as a vector of task IDs.
 */
std::vector<int> GRASP(const std::vector<Order>& orders,
                       const std::vector<std::vector<int>>& setupTimes,
                       const std::vector<int>& initialSetupTimes,
                       double& totalPenaltyCost,
                       std::mt19937& rng)
{
    constexpr int maxIterations = GRASP_ITERATIONS;
    std::vector<int> bestSolution;
    double bestPenaltyCost = std::numeric_limits<double>::infinity();

    // Define RCL size (e.g., top 25% of candidates)
    const double alpha = 0.6;
    for (int iter = 0; iter < maxIterations; ++iter)
    {
        // Construct schedule using RCL-based selection
        std::vector<int> newSchedule = greedyConstruction(orders, setupTimes, initialSetupTimes, alpha, &rng);

        // Compute the penalty cost for the new schedule
        ScheduleData scheduleData;
        scheduleData.schedule = newSchedule;
        calculateTotalPenalty(scheduleData, orders, setupTimes, initialSetupTimes);
        double iterationPenaltyCost = scheduleData.totalPenalty;
        // Apply local search with ILS
        newSchedule = ILS(newSchedule, orders, setupTimes, initialSetupTimes, iterationPenaltyCost, rng);

        // Recalculate the penalty cost after ILS
        ScheduleData improvedScheduleData;
        improvedScheduleData.schedule = newSchedule;
        calculateTotalPenalty(improvedScheduleData, orders, setupTimes, initialSetupTimes);

        // Update best solution if improvement is found
        if (const double improvedPenaltyCost = improvedScheduleData.totalPenalty; improvedPenaltyCost < bestPenaltyCost)
        {
            bestSolution = newSchedule;
            bestPenaltyCost = improvedPenaltyCost;

            // Output results when a new best solution is found
            std::cout << "=============================================" << std::endl;
            std::cout << "GRASP iteration " << iter + 1 << ": Best solution updated" << std::endl;
            std::cout << "Best Penalty: " << bestPenaltyCost << std::endl;
            std::cout << "Best Schedule: [";
            for (size_t j = 0; j < bestSolution.size(); ++j)
            {
                std::cout << bestSolution[j] + 1;
                if (j != bestSolution.size() - 1)
                    std::cout << ", ";
            }
            std::cout << "]" << std::endl;
            std::cout << "=============================================" << std::endl;
        }

        // Early termination if optimal solution is found
        if (bestPenaltyCost == 0)
        {
            break;
        }
    }

    printImprovementStatistics();
    totalPenaltyCost = bestPenaltyCost;
    return bestSolution;
}

/**
 * Implements the Neighborhood Descent (RVND) local search.
 *
 * @param scheduleData       Reference to the current schedule data.
 * @param orders             Vector of orders.
 * @param setupTimes         Matrix of setup times between tasks.
 * @param initialSetupTimes  Vector of initial setup times.
 * @param rng                Random number generator.
 */
void RVND(ScheduleData& scheduleData, const std::vector<Order>& orders,
                  const std::vector<std::vector<int>>& setupTimes,
                  const std::vector<int>& initialSetupTimes, std::mt19937& rng)
{
    std::vector<std::function<bool(ScheduleData&, const std::vector<Order>&,
       const std::vector<std::vector<int>> &,
       const std::vector<int>&)>> neighborhoods = {
        reinsertionNeighborhood,
        swapNeighborhood,
        twoOptNeighborhood
};
    bool improvement = true;

    while (improvement)
    {
        improvement = false;
        std::shuffle(neighborhoods.begin(), neighborhoods.end(), rng);  // Shuffle neighborhoods for variability

        for (const auto& neighborhood : neighborhoods)
        {
            // Generate a neighbor using the current neighborhood
            ScheduleData neighborScheduleData = scheduleData; // Create a copy to test the move

            if (bool moved = neighborhood(neighborScheduleData, orders, setupTimes, initialSetupTimes))
            {
                // Accept the move
                scheduleData = std::move(neighborScheduleData);
                improvement = true;
            }

            if (improvement)
            {
                break; // Restart neighborhood search after an improvement
            }
        }
    }
}

/**
 * Perturbs the current schedule to escape local optima.
 *
 * @param schedule The current schedule to perturb.
 * @param rng      Random number generator.
 */
void perturbSolution(std::vector<int>& schedule, std::mt19937& rng) {
    const int n = schedule.size();
    if (n < 8) return;

    // Apply Double Bridge move (diversification)
    int segmentSize = n / 4;
    if (segmentSize < 2) segmentSize = 2;

    // Choose positions using the RNG passed down
    std::uniform_int_distribution<int> posDist(1, n - 3 * segmentSize - 1);
    int pos1 = posDist(rng);
    int pos2 = pos1 + segmentSize;
    int pos3 = pos2 + segmentSize;
    int pos4 = pos3 + segmentSize;

    if (pos4 > n) {
        pos1 = 0;
        pos2 = pos1 + segmentSize;
        pos3 = pos2 + segmentSize;
        pos4 = n;
    }

    std::vector<int> part1(schedule.begin() + pos1, schedule.begin() + pos2);
    std::vector<int> part2(schedule.begin() + pos2, schedule.begin() + pos3);
    std::vector<int> part3(schedule.begin() + pos3, schedule.begin() + pos4);

    schedule.erase(schedule.begin() + pos1, schedule.begin() + pos4);

    schedule.insert(schedule.begin() + pos1, part3.begin(), part3.end());
    schedule.insert(schedule.begin() + pos1 + part3.size(), part1.begin(), part1.end());
    schedule.insert(schedule.begin() + pos1 + part3.size() + part1.size(), part2.begin(), part2.end());
}

/**
 * Implements the Iterated Local Search (ILS) for improving schedules.
 *
 * @param initialSchedule    The initial schedule to start the search.
 * @param orders             Vector of orders.
 * @param setupTimes         Matrix of setup times between tasks.
 * @param initialSetupTimes  Vector of initial setup times.
 * @param currentPenaltyCost Reference to store the penalty cost after ILS.
 * @param rng                Random number generator.
 * @return                   Improved schedule as a vector of task IDs.
 */
std::vector<int> ILS(const std::vector<int>& initialSchedule,
                     const std::vector<Order>& orders,
                     const std::vector<std::vector<int>>& setupTimes,
                     const std::vector<int>& initialSetupTimes,
                     double& currentPenaltyCost,
                     std::mt19937& rng)
{
    // Initialize best and current schedule data
    ScheduleData bestScheduleData;
    bestScheduleData.schedule = initialSchedule;
    calculateTotalPenalty(bestScheduleData, orders, setupTimes, initialSetupTimes);
    double bestPenalty = bestScheduleData.totalPenalty;

    ScheduleData currentScheduleData = bestScheduleData;
    int noImprovementCounter = 0;

    int max_no_improvement_iterations = 4 * initialSchedule.size();

    while (noImprovementCounter < max_no_improvement_iterations)
    {
        // Perform RVND local search
        RVND(currentScheduleData, orders, setupTimes, initialSetupTimes, rng);

        if (currentScheduleData.totalPenalty < bestPenalty)
        {
            bestScheduleData = currentScheduleData;
            bestPenalty = currentScheduleData.totalPenalty;
            noImprovementCounter = 0;
        }
        else
        {
            noImprovementCounter++;
        }

        // Perturb the current solution
        perturbSolution(currentScheduleData.schedule, rng);
        calculateTotalPenalty(currentScheduleData, orders, setupTimes, initialSetupTimes);
    }

    currentPenaltyCost = bestPenalty;
    return bestScheduleData.schedule;
}
