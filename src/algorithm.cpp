#include "algorithm.h"
#include <algorithm>
#include <vector>
#include <iostream>

// Function to calculate total cost (penalties + setup times) and total penalty cost
double calculateTotalCost(const std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double& totalPenaltyCost) {
    double totalCost = 0.0;
    totalPenaltyCost = 0.0;
    int currentTime = 0;
    int currentTask = -1;

    for (int i = 0; i < schedule.size(); ++i) {
        int taskId = schedule[i];
        const Order& order = orders[taskId];

        if (currentTask >= 0) {
            totalCost += setupTimes[currentTask][taskId];
            currentTime += setupTimes[currentTask][taskId];
        }

        currentTime += order.processingTime;

        if (currentTime > order.dueTime) {
            double penalty = order.penaltyRate * (currentTime - order.dueTime);
            totalPenaltyCost += penalty;
            totalCost += penalty;
        }

        currentTask = taskId;
    }

    return totalCost;
}


// Calculates initial setup time weight based on averages
double calculateInitialWeight(const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes) {
    int n = orders.size();

    double avgPenalty = 0.0;
    for (const auto& order : orders) {
        avgPenalty += order.penaltyRate;
    }
    avgPenalty /= n;

    double avgProcessingTime = 0.0;
    for (const auto& order : orders) {
        avgProcessingTime += order.processingTime;
    }
    avgProcessingTime /= n;

    double avgSetupTime = 0.0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                avgSetupTime += setupTimes[i][j];
            }
        }
    }
    avgSetupTime /= (n * (n - 1));

    return (avgPenalty / avgSetupTime) * (avgProcessingTime / avgSetupTime);
}

double calculateDynamicWeight(int tasksCompleted, int totalTasks, double initialWeight, double finalWeight) {
    return initialWeight + (finalWeight - initialWeight) * (static_cast<double>(tasksCompleted) / totalTasks);
}

double calculatePriority(const Order& order, int currentTime, int currentTask, const std::vector<std::vector<int>>& setupTimes, double dynamicSetupTimeWeight) {
    int timeLeft = order.dueTime - currentTime;
    if (timeLeft <= 0) {
        timeLeft = 1;
    }

    int setupTime = (currentTask >= 0) ? setupTimes[currentTask][order.id] : 0;

    return (static_cast<double>(order.penaltyRate) / timeLeft) * (1.0 / order.processingTime) + dynamicSetupTimeWeight * setupTime;
}

// Advanced Greedy Algorithm using averages for initial weight and dynamic adjustment
// It calculates total cost (penalty + setup time) as it builds the schedule
std::vector<int> advancedGreedyAlgorithmWithDynamicWeight(const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double finalSetupTimeWeight, double& totalPenaltyCost, double& totalCost) {
    std::vector<int> schedule;
    std::vector<Order> remainingOrders = orders;
    int currentTime = 0; // Start time of the schedule
    int currentTask = -1; // No task initially
    int totalTasks = orders.size();

    // Initialize cost variables
    totalPenaltyCost = 0.0;
    totalCost = 0.0;

    // Step 1: Calculate the initial setup time weight based on averages
    double initialSetupTimeWeight = calculateInitialWeight(orders, setupTimes);

    for (int tasksCompleted = 0; tasksCompleted < totalTasks; ++tasksCompleted) {
        // Step 2: Dynamically adjust setup time weight based on progress
        double dynamicSetupTimeWeight = calculateDynamicWeight(tasksCompleted, totalTasks, initialSetupTimeWeight, finalSetupTimeWeight);

        // Find the task with the highest priority
        auto bestOrder = remainingOrders.end();
        double bestPriority = -1e9;

        for (auto it = remainingOrders.begin(); it != remainingOrders.end(); ++it) {
            double priority = calculatePriority(*it, currentTime, currentTask, setupTimes, dynamicSetupTimeWeight);
            if (priority > bestPriority) {
                bestPriority = priority;
                bestOrder = it;
            }
        }

        // Add the best order to the schedule
        Order chosenOrder = *bestOrder;
        schedule.push_back(chosenOrder.id);

        // Update the current time and calculate setup time cost
        if (currentTask >= 0) {
            double setupTime = setupTimes[currentTask][chosenOrder.id];
            totalCost += setupTime;
            currentTime += setupTime;
        }

        // Add processing time
        currentTime += chosenOrder.processingTime;

        // Calculate penalty cost if the task is completed after due time
        if (currentTime > chosenOrder.dueTime) {
            double penalty = chosenOrder.penaltyRate * (currentTime - chosenOrder.dueTime);
            totalPenaltyCost += penalty;
            totalCost += penalty;  // Add penalty to total cost
        }

        // Remove the chosen order from the remaining orders
        remainingOrders.erase(bestOrder);

        // Update the current task
        currentTask = chosenOrder.id;
    }

    return schedule;
}
