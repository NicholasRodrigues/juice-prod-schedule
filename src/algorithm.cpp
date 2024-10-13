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
#include "tabu_list.h"


// algorithm.cpp
int swap_improvement_count = 0;
int block_exchange_improvement_count = 0;
int two_opt_improvement_count = 0;
int block_shift_improvement_count = 0;


// Tabu Entry structure
struct TabuEntry {
    size_t solutionHash;
    int tenure;
};

void printImprovementStatistics() {
    std::cout << "=============================================" << std::endl;
    std::cout << "Improvement Statistics for this run:" << std::endl;
    std::cout << "Swap Neighborhood: " << swap_improvement_count << " improvements" << std::endl;
    std::cout << "Block Exchange Neighborhood: " << block_exchange_improvement_count << " improvements" << std::endl;
    std::cout << "2-Opt Neighborhood: " << two_opt_improvement_count << " improvements" << std::endl;
    std::cout << "Block Shift Neighborhood: " << block_shift_improvement_count << " improvements" << std::endl;
    std::cout << "=============================================" << std::endl;
}


constexpr int TABU_TENURE = 100;           // Tenure for tabu entries
constexpr int MAX_TABU_LIST_SIZE = 1000;  // Maximum size of the tabu list

std::unordered_set<size_t> tabuSet;  // For O(1) lookup
std::deque<TabuEntry> tabuQueue;     // To track entry order and tenure



size_t computeScheduleHash(const std::vector<int>& schedule) {
    size_t seed = 0;
    for (const int job : schedule) {
        std::hash<int> intHasher;
        seed ^= intHasher(job) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}


double calculatePriority(const Order &order, const int setupTime)
{
    // Higher penalty rate and earlier due date should increase priority, but we also account for processing time and setup time
    return (order.penaltyRate / (order.processingTime + setupTime)) * (1.0 / (order.dueTime));
}

// Threshold acceptance function for slightly worse solutions
// bool acceptSolution(double currentPenalty, double newPenalty, double threshold) {
//    return (newPenalty < currentPenalty) || (std::exp((currentPenalty - newPenalty) / threshold) > ((double) rand() / RAND_MAX));
//}

// algorithm.cpp
// algorithm.cpp
void calculateTotalPenalty(ScheduleData &scheduleData, const std::vector<Order> &orders,
                           const std::vector<std::vector<int>> &setupTimes,
                           const std::vector<int> &initialSetupTimes)
{
    double totalPenaltyCost = 0.0;
    long long int currentTime = 0;
    int currentTask = -1;

    const int n = scheduleData.schedule.size();
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
//
// double calculateIncrementalPenalty(ScheduleData &scheduleData, const std::vector<Order> &orders,
//                                    const std::vector<std::vector<int> > &setupTimes,
//                                    const std::vector<int> &initialSetupTimes, int startIdx) {
//     // O startIdx define de onde começar a recalcular
//     int n = scheduleData.schedule.size();
//     long long int currentTime = 0;
//     double totalPenalty = scheduleData.totalPenalty;
//
//     // Subtraímos as penalidades antigas das tarefas a partir de startIdx até o final
//     for (int i = startIdx; i < n; ++i) {
//         totalPenalty -= scheduleData.penalties[i];
//     }
//
//     // Usamos o tempo e a tarefa anteriores para recalcular a partir de startIdx
//     if (startIdx > 0) {
//         currentTime = scheduleData.arrivalTimes[startIdx - 1];
//     } else {
//         currentTime = 0; // Se for a primeira tarefa, o tempo é 0
//     }
//
//     int currentTask = (startIdx > 0) ? scheduleData.schedule[startIdx - 1] : -1;
//
//     // Recalcula penalidade e tempo de chegada a partir de startIdx
//     for (int i = startIdx; i < n; ++i) {
//         int taskId = scheduleData.schedule[i];
//         const Order &order = orders[taskId];
//
//         int setupTime = 0;
//         if (currentTask >= 0) {
//             setupTime = setupTimes[currentTask][taskId];
//         } else {
//             setupTime = initialSetupTimes[taskId];
//         }
//
//         currentTime += setupTime + order.processingTime;
//         scheduleData.arrivalTimes[i] = currentTime;
//
//         double penalty = 0.0;
//         if (currentTime > order.dueTime) {
//             penalty = order.penaltyRate * (currentTime - order.dueTime);
//         }
//         scheduleData.penalties[i] = penalty;
//         totalPenalty += penalty;
//
//         currentTask = taskId; // Atualiza a tarefa corrente
//     }
//
//     return totalPenalty;
// }
// New greedy algorithm considering both due date, penalty rate, and setup time (dynamic)
std::vector<int> greedyAlgorithm(const std::vector<Order> &orders,
                                 const std::vector<std::vector<int>> &setupTimes,
                                 const std::vector<int> &initialSetupTimes,
                                 double &totalPenaltyCost)
{
    int n = orders.size();
    std::vector<bool> scheduled(n, false); // Track which jobs have been scheduled
    std::vector<int> schedule;
    int currentTime = 0;
    int currentTask = -1;
    totalPenaltyCost = 0.0;

    for (int count = 0; count < n; ++count)
    {
        int bestTask = -1;
        double bestPriority = -1.0;

        for (int i = 0; i < n; ++i)
        {
            if (scheduled[i])
                continue; // Skip jobs that are already scheduled

            // Calculate setup time dynamically (from last scheduled task)
            const int setupTime = (currentTask >= 0) ? setupTimes[currentTask][i] : initialSetupTimes[i];

            // Calculate the priority for this job

            if (const double priority = calculatePriority(orders[i], setupTime); bestTask == -1 || priority > bestPriority)
            {
                bestTask = i;
                bestPriority = priority;
            }
        }

        // Schedule the best task found
        scheduled[bestTask] = true;
        schedule.push_back(bestTask);

        const int setupTime = (currentTask >= 0) ? setupTimes[currentTask][bestTask] : initialSetupTimes[bestTask];
        currentTime += setupTime + orders[bestTask].processingTime;

        if (currentTime > orders[bestTask].dueTime)
        {
            const double penalty = orders[bestTask].penaltyRate * (currentTime - orders[bestTask].dueTime);
            totalPenaltyCost += penalty;
        }

        currentTask = bestTask;
    }

    return schedule;
}

// algorithm.cpp
std::vector<int> GRASP(const std::vector<Order>& orders,
                       const std::vector<std::vector<int>>& setupTimes,
                       const std::vector<int>& initialSetupTimes,
                       double& totalPenaltyCost,
                       std::mt19937& rng)
{
    constexpr int maxIterations = GRASP_ITERATIONS;
    std::vector<int> bestSolution;
    double bestPenaltyCost = std::numeric_limits<double>::infinity();

    const int rclSize = orders.size() / 4;

    for (int iter = 0; iter < maxIterations; ++iter) {
        std::vector<int> newSchedule;
        const int n = orders.size();

        // Priority queue for greedy task selection, based on dynamic priorities
        std::priority_queue<TaskPriority> taskQueue;

        // Add all tasks to the priority queue based on their initial setup times
        for (int i = 0; i < n; ++i) {
            const int setupTime = initialSetupTimes[i];
            const double priority = calculatePriority(orders[i], setupTime);
            taskQueue.push({i, priority});
        }

        // List to track scheduled tasks
        std::vector<bool> scheduled(n, false);
        int currentTask = -1;
        int currentTime = 0;

        while (!taskQueue.empty()) {
            // Build Restricted Candidate List (RCL) from top RCL_SIZE tasks in the heap
            std::vector<TaskPriority> rcl;
            for (int i = 0; i < rclSize && !taskQueue.empty(); ++i) {
                rcl.push_back(taskQueue.top());
                taskQueue.pop();
            }

            // Randomly select a task from the RCL
            std::uniform_int_distribution<int> distribution(0, rcl.size() - 1);
            const int rclIndex = distribution(rng);
            int selectedTaskId = rcl[rclIndex].taskId;

            // Schedule the selected task
            newSchedule.push_back(selectedTaskId);
            scheduled[selectedTaskId] = true;

            // Update current time and penalties based on the selected task
            const int setupTime = (currentTask >= 0) ? setupTimes[currentTask][selectedTaskId] : initialSetupTimes[selectedTaskId];
            currentTime += setupTime + orders[selectedTaskId].processingTime;

            // Recalculate priorities for unscheduled tasks and repopulate the heap
            std::priority_queue<TaskPriority> newQueue;
            for (int i = 0; i < n; ++i) {
                if (!scheduled[i]) {
                    const int newSetupTime = (currentTask >= 0) ? setupTimes[currentTask][i] : initialSetupTimes[i];
                    const double newPriority = calculatePriority(orders[i], newSetupTime);
                    newQueue.push({i, newPriority});
                }
            }
            taskQueue = std::move(newQueue); // Replace the old heap with the updated one

            currentTask = selectedTaskId;
        }

        // Compute the penalty cost for the new schedule before ILS
        ScheduleData scheduleData;
        scheduleData.schedule = newSchedule;
        calculateTotalPenalty(scheduleData, orders, setupTimes, initialSetupTimes);
        double iterationPenaltyCost = scheduleData.totalPenalty;

        // Apply local search with ILS, passing RNG down to ILS
        newSchedule = ILS(newSchedule, orders, setupTimes, initialSetupTimes, iterationPenaltyCost, rng);

        if (iterationPenaltyCost < bestPenaltyCost) {
            bestSolution = newSchedule;
            bestPenaltyCost = iterationPenaltyCost;

            // Output results when a new best solution is found
            std::cout << "=============================================" << std::endl;
            std::cout << "GRASP iteration " << iter + 1 << ": Best solution updated" << std::endl;
            std::cout << "Best Penalty: " << bestPenaltyCost << std::endl;
            std::cout << "Best Schedule: [";
            for (size_t j = 0; j < bestSolution.size(); ++j) {
                std::cout << bestSolution[j] + 1;
                if (j != bestSolution.size() - 1) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
            std::cout << "=============================================" << std::endl;
        }

        if (bestPenaltyCost == 0) {
            break;  // Early exit if optimal solution found
        }
    }

    totalPenaltyCost = bestPenaltyCost;
    return bestSolution;
}
void adaptiveRVND(ScheduleData& scheduleData, const std::vector<Order>& orders,
                  const std::vector<std::vector<int>>& setupTimes,
                  const std::vector<int>& initialSetupTimes, std::mt19937& rng)
{
    std::vector<std::function<bool(ScheduleData&, const std::vector<Order>&,
                                   const std::vector<std::vector<int>>&,
                                   const std::vector<int>&)>> neighborhoods = {
        blockExchangeNeighborhood,
        blockShiftNeighborhood,
        twoOptNeighborhood
    };

    bool improvement = true;

    // Initialize the best solution as the current solution
    ScheduleData bestScheduleData = scheduleData;
    double bestPenalty = scheduleData.totalPenalty;

    while (improvement) {
        improvement = false;
        std::shuffle(neighborhoods.begin(), neighborhoods.end(), rng);  // Use the given RNG
        //NAO USAR SHUFFLE

        for (const auto& neighborhood : neighborhoods) {
            if (neighborhood(scheduleData, orders, setupTimes, initialSetupTimes)) {
                improvement = true;

                // Check if the new solution is better than the best found so far
                if (scheduleData.totalPenalty < bestPenalty) {
                    bestPenalty = scheduleData.totalPenalty;
                    bestScheduleData = scheduleData;  // Update best solution
                }
                break;  // Restart the loop after an improvement
            }
        }
    }

    // After the RVND finishes, update the input schedule data with the best solution found
    scheduleData = bestScheduleData;
}
void perturbSolution(std::vector<int>& schedule, std::mt19937& rng) {
    int n = schedule.size();
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

std::vector<int> ILS(const std::vector<int>& initialSchedule,
                     const std::vector<Order>& orders,
                     const std::vector<std::vector<int>>& setupTimes,
                     const std::vector<int>& initialSetupTimes,
                     double& currentPenaltyCost,
                     std::mt19937& rng)
{
    ScheduleData bestScheduleData;
    bestScheduleData.schedule = initialSchedule;
    double bestPenalty = currentPenaltyCost;

    ScheduleData currentScheduleData = bestScheduleData;
    int noImprovementCounter = 0;

    int iteration = 0;
    int max_no_improvement_iterations = 4*initialSchedule.size();

    while (noImprovementCounter < max_no_improvement_iterations) {
        iteration++;

        // Perform RVND local search, passing RNG down
        adaptiveRVND(currentScheduleData, orders, setupTimes, initialSetupTimes, rng);

        if (currentScheduleData.totalPenalty < bestPenalty) { //NAO SEI SE PRECISA DISSO AQ
            bestScheduleData = currentScheduleData;
            bestPenalty = currentScheduleData.totalPenalty;
            noImprovementCounter = 0;
        } else {
            noImprovementCounter++;
        }

        // Perturb the current solution using the same RNG
        perturbSolution(currentScheduleData.schedule, rng);
        calculateTotalPenalty(currentScheduleData, orders, setupTimes, initialSetupTimes);
    }

    currentPenaltyCost = bestPenalty;
    return bestScheduleData.schedule;
}
