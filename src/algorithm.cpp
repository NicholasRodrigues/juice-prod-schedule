#include "algorithm.h"
#include "neighborhoods.h"
#include <iostream>
#include <chrono>

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

// Greedy Algorithm using Earliest Due Date (EDD) heuristic
std::vector<int> greedyAlgorithm(const std::vector<Order> &orders,
                                 const std::vector<std::vector<int>> &setupTimes,
                                 const std::vector<int> &initialSetupTimes,
                                 double &totalPenaltyCost)
{
    int n = orders.size();
    std::vector<int> schedule;
    int currentTime = 0;
    int currentTask = -1;

    totalPenaltyCost = 0.0;

    // Sort orders based on Earliest Due Date (EDD)
    std::vector<Order> sortedOrders = orders;
    std::sort(sortedOrders.begin(), sortedOrders.end(), [](const Order &a, const Order &b)
              { return a.dueTime < b.dueTime; });

    for (const auto &order : sortedOrders)
    {
        int taskId = order.id;

        int setupTime = (currentTask >= 0) ? setupTimes[currentTask][taskId] : initialSetupTimes[taskId];

        currentTime += setupTime + order.processingTime;

        if (currentTime > order.dueTime)
        {
            double penalty = order.penaltyRate * (currentTime - order.dueTime);
            totalPenaltyCost += penalty;
        }

        schedule.push_back(taskId);
        currentTask = taskId;
    }

    return schedule;
}

std::vector<int> GRASP(const std::vector<Order> &orders,
                       const std::vector<std::vector<int>> &setupTimes,
                       const std::vector<int> &initialSetupTimes,
                       double &totalPenaltyCost)
{

    // Steps:
    // 1. Generate a random greedy solution
    // 2. Use local search with AdaptiveRVND
    // 3. Repeat steps 1 and 2 until stopping criteria is met
    // 4. Save the best solution

    int maxIterations = GRASP_ITERATIONS;
    std::vector<int> bestSolution = greedyAlgorithm(orders, setupTimes, initialSetupTimes, totalPenaltyCost);
    std::vector<int> currentSolution = bestSolution;

    double bestPenaltyCost = calculateTotalPenalty(bestSolution, orders, setupTimes, initialSetupTimes);

    int rclSize = RCL_SIZE;

    for (int i = 0; i < maxIterations; i++)
    {
        std::vector<int> newSchedule;

        // Sort orders based on Earliest Due Date (EDD)
        std::vector<Order> sortedOrders = orders;
        std::sort(sortedOrders.begin(), sortedOrders.end(), [](const Order &a, const Order &b)
                  { return a.dueTime < b.dueTime; });

        double iterationPenaltyCost = 0;
        int currentTime = 0;
        int currentTask = -1;

        while (newSchedule.size() < orders.size())
        {
            std::vector<Order> bestOrders;
            for (int j = 0; j < rclSize; j++)
            {
                bestOrders.push_back(sortedOrders[j]);
            }

            // Random select one order from the best orders
            std::mt19937 rng(std::random_device{}());
            std::uniform_int_distribution<int> distribution(0, bestOrders.size() - 1);
            int index = distribution(rng);

            Order selectedOrder = bestOrders[index];

            int setupTime = (currentTask >= 0) ? setupTimes[currentTask][selectedOrder.id] : initialSetupTimes[selectedOrder.id];

            currentTime += setupTime + selectedOrder.processingTime;

            if (currentTime > selectedOrder.dueTime)
            {
                double penalty = selectedOrder.penaltyRate * (currentTime - selectedOrder.dueTime);
                iterationPenaltyCost += penalty;
            }

            newSchedule.push_back(selectedOrder.id);

            // Remove the selected order from the sorted orders
            sortedOrders.erase(sortedOrders.begin() + index);
        }

        if (iterationPenaltyCost < bestPenaltyCost)
        {
            bestSolution = newSchedule;
            bestPenaltyCost = iterationPenaltyCost;
        }
    }

    totalPenaltyCost = bestPenaltyCost;

    return bestSolution;
}

std::vector<int> adaptiveRVND(std::vector<int> &schedule, const std::vector<Order> &orders,
                              const std::vector<std::vector<int>> &setupTimes,
                              const std::vector<int> &initialSetupTimes)
{
    std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                   const std::vector<std::vector<int>> &,
                                   const std::vector<int> &)>>
        neighborhoods = {
            reinsertionNeighborhood,
            twoOptNeighborhood,
            swapNeighborhood};

    bool improvement = true;
    std::mt19937 rng(std::random_device{}());

    while (improvement)
    {
        improvement = false;

        std::shuffle(neighborhoods.begin(), neighborhoods.end(), rng);

        for (size_t i = 0; i < neighborhoods.size(); ++i)
        {
            bool neighborhoodImprovement = neighborhoods[i](schedule, orders, setupTimes, initialSetupTimes);
            if (neighborhoodImprovement)
            {
                improvement = true;
                break;
            }
        }
    }

    return schedule;
}

// std::vector<int> adaptiveRVND(std::vector<int>& schedule, const std::vector<Order>& orders,
//                               const std::vector<std::vector<int>>& setupTimes,
//                               const std::vector<int>& initialSetupTimes) {
//     std::vector<std::function<bool(std::vector<int>&, const std::vector<Order>&,
//             const std::vector<std::vector<int>>&,
//             const std::vector<int>&)> > neighborhoods = {
//         reinsertionNeighborhood,
//         twoOptNeighborhood,
//         swapNeighborhood
//     };
//
//     std::vector<double> neighborhoodWeights(neighborhoods.size(), 1.0);
//     std::mt19937 rng(std::random_device{}());
//
//     bool improvement = true;
//     int noImprovementCount = 0;
//
//     while (improvement) {
//         adaptiveShuffle(neighborhoods, neighborhoodWeights, rng);
//
//         improvement = false;
//         for (size_t i = 0; i < neighborhoods.size(); ++i) {
//             double previousPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);
//             bool neighborhoodImprovement = neighborhoods[i](schedule, orders, setupTimes, initialSetupTimes);
//
//             double currentPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);
//
//             if (neighborhoodImprovement && currentPenalty < previousPenalty - IMPROVEMENT_THRESHOLD) {
//                 improvement = true;
//                 neighborhoodWeights[i] += 1.0;
//                 noImprovementCount = 0;
//                 break;  // Restart with reshuffled neighborhoods
//             } else {
//                 neighborhoodWeights[i] = std::max(0.1, neighborhoodWeights[i] * 0.9);
//             }
//         }
//
//         if (!improvement) {
//             noImprovementCount++;
//         }
//     }
//
//     return schedule;
// }

void adaptiveShuffle(std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                                    const std::vector<std::vector<int>> &,
                                                    const std::vector<int> &)>> &neighborhoods,
                     std::vector<double> &neighborhoodWeights, std::mt19937 &g)
{
    if (neighborhoods.empty() || neighborhoodWeights.empty())
    {
        std::cerr << "Error: Neighborhoods or weights are empty!" << std::endl;
        return;
    }

    double totalWeight = std::accumulate(neighborhoodWeights.begin(), neighborhoodWeights.end(), 0.0);
    std::vector<double> probabilities;
    for (double weight : neighborhoodWeights)
    {
        probabilities.push_back(weight / totalWeight);
    }

    std::discrete_distribution<> dist(probabilities.begin(), probabilities.end());

    std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                   const std::vector<std::vector<int>> &,
                                   const std::vector<int> &)>>
        shuffledNeighborhoods;

    std::vector<int> indices(neighborhoods.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), g);

    for (int idx : indices)
    {
        shuffledNeighborhoods.push_back(neighborhoods[idx]);
    }

    neighborhoods = shuffledNeighborhoods;
}

// Perturbation function for ILS
void perturbSolution(std::vector<int> &schedule, int perturbationStrength)
{
    int n = schedule.size();
    if (n < 8)
        return;

    std::mt19937 rng(std::random_device{}());

    std::uniform_int_distribution<int> perturbationChoiceDist(1, 3); // 1: 2-opt, 2: double bridge, 3: both
    int perturbationChoice = perturbationChoiceDist(rng);

    if (perturbationChoice == 1 || perturbationChoice == 3)
    {
        std::uniform_int_distribution<int> moveCountDist(3, 5);
        std::uniform_int_distribution<int> blockSizeDist(2, std::min(5, n / 2));

        int numMoves = moveCountDist(rng);

        for (int move = 0; move < numMoves; ++move)
        {
            int blockSize = blockSizeDist(rng);

            if (blockSize >= n)
                continue;

            std::uniform_int_distribution<int> indexDist(0, n - blockSize - 1);
            int i = indexDist(rng);
            int j = i + blockSize - 1;

            std::reverse(schedule.begin() + i, schedule.begin() + j + 1);
        }
    }

    if (perturbationChoice == 2 || perturbationChoice == 3)
    {
        int segmentSize = n / 4;
        std::uniform_int_distribution<int> posDist(1, n - 1);

        int pos1 = posDist(rng) % (n - 3 * segmentSize);
        int pos2 = pos1 + segmentSize;
        int pos3 = pos2 + segmentSize;
        int pos4 = pos3 + segmentSize;

        if (pos4 >= n)
        {
            pos1 = 0;
            pos2 = pos1 + segmentSize;
            pos3 = pos2 + segmentSize;
            pos4 = pos3 + segmentSize;
        }

        std::vector<int> part1(schedule.begin() + pos1, schedule.begin() + pos2);
        std::vector<int> part2(schedule.begin() + pos2, schedule.begin() + pos3);
        std::vector<int> part3(schedule.begin() + pos3, schedule.begin() + pos4);

        schedule.erase(schedule.begin() + pos1, schedule.begin() + pos4);

        schedule.insert(schedule.begin() + pos1, part3.begin(), part3.end());
        schedule.insert(schedule.begin() + pos1 + part3.size(), part1.begin(), part1.end());
        schedule.insert(schedule.begin() + pos1 + part3.size() + part1.size(), part2.begin(), part2.end());
    }
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

std::vector<int> ILS(const std::vector<int> &initialSchedule, const std::vector<Order> &orders,
                     const std::vector<std::vector<int>> &setupTimes,
                     const std::vector<int> &initialSetupTimes)
{
    std::vector<int> bestSolution = initialSchedule;
    double bestPenalty = calculateTotalPenalty(bestSolution, orders, setupTimes, initialSetupTimes);

    std::vector<int> currentSolution = bestSolution;
    int noImprovementCounter = 0;
    int perturbationStrength = 1;

    int iteration = 0;

    while (noImprovementCounter < MAX_NO_IMPROVEMENT_ITERATIONS)
    {
        iteration++;

        // Apply VND
        currentSolution = adaptiveRVND(currentSolution, orders, setupTimes, initialSetupTimes);
        double currentPenalty = calculateTotalPenalty(currentSolution, orders, setupTimes, initialSetupTimes);

        std::cout << "Iteration: " << iteration
                  << ", Current Penalty: " << currentPenalty
                  << ", Best Penalty: " << bestPenalty
                  << ", No Improvement Counter: " << noImprovementCounter << std::endl;

        if (currentPenalty + IMPROVEMENT_THRESHOLD < bestPenalty)
        {
            // Improvement found
            bestSolution = currentSolution;
            bestPenalty = currentPenalty;
            noImprovementCounter = 0;
            perturbationStrength = 1; // Reset perturbation strength
        }
        else
        {
            // No significant improvement
            noImprovementCounter++;
            perturbationStrength = std::min(perturbationStrength + 1, PERTURBATION_STRENGTH_MAX);
        }

        // Perturb the current solution
        perturbSolution(currentSolution, perturbationStrength);
    }

    return bestSolution;
}