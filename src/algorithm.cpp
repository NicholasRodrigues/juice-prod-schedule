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

// Tabu Entry structure
struct TabuEntry {
    size_t solutionHash;
    int tenure;
};

const int TABU_TENURE = 100;           // Tenure for tabu entries
const int MAX_TABU_LIST_SIZE = 100000;  // Maximum size of the tabu list

std::unordered_set<size_t> tabuSet;  // For O(1) lookup
std::deque<TabuEntry> tabuQueue;     // To track entry order and tenure



size_t computeScheduleHash(const std::vector<int>& schedule) {
    std::hash<int> intHasher;
    size_t seed = 0;
    for (int job : schedule) {
        seed ^= intHasher(job) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
}


double calculatePriority(const Order &order, int setupTime)
{
    // Higher penalty rate and earlier due date should increase priority, but we also account for processing time and setup time
    return (order.penaltyRate / (order.processingTime + setupTime)) * (1.0 / (order.dueTime));
}

// Threshold acceptance function for slightly worse solutions
// bool acceptSolution(double currentPenalty, double newPenalty, double threshold) {
//    return (newPenalty < currentPenalty) || (std::exp((currentPenalty - newPenalty) / threshold) > ((double) rand() / RAND_MAX));
//}

double calculateTotalPenalty(const std::vector<int> &schedule, const std::vector<Order> &orders,
                             const std::vector<std::vector<int>> &setupTimes,
                             const std::vector<int> &initialSetupTimes)
{
    double totalPenaltyCost = 0.0;
    int currentTime = 0;
    int currentTask = -1;

    for (size_t i = 0; i < schedule.size(); ++i)
    {
        int taskId = schedule[i];
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
        currentTime += setupTime;

        currentTime += order.processingTime;

        if (currentTime > order.dueTime)
        {
            double penalty = order.penaltyRate * (currentTime - order.dueTime);
            totalPenaltyCost += penalty;
        }

        currentTask = taskId;
    }

    return totalPenaltyCost;
}

double calculateMaxPenalty(const std::vector<int>& schedule, const std::vector<Order>& orders,
                           const std::vector<std::vector<int>>& setupTimes,
                           const std::vector<int>& initialSetupTimes) {
    double maxPenalty = 0.0;
    int currentTime = 0;
    int currentTask = -1;

    for (size_t i = 0; i < schedule.size(); ++i) {
        int taskId = schedule[i];
        const Order& order = orders[taskId];

        int setupTime = (currentTask >= 0) ? setupTimes[currentTask][taskId] : initialSetupTimes[taskId];
        currentTime += setupTime + order.processingTime;

        if (currentTime > order.dueTime) {
            double penalty = order.penaltyRate * (currentTime - order.dueTime);
            if (penalty > maxPenalty) {
                maxPenalty = penalty;
            }
        }

        currentTask = taskId;
    }

    return maxPenalty;
}


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
            int setupTime = (currentTask >= 0) ? setupTimes[currentTask][i] : initialSetupTimes[i];

            // Calculate the priority for this job
            double priority = calculatePriority(orders[i], setupTime);

            if (bestTask == -1 || priority > bestPriority)
            {
                bestTask = i;
                bestPriority = priority;
            }
        }

        // Schedule the best task found
        scheduled[bestTask] = true;
        schedule.push_back(bestTask);

        int setupTime = (currentTask >= 0) ? setupTimes[currentTask][bestTask] : initialSetupTimes[bestTask];
        currentTime += setupTime + orders[bestTask].processingTime;

        if (currentTime > orders[bestTask].dueTime)
        {
            double penalty = orders[bestTask].penaltyRate * (currentTime - orders[bestTask].dueTime);
            totalPenaltyCost += penalty;
        }

        currentTask = bestTask;
    }

    return schedule;
}

std::vector<int> GRASP(const std::vector<Order>& orders,
                       const std::vector<std::vector<int>>& setupTimes,
                       const std::vector<int>& initialSetupTimes,
                       double& totalPenaltyCost) {
    int maxIterations = GRASP_ITERATIONS;
    std::vector<int> bestSolution;
    double bestPenaltyCost = std::numeric_limits<double>::infinity();

    int rclSize = RCL_SIZE;

    std::vector<Order> sortedOrders = orders;
    std::sort(sortedOrders.begin(), sortedOrders.end(), [](const Order& a, const Order& b) {
        return a.dueTime < b.dueTime;
    });

    std::mt19937 rng(std::random_device{}());

    // Initialize the Tabu List outside the loop to maintain it across GRASP runs
    TabuList tabuList(MAX_TABU_LIST_SIZE);

    for (int iter = 0; iter < maxIterations; ++iter) {
        std::vector<int> newSchedule;
        int n = sortedOrders.size();

        // Initialize ordersPoolIndices with indices to sortedOrders
        std::vector<int> ordersPoolIndices(n);
        std::iota(ordersPoolIndices.begin(), ordersPoolIndices.end(), 0);

        while (!ordersPoolIndices.empty()) {
            int rclSizeLimited = std::min(rclSize, static_cast<int>(ordersPoolIndices.size()));

            // Randomly select an index from the first rclSizeLimited indices
            std::uniform_int_distribution<int> distribution(0, rclSizeLimited - 1);
            int rclIndex = distribution(rng);
            int selectedIndex = ordersPoolIndices[rclIndex];

            const Order& selectedOrder = sortedOrders[selectedIndex];

            // Add selected order to the new schedule
            newSchedule.push_back(selectedOrder.id);

            // Remove selectedIndex from ordersPoolIndices
            std::swap(ordersPoolIndices[rclIndex], ordersPoolIndices.back());
            ordersPoolIndices.pop_back();
        }

        // Compute the penalty cost for the new schedule before ILS
        double iterationPenaltyCost = calculateTotalPenalty(newSchedule, orders, setupTimes, initialSetupTimes);

        // Apply local search with ILS, passing the local penalty cost and the Tabu List
        newSchedule = ILS(newSchedule, orders, setupTimes, initialSetupTimes, iterationPenaltyCost, tabuList);

        // After ILS, iterationPenaltyCost is updated inside ILS

        if (iterationPenaltyCost < bestPenaltyCost) {
            bestSolution = newSchedule;
            bestPenaltyCost = iterationPenaltyCost;
        }

        if (bestPenaltyCost == 0) {
            break;
        }
    }

    totalPenaltyCost = bestPenaltyCost;

    std::cout << "Penalty cost after GRASP: " << bestPenaltyCost << std::endl;

    return bestSolution;
}


// std::vector<int> adaptiveRVND(std::vector<int>& schedule, const std::vector<Order>& orders,
//                               const std::vector<std::vector<int>>& setupTimes,
//                               const std::vector<int>& initialSetupTimes, double& currentPenalty) {
//     std::vector<std::function<bool(std::vector<int>&, const std::vector<Order>&,
//     const std::vector<std::vector<int>>&,
//     const std::vector<int>&, double&)> > neighborhoods = {
//             reinsertionNeighborhood,
//             twoOptNeighborhood,
//             swapNeighborhood
//     };
//
//     bool improvement = true;
//     std::mt19937 rng(std::random_device{}());
//
//     while (improvement) {
//         improvement = false;
//
//         std::shuffle(neighborhoods.begin(), neighborhoods.end(), rng);
//
//         for (size_t i = 0; i < neighborhoods.size(); ++i) {
//             bool neighborhoodImprovement = neighborhoods[i](schedule, orders, setupTimes, initialSetupTimes, currentPenalty);
//             if (neighborhoodImprovement) {
//                 improvement = true;
//                 break;
//             }
//         }
//     }
//
//     return schedule;
// }

// Dynamic weights for neighborhoods
void adaptiveShuffle(std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                                    const std::vector<std::vector<int>> &,
                                                    const std::vector<int> &, double &)>> &neighborhoods,
                     std::vector<double> &neighborhoodWeights, std::mt19937 &rng)
{

    if (neighborhoods.empty() || neighborhoodWeights.empty())
    {
        std::cerr << "Error: Neighborhoods or weights are empty!" << std::endl;
        return;
    }

    // Total weight of all neighborhoods
    double totalWeight = std::accumulate(neighborhoodWeights.begin(), neighborhoodWeights.end(), 0.0);

    // If all weights are zero, reset to equal probabilities
    if (totalWeight == 0.0)
    {
        for (double &weight : neighborhoodWeights)
        {
            weight = 1.0;
        }
        totalWeight = neighborhoodWeights.size();
    }

    // Calculate the probability for each neighborhood based on its weight
    std::vector<double> probabilities;
    for (double weight : neighborhoodWeights)
    {
        probabilities.push_back(weight / totalWeight);
    }

    std::discrete_distribution<> dist(probabilities.begin(), probabilities.end());

    // Shuffle neighborhoods based on their weights
    std::vector<int> indices(neighborhoods.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), rng);

    std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                   const std::vector<std::vector<int>> &,
                                   const std::vector<int> &, double &)>>
        shuffledNeighborhoods;

    for (int idx : indices)
    {
        shuffledNeighborhoods.push_back(neighborhoods[idx]);
    }

    neighborhoods = shuffledNeighborhoods;
}

// RVND with adaptive neighborhood selection and dynamic acceptance
// RVND with tabu search mechanism
std::vector<int> adaptiveRVND(std::vector<int>& schedule, const std::vector<Order>& orders,
                              const std::vector<std::vector<int>>& setupTimes,
                              const std::vector<int>& initialSetupTimes, double& currentPenalty) {
    // Define the available neighborhoods
    std::vector<std::function<bool(std::vector<int>&, const std::vector<Order>&,
    const std::vector<std::vector<int>>&,
    const std::vector<int>&, double&)>>
    neighborhoods = {
            reinsertionNeighborhood,
            swapNeighborhood,
            blockExchangeNeighborhood,
            blockShiftNeighborhood,
            threeOptNeighborhood
    };

    std::mt19937 rng(std::random_device{}());
    bool improvement = true;

    while (improvement) {
        improvement = false;

        // Shuffle the neighborhoods at each iteration
        std::shuffle(neighborhoods.begin(), neighborhoods.end(), rng);

        for (const auto& neighborhood : neighborhoods) {
            double previousPenalty = currentPenalty;
            std::vector<int> tempSchedule = schedule;  // Copy current schedule

            bool neighborhoodImprovement = neighborhood(tempSchedule, orders, setupTimes, initialSetupTimes, currentPenalty);

            if (neighborhoodImprovement) {
                // Accept the move
                schedule = tempSchedule;
                improvement = true;
                break;  // Restart after finding an improvement
            } else {
                currentPenalty = previousPenalty;  // Restore penalty
            }
        }
    }

    return schedule;
}




//// Perturbation function for ILS
// void perturbSolution(std::vector<int>& schedule, int perturbationStrength) {
//     int n = schedule.size();
//     if (n < 8) return;
//
//     std::mt19937 rng(std::random_device{}());
//
//     std::uniform_int_distribution<int> perturbationChoiceDist(1, 3);  // 1: 2-opt, 2: double bridge, 3: both
//     int perturbationChoice = perturbationChoiceDist(rng);
//
//     if (perturbationChoice == 1 || perturbationChoice == 3) {
//         std::uniform_int_distribution<int> moveCountDist(3, 5);
//         std::uniform_int_distribution<int> blockSizeDist(2, std::min(5, n / 2));
//
//         int numMoves = moveCountDist(rng);
//
//         for (int move = 0; move < numMoves; ++move) {
//             int blockSize = blockSizeDist(rng);
//
//             if (blockSize >= n) continue;
//
//             std::uniform_int_distribution<int> indexDist(0, n - blockSize - 1);
//             int i = indexDist(rng);
//             int j = i + blockSize - 1;
//
//             std::reverse(schedule.begin() + i, schedule.begin() + j + 1);
//         }
//     }
//
//     if (perturbationChoice == 2 || perturbationChoice == 3) {
//         int segmentSize = n / 4;
//         std::uniform_int_distribution<int> posDist(1, n - 1);
//
//         int pos1 = posDist(rng) % (n - 3 * segmentSize);
//         int pos2 = pos1 + segmentSize;
//         int pos3 = pos2 + segmentSize;
//         int pos4 = pos3 + segmentSize;
//
//         if (pos4 >= n) {
//             pos1 = 0;
//             pos2 = pos1 + segmentSize;
//             pos3 = pos2 + segmentSize;
//             pos4 = pos3 + segmentSize;
//         }
//
//         std::vector<int> part1(schedule.begin() + pos1, schedule.begin() + pos2);
//         std::vector<int> part2(schedule.begin() + pos2, schedule.begin() + pos3);
//         std::vector<int> part3(schedule.begin() + pos3, schedule.begin() + pos4);
//
//         schedule.erase(schedule.begin() + pos1, schedule.begin() + pos4);
//
//         schedule.insert(schedule.begin() + pos1, part3.begin(), part3.end());
//         schedule.insert(schedule.begin() + pos1 + part3.size(), part1.begin(), part1.end());
//         schedule.insert(schedule.begin() + pos1 + part3.size() + part1.size(), part2.begin(), part2.end());
//     }
// }

// Perturbation function using 2-opt, double bridge, and shuffling
//void perturbSolution(std::vector<int> &schedule, int perturbationStrength)
//{
//    int n = schedule.size();
//    if (n < 8)
//        return; // Ensure there are enough jobs to perform significant perturbations
//
//    std::mt19937 rng(std::random_device{}());
//
//    // Randomly choose the perturbation type (1: 2-opt, 2: double bridge, 3: shuffling)
//    std::uniform_int_distribution<int> perturbationChoiceDist(1, 3);
////    int perturbationChoice = perturbationChoiceDist(rng);
//    int perturbationChoice = 3;
//
//    // Apply 2-opt move (intensification)
//    if (perturbationChoice == 1 || perturbationChoice == 2)
//    {
//        std::uniform_int_distribution<int> moveCountDist(3, 5);                  // Number of times to perform 2-opt moves
//        std::uniform_int_distribution<int> blockSizeDist(2, std::min(5, n / 2)); // Size of the block to reverse
//
//        int numMoves = moveCountDist(rng);
//
//        for (int move = 0; move < numMoves; ++move)
//        {
//            int blockSize = blockSizeDist(rng);
//
//            if (blockSize >= n)
//                continue;
//
//            std::uniform_int_distribution<int> indexDist(0, n - blockSize - 1);
//            int i = indexDist(rng);
//            int j = i + blockSize - 1;
//
//            // Reverse the block between i and j
//            std::reverse(schedule.begin() + i, schedule.begin() + j + 1);
//        }
//    }
//
//    // Apply Double Bridge move (diversification)
//    if (perturbationChoice == 2 || perturbationChoice == 3)
//    {
//        int segmentSize = n / 4;
//        if (segmentSize < 2)
//            return;
//
//        // Choose four segments for the double bridge move
//        std::uniform_int_distribution<int> posDist(1, n - 1);
//        int pos1 = posDist(rng) % (n - 3 * segmentSize);
//        int pos2 = pos1 + segmentSize;
//        int pos3 = pos2 + segmentSize;
//        int pos4 = pos3 + segmentSize;
//
//        // Adjust if boundaries are exceeded
//        if (pos4 >= n)
//        {
//            pos1 = 0;
//            pos2 = pos1 + segmentSize;
//            pos3 = pos2 + segmentSize;
//            pos4 = pos3 + segmentSize;
//        }
//
//        // Extract the four segments
//        std::vector<int> part1(schedule.begin() + pos1, schedule.begin() + pos2);
//        std::vector<int> part2(schedule.begin() + pos2, schedule.begin() + pos3);
//        std::vector<int> part3(schedule.begin() + pos3, schedule.begin() + pos4);
//
//        // Remove the four segments from the schedule
//        schedule.erase(schedule.begin() + pos1, schedule.begin() + pos4);
//
//        // Reinsert the segments in a new order: part3, part1, part2
//        schedule.insert(schedule.begin() + pos1, part3.begin(), part3.end());
//        schedule.insert(schedule.begin() + pos1 + part3.size(), part1.begin(), part1.end());
//        schedule.insert(schedule.begin() + pos1 + part3.size() + part1.size(), part2.begin(), part2.end());
//    }
//
//    // Additional random shuffling of a block (further diversification)
//    if (perturbationChoice == 3)
//    {
//        std::uniform_int_distribution<int> blockSizeDist(3, std::min(7, n / 3));
//        int blockSize = blockSizeDist(rng);
//
//        if (blockSize < n)
//        {
//            std::uniform_int_distribution<int> startIndexDist(0, n - blockSize - 1);
//            int startIdx = startIndexDist(rng);
//
//            // Shuffle the block of jobs
//            std::shuffle(schedule.begin() + startIdx, schedule.begin() + startIdx + blockSize, rng);
//        }
//    }
//}

void perturbSolution(std::vector<int>& schedule, int perturbationStrength){
    int n = schedule.size();
    if (n < 8) return; // Ensure there are enough jobs to perform significant perturbations

    std::mt19937 rng(std::random_device{}());

    // Apply Double Bridge move (diversification)
    int segmentSize = n / 4;
    if (segmentSize < 2) segmentSize = 2; // Ensure minimum segment size of 2

    // Choose positions for the four segments
    std::uniform_int_distribution<int> posDist(1, n - 3 * segmentSize - 1);
    int pos1 = posDist(rng);
    int pos2 = pos1 + segmentSize;
    int pos3 = pos2 + segmentSize;
    int pos4 = pos3 + segmentSize;

    // Adjust if boundaries are exceeded
    if (pos4 > n) {
        pos1 = 0;
        pos2 = pos1 + segmentSize;
        pos3 = pos2 + segmentSize;
        pos4 = n; // Ensure pos4 does not exceed n
    }

    // Extract the four segments
    std::vector<int> part1(schedule.begin() + pos1, schedule.begin() + pos2);
    std::vector<int> part2(schedule.begin() + pos2, schedule.begin() + pos3);
    std::vector<int> part3(schedule.begin() + pos3, schedule.begin() + pos4);

    // Remove the segments from the schedule
    schedule.erase(schedule.begin() + pos1, schedule.begin() + pos4);

    // Reinsert the segments in the new order: part3, part1, part2
    schedule.insert(schedule.begin() + pos1, part3.begin(), part3.end());
    schedule.insert(schedule.begin() + pos1 + part3.size(), part1.begin(), part1.end());
    schedule.insert(schedule.begin() + pos1 + part3.size() + part1.size(), part2.begin(), part2.end());
}


// std::vector<int> ILS(const std::vector<int>& initialSchedule, const std::vector<Order>& orders,
//                      const std::vector<std::vector<int>>& setupTimes,
//                      const std::vector<int>& initialSetupTimes) {
//     std::vector<int> bestSolution = initialSchedule;
//     double bestPenalty = calculateTotalPenalty(bestSolution, orders, setupTimes, initialSetupTimes);
//
//     std::vector<int> currentSolution = bestSolution;
//     int noImprovementCounter = 0;
//     int perturbationStrength = 1;
//
//     while (noImprovementCounter < MAX_NO_IMPROVEMENT_ITERATIONS) {
//         currentSolution = adaptiveRVND(currentSolution, orders, setupTimes, initialSetupTimes);
//         double currentPenalty = calculateTotalPenalty(currentSolution, orders, setupTimes, initialSetupTimes);
//
//         if (currentPenalty < bestPenalty - IMPROVEMENT_THRESHOLD) {
//             // Improvement found
//             bestSolution = currentSolution;
//             bestPenalty = currentPenalty;
//             noImprovementCounter = 0;
//             perturbationStrength = 1;  // Reset perturbation strength
//         } else {
//             // No improvement
//             noImprovementCounter++;
//             perturbationStrength = std::min(perturbationStrength + 1, PERTURBATION_STRENGTH_MAX);
//         }
//
//         // Perturb the current solution
//         perturbSolution(currentSolution, perturbationStrength);
//     }
//
//     return bestSolution;
// }
std::vector<int> ILS(const std::vector<int>& initialSchedule, const std::vector<Order>& orders,
                     const std::vector<std::vector<int>>& setupTimes,
                     const std::vector<int>& initialSetupTimes, double& currentPenaltyCost,
                     TabuList& tabuList) {
    std::vector<int> bestSolution = initialSchedule;
    double bestPenalty = currentPenaltyCost;

    std::vector<int> currentSolution = bestSolution;
    double currentPenalty = bestPenalty;
    int noImprovementCounter = 0;
    int perturbationStrength = 1;

    int iteration = 0;

    while (noImprovementCounter < MAX_NO_IMPROVEMENT_ITERATIONS) {
        iteration++;

        // Perform RVND local search
        currentSolution = adaptiveRVND(currentSolution, orders, setupTimes, initialSetupTimes, currentPenalty);

        size_t currentHash = computeScheduleHash(currentSolution);

        std::cout << "Iteration: " << iteration
                  << ", Current Penalty: " << currentPenalty
                  << ", Best Penalty: " << bestPenalty
                  << ", No Improvement Counter: " << noImprovementCounter << std::endl;

        if (currentPenalty < bestPenalty) {
            // Improvement found
            bestSolution = currentSolution;
            bestPenalty = currentPenalty;
            noImprovementCounter = 0;
            perturbationStrength = 1;
            // Clear the Tabu List when an improving solution is found
            tabuList.clear();
        } else if (currentPenalty == bestPenalty) {
            if (!tabuList.isTabu(currentHash)) {
                // Not in Tabu List, accept the solution
                bestSolution = currentSolution;
                bestPenalty = currentPenalty;
                noImprovementCounter = 0;
                perturbationStrength = 1;
                // Add to Tabu List
                tabuList.addSolution(currentHash);
            } else {
                // Solution is in Tabu List, perturb the solution
                while (tabuList.isTabu(currentHash) && currentPenalty == bestPenalty) {
                    // Perturb the current solution
                    perturbSolution(currentSolution, perturbationStrength);
                    currentPenalty = calculateTotalPenalty(currentSolution, orders, setupTimes, initialSetupTimes);
                    currentHash = computeScheduleHash(currentSolution);
                }
                perturbationStrength = std::min(perturbationStrength + 1, PERTURBATION_STRENGTH_MAX);
                continue;
            }
        } else {
            // Worse solution, increase no improvement counter
            noImprovementCounter++;
            perturbationStrength = std::min(perturbationStrength + 1, PERTURBATION_STRENGTH_MAX);
        }

        // Perturb the current solution
        perturbSolution(currentSolution, perturbationStrength);
        currentPenalty = calculateTotalPenalty(currentSolution, orders, setupTimes, initialSetupTimes);
    }

    currentPenaltyCost = bestPenalty;
    return bestSolution;
}
