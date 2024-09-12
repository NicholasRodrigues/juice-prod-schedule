#include "neighborhoods.h"
#include <algorithm>
#include <iostream>

// Utility to calculate the cost difference for a swap operation
double calculateSwapCostDiff(int i, int j, std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double currentCost) {
    int task_i = schedule[i];
    int task_j = schedule[j];

    double oldCost = currentCost;

    // Remove setup costs around i and j
    if (i > 0) {
        oldCost -= setupTimes[schedule[i - 1]][task_i];
        oldCost += setupTimes[schedule[i - 1]][task_j];
    }
    if (i < schedule.size() - 1) {
        oldCost -= setupTimes[task_i][schedule[i + 1]];
        oldCost += setupTimes[task_j][schedule[i + 1]];
    }
    if (j > 0) {
        oldCost -= setupTimes[schedule[j - 1]][task_j];
        oldCost += setupTimes[schedule[j - 1]][task_i];
    }
    if (j < schedule.size() - 1) {
        oldCost -= setupTimes[task_j][schedule[j + 1]];
        oldCost += setupTimes[task_i][schedule[j + 1]];
    }

    // Adjust penalties for task i and task j
    int currentTime = 0;
    for (int k = 0; k < schedule.size(); ++k) {
        int task = schedule[k];
        currentTime += orders[task].processingTime;

        if (currentTime > orders[task].dueTime) {
            double penalty = orders[task].penaltyRate * (currentTime - orders[task].dueTime);
            if (k == i || k == j) {
                oldCost -= penalty;
            }
            oldCost += penalty;
        }
    }

    return oldCost;
}

// Swap neighborhood function with dynamic cost update
bool swapNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double& totalCost) {
    bool improvementFound = false;
    double bestCost = totalCost;

    for (int i = 0; i < schedule.size() - 1; ++i) {
        for (int j = i + 1; j < schedule.size(); ++j) {

            double newCost = calculateSwapCostDiff(i, j, schedule, orders, setupTimes, totalCost);

            if (newCost < bestCost) {
                bestCost = newCost;
                std::swap(schedule[i], schedule[j]);
                improvementFound = true;
                totalCost = newCost;
            }
        }
    }


    return improvementFound;
}

// Utility to calculate cost difference for a 2-opt operation
double calculateTwoOptCostDiff(int i, int j, std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double currentCost) {
    double oldCost = currentCost;

    // Remove the setup cost for the original sequence (i to j)
    if (i > 0) {
        oldCost -= setupTimes[schedule[i - 1]][schedule[i]];
    }
    if (j < schedule.size() - 1) {
        oldCost -= setupTimes[schedule[j]][schedule[j + 1]];
    }

    // Apply the 2-opt move (reverse the segment between i and j)
    std::reverse(schedule.begin() + i, schedule.begin() + j + 1);

    // Calculate the new setup cost
    if (i > 0) {
        oldCost += setupTimes[schedule[i - 1]][schedule[i]];
    }
    if (j < schedule.size() - 1) {
        oldCost += setupTimes[schedule[j]][schedule[j + 1]];
    }

    return oldCost;
}

// 2-opt neighborhood function with dynamic cost update
bool twoOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double& totalCost) {
    bool improvementFound = false;
    double bestCost = totalCost;

    for (int i = 0; i < schedule.size() - 1; ++i) {
        for (int j = i + 1; j < schedule.size(); ++j) {
            double newCost = calculateTwoOptCostDiff(i, j, schedule, orders, setupTimes, totalCost);

            if (newCost < bestCost) {
                bestCost = newCost;
                improvementFound = true;
                totalCost = newCost;
            }
            std::reverse(schedule.begin() + i, schedule.begin() + j + 1);  // Revert the move for further iterations
        }
    }

    return improvementFound;
}

double calculateReinsertionCostDiff(int i, int newPos, std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double currentCost) {
    double oldCost = currentCost;


    // Ensure the indices are valid
    if (schedule.empty() || i >= schedule.size() || i < 0 || newPos < 0 || newPos > schedule.size()) {
        std::cerr << "Error: Invalid index in calculateReinsertionCostDiff! i: " << i << ", newPos: " << newPos << std::endl;
        return oldCost; // Return current cost if indices are invalid
    }

    // Extract the task at position i
    int task = schedule[i];

    // Remove the original setup costs around the task
    if (i > 0) {
        oldCost -= setupTimes[schedule[i - 1]][task];
    }
    if (i < schedule.size() - 1) {
        oldCost -= setupTimes[task][schedule[i + 1]];
    }

    // Remove the task from its current position
    schedule.erase(schedule.begin() + i);

    // Adjust newPos if necessary (because of the erasure)
    if (newPos > i) {
        newPos--;
    }

    // Insert the task in the new position
    if (newPos >= 0 && newPos <= schedule.size()) {
        schedule.insert(schedule.begin() + newPos, task);
    } else {
        std::cerr << "Error: newPos is out of bounds! newPos: " << newPos << std::endl;
        return oldCost;
    }

    // Add the new setup costs around the task
    if (newPos > 0) {
        oldCost += setupTimes[schedule[newPos - 1]][task];
    }
    if (newPos < schedule.size() - 1) {
        oldCost += setupTimes[task][schedule[newPos + 1]];
    }

    return oldCost;
}


// Reinsertion neighborhood function with dynamic cost update
bool reinsertionNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double& totalCost) {
    bool improvementFound = false;
    double bestCost = totalCost;


    for (int i = 0; i < schedule.size(); ++i) {
        for (int j = 0; j <= schedule.size(); ++j) {
            if (i == j) continue;  // Skip reinserting in the same place

            // Check that the schedule is valid before calling calculateReinsertionCostDiff
            if (schedule.size() < 2) {
                std::cerr << "Schedule too small to apply reinsertion!" << std::endl;
                continue;
            }


            // Call the cost calculation with proper boundary checking
            double newCost = calculateReinsertionCostDiff(i, j, schedule, orders, setupTimes, totalCost);

            if (newCost < bestCost) {
                bestCost = newCost;
                improvementFound = true;
                totalCost = newCost;
            }
        }
    }

    return improvementFound;
}

