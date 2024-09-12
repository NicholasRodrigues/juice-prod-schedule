#include "algorithm.h"
#include <algorithm>
#include <vector>

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
std::vector<int> advancedGreedyAlgorithmWithDynamicWeight(const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double finalSetupTimeWeight) {
    std::vector<int> schedule;
    std::vector<Order> remainingOrders = orders;
    int currentTime = 0; // Start time of the schedule
    int currentTask = -1; // No task initially
    int totalTasks = orders.size();

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

        // Update the current time (including processing time and setup time)
        if (currentTask >= 0) {
            currentTime += setupTimes[currentTask][chosenOrder.id];
        }
        currentTime += chosenOrder.processingTime;

        remainingOrders.erase(bestOrder);

        currentTask = chosenOrder.id;
    }

    return schedule;
}
