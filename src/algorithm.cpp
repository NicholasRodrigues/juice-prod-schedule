#include "algorithm.h"
#include "neighborhoods.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <functional>

// Calculate total cost and total penalty cost for a given schedule
void calculateTotalCost(const std::vector<int> &schedule, const std::vector<Order> &orders,
                        const std::vector<std::vector<int>> &setupTimes, double &totalCost, double &totalPenaltyCost)
{
    totalCost = 0.0;
    totalPenaltyCost = 0.0;
    int currentTime = 0;
    int currentTask = -1;

    for (size_t i = 0; i < schedule.size(); ++i)
    {
        int taskId = schedule[i];
        const Order &order = orders[taskId];

        // Accumulate setup times
        if (currentTask >= 0)
        {
            totalCost += setupTimes[currentTask][taskId];
            currentTime += setupTimes[currentTask][taskId];
        }

        // Accumulate processing time
        currentTime += order.processingTime;

        // Calculate penalty if the task is late
        if (currentTime > order.dueTime)
        {
            double penalty = order.penaltyRate * (currentTime - order.dueTime);
            totalPenaltyCost += penalty;
            totalCost += penalty;
        }

        currentTask = taskId;
    }
}

// Shuffle neighborhoods based on their weights
void adaptiveShuffle(std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                                    const std::vector<std::vector<int>> &, double &)>> &neighborhoods,
                     std::vector<double> &neighborhoodWeights, std::mt19937 &g)
{
    if (neighborhoods.empty() || neighborhoodWeights.empty())
    {
        std::cerr << "Error: Neighborhoods or weights are empty!" << std::endl;
        return;
    }

    // Normalize the weights
    double totalWeight = std::accumulate(neighborhoodWeights.begin(), neighborhoodWeights.end(), 0.0);
    std::vector<double> probabilities;
    for (double weight : neighborhoodWeights)
    {
        probabilities.push_back(weight / totalWeight);
    }

    // Create a distribution based on normalized weights
    std::discrete_distribution<> dist(probabilities.begin(), probabilities.end());

    // Shuffle the neighborhoods based on the distribution
    std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                   const std::vector<std::vector<int>> &, double &)>>
        shuffledNeighborhoods;

    // Ensure that all neighborhoods are considered
    std::vector<int> indices(neighborhoods.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), g);

    for (int idx : indices)
    {
        shuffledNeighborhoods.push_back(neighborhoods[idx]);
    }

    neighborhoods = shuffledNeighborhoods;
}

// RVND Algorithm
std::vector<int> RVND(std::vector<int> &schedule, const std::vector<Order> &orders,
                      const std::vector<std::vector<int>> &setupTimes, double &totalCost)
{
    std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                   const std::vector<std::vector<int>> &, double &)>>
        neighborhoods = {
            swapNeighborhood,
            twoOptNeighborhood,
            reinsertionNeighborhood,
            orOptNeighborhood};

    std::vector<double> neighborhoodWeights(neighborhoods.size(), 1.0);
    bool improvement = true;
    int noImprovementCount = 0;

    std::mt19937 g(RANDOM_SEED);

    while (improvement && noImprovementCount < MAX_NO_IMPROVEMENT_ITERATIONS)
    {
        adaptiveShuffle(neighborhoods, neighborhoodWeights, g);
        improvement = false;

        for (size_t i = 0; i < neighborhoods.size(); ++i)
        {
            double previousCost = totalCost;
            bool neighborhoodImprovement = neighborhoods[i](schedule, orders, setupTimes, totalCost);

            if (neighborhoodImprovement && totalCost < previousCost - IMPROVEMENT_THRESHOLD)
            {
                improvement = true;
                neighborhoodWeights[i] += 1.0;
                noImprovementCount = 0;
                // Lets reshuffle the neighbors
                break;
            }
            else
            {
                neighborhoodWeights[i] = std::max(0.1, neighborhoodWeights[i] * 0.9);
            }
        }

        if (!improvement)
        {
            noImprovementCount++;
        }
    }

    return schedule;
}

// Calculates initial setup time weight based on averages
double calculateInitialWeight(const std::vector<Order> &orders, const std::vector<std::vector<int>> &setupTimes)
{
    int n = orders.size();

    double avgPenalty = 0.0;
    for (const auto &order : orders)
    {
        avgPenalty += order.penaltyRate;
    }
    avgPenalty /= n;

    double avgProcessingTime = 0.0;
    for (const auto &order : orders)
    {
        avgProcessingTime += order.processingTime;
    }
    avgProcessingTime /= n;

    double avgSetupTime = 0.0;
    int count = 0;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            if (i != j)
            {
                avgSetupTime += setupTimes[i][j];
                count++;
            }
        }
    }
    avgSetupTime /= count;

    return (avgPenalty / avgSetupTime) * (avgProcessingTime / avgSetupTime);
}

double calculateDynamicWeight(int tasksCompleted, int totalTasks, double initialWeight, double finalWeight)
{
    return initialWeight + (finalWeight - initialWeight) * (static_cast<double>(tasksCompleted) / totalTasks);
}

double calculatePriority(const Order &order, int currentTime, int currentTask,
                         const std::vector<std::vector<int>> &setupTimes, double dynamicSetupTimeWeight)
{
    int timeLeft = order.dueTime - currentTime;
    if (timeLeft <= 0)
    {
        timeLeft = 1;
    }

    int setupTime = (currentTask >= 0) ? setupTimes[currentTask][order.id] : 0;

    // Look-ahead to anticipate future penalties
    double estimatedFuturePenalty = (order.penaltyRate * std::max(0, (currentTime + setupTime + order.processingTime) - order.dueTime));

    double priority = -(dynamicSetupTimeWeight * setupTime + estimatedFuturePenalty);

    return priority;
}

std::vector<int> advancedGreedyAlgorithmWithDynamicWeight(const std::vector<Order> &orders,
                                                          const std::vector<std::vector<int>> &setupTimes,
                                                          double finalSetupTimeWeight, double &totalPenaltyCost,
                                                          double &totalCost)
{
    std::vector<int> schedule;
    std::vector<bool> scheduled(orders.size(), false);
    int currentTime = 0;
    int currentTask = -1;
    int totalTasks = orders.size();

    totalPenaltyCost = 0.0;
    totalCost = 0.0;

    double initialSetupTimeWeight = calculateInitialWeight(orders, setupTimes);

    for (int tasksCompleted = 0; tasksCompleted < totalTasks; ++tasksCompleted)
    {
        double dynamicSetupTimeWeight = calculateDynamicWeight(tasksCompleted, totalTasks,
                                                               initialSetupTimeWeight, finalSetupTimeWeight);

        int bestOrderIndex = -1;
        double bestPriority = -std::numeric_limits<double>::infinity();

        for (int idx = 0; idx < orders.size(); ++idx)
        {
            if (scheduled[idx])
                continue;

            const Order &order = orders[idx];
            double priority = calculatePriority(order, currentTime, currentTask, setupTimes, dynamicSetupTimeWeight);
            if (priority > bestPriority)
            {
                bestPriority = priority;
                bestOrderIndex = idx;
            }
        }

        if (bestOrderIndex == -1)
        {
            std::cerr << "Error: No unscheduled orders remaining!" << std::endl;
            break;
        }

        const Order &chosenOrder = orders[bestOrderIndex];
        schedule.push_back(chosenOrder.id);
        scheduled[bestOrderIndex] = true;

        if (currentTask >= 0)
        {
            currentTime += setupTimes[currentTask][chosenOrder.id];
            totalCost += setupTimes[currentTask][chosenOrder.id];
        }
        currentTime += chosenOrder.processingTime;

        if (currentTime > chosenOrder.dueTime)
        {
            double penalty = chosenOrder.penaltyRate * (currentTime - chosenOrder.dueTime);
            totalPenaltyCost += penalty;
            totalCost += penalty;
        }

        currentTask = chosenOrder.id;
    }

    return schedule;
}
