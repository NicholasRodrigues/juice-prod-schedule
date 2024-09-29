#include "neighborhoods.h"
#include "algorithm.h"
#include <algorithm>
#include <vector>
#include <set>
#include <iostream>

// Define improvement threshold
#ifndef IMPROVEMENT_THRESHOLD
#define IMPROVEMENT_THRESHOLD 1e-6
#endif

// Helper Function to Calculate Penalty for a Single Task
double calculatePenalty(const std::vector<int>& schedule, const std::vector<Order>& orders,
                        const std::vector<std::vector<int>>& setupTimes, int idx) {
    int currentTime = 0;
    int prevTask = -1;
    for (int k = 0; k <= idx; ++k) {
        int taskId = schedule[k];
        if (prevTask != -1) {
            currentTime += setupTimes[prevTask][taskId];
        }
        currentTime += orders[taskId].processingTime;
        prevTask = taskId;
    }

    const Order& order = orders[schedule[idx]];
    if (currentTime > order.dueTime) {
        return order.penaltyRate * (currentTime - order.dueTime);
    } else {
        return 0.0;
    }
}

// Swap Neighborhood Function
bool swapNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                      const std::vector<std::vector<int>>& setupTimes, double& totalCost) {
    int n = schedule.size();

    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double deltaCost = calculateSwapDeltaCost(schedule, orders, setupTimes, i, j);

            if (deltaCost < -IMPROVEMENT_THRESHOLD) {
                // Apply the swap
                std::swap(schedule[i], schedule[j]);
                totalCost += deltaCost;

                // Ensure totalCost remains non-negative
                if (totalCost < 0.0) {
                    std::cerr << "Warning: Total cost became negative after swap. Resetting to zero." << std::endl;
                    totalCost = 0.0;
                }

                return true;  // Improvement found
            }
        }
    }

    return false;  // No improvement found
}

// Helper Function for Swap Neighborhood
double calculateSwapDeltaCost(const std::vector<int>& schedule, const std::vector<Order>& orders,
                              const std::vector<std::vector<int>>& setupTimes, int i, int j) {
    int n = schedule.size();

    // Get task IDs
    int task_i = schedule[i];
    int task_j = schedule[j];

    // Get predecessors and successors
    int prev_i = (i > 0) ? schedule[i - 1] : -1;
    int next_i = (i + 1 < n) ? schedule[i + 1] : -1;
    int prev_j = (j > 0) ? schedule[j - 1] : -1;
    int next_j = (j + 1 < n) ? schedule[j + 1] : -1;

    // Adjust for adjacent swaps
    if (i + 1 == j) {
        next_i = task_j;
        prev_j = task_i;
    }

    double deltaCost = 0.0;

    // Remove old setup times
    if (prev_i != -1) deltaCost -= setupTimes[prev_i][task_i];
    if (next_i != -1) deltaCost -= setupTimes[task_i][next_i];
    if (prev_j != -1) deltaCost -= setupTimes[prev_j][task_j];
    if (next_j != -1) deltaCost -= setupTimes[task_j][next_j];

    // Add new setup times
    if (prev_i != -1) deltaCost += setupTimes[prev_i][task_j];
    if (next_i != -1) deltaCost += setupTimes[task_j][next_i];
    if (prev_j != -1) deltaCost += setupTimes[prev_j][task_i];
    if (next_j != -1) deltaCost += setupTimes[task_i][next_j];

    // Calculate changes in penalties
    double oldPenalty_i = calculatePenalty(schedule, orders, setupTimes, i);
    double oldPenalty_j = calculatePenalty(schedule, orders, setupTimes, j);

    // Swap the tasks in a temporary schedule
    std::vector<int> newSchedule = schedule;
    std::swap(newSchedule[i], newSchedule[j]);

    // Calculate new penalties
    double newPenalty_i = calculatePenalty(newSchedule, orders, setupTimes, i);
    double newPenalty_j = calculatePenalty(newSchedule, orders, setupTimes, j);

    // Update deltaCost with penalty changes
    deltaCost += (newPenalty_i + newPenalty_j) - (oldPenalty_i + oldPenalty_j);

    return deltaCost;
}

// Two-Opt Neighborhood Function
bool twoOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                        const std::vector<std::vector<int>>& setupTimes, double& totalCost) {
    int n = schedule.size();

    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double deltaCost = calculateTwoOptDeltaCost(schedule, orders, setupTimes, i, j);

            if (deltaCost < -IMPROVEMENT_THRESHOLD) {
                // Perform the 2-opt move
                std::reverse(schedule.begin() + i, schedule.begin() + j + 1);
                totalCost += deltaCost;

                // Ensure totalCost remains non-negative
                if (totalCost < 0.0) {
                    std::cerr << "Warning: Total cost became negative after 2-opt. Resetting to zero." << std::endl;
                    totalCost = 0.0;
                }

                return true;  // Improvement found
            }
        }
    }

    return false;  // No improvement found
}

// Helper Function for Two-Opt Neighborhood
double calculateTwoOptDeltaCost(const std::vector<int>& schedule, const std::vector<Order>& orders,
                                const std::vector<std::vector<int>>& setupTimes, int i, int j) {
    int n = schedule.size();

    // Get predecessors and successors
    int prev_i = (i > 0) ? schedule[i - 1] : -1;
    int next_j = (j + 1 < n) ? schedule[j + 1] : -1;

    double deltaCost = 0.0;

    // Remove old setup times
    if (prev_i != -1) deltaCost -= setupTimes[prev_i][schedule[i]];
    if (j + 1 < n) deltaCost -= setupTimes[schedule[j]][next_j];

    // Add new setup times
    if (prev_i != -1) deltaCost += setupTimes[prev_i][schedule[j]];
    if (j + 1 < n) deltaCost += setupTimes[schedule[i]][next_j];

    // Calculate changes in penalties for positions i to j
    double oldPenalty = 0.0;
    double newPenalty = 0.0;
    int currentTime = 0;
    int prevTask = -1;

    // Compute current time up to position i
    for (int k = 0; k < i; ++k) {
        int taskId = schedule[k];
        if (k > 0) {
            currentTime += setupTimes[schedule[k - 1]][taskId];
        }
        currentTime += orders[taskId].processingTime;
        prevTask = taskId;
    }

    // Original penalties
    for (int k = i; k <= j; ++k) {
        int taskId = schedule[k];
        if (prevTask != -1) {
            currentTime += setupTimes[prevTask][taskId];
        }
        currentTime += orders[taskId].processingTime;

        if (currentTime > orders[taskId].dueTime) {
            oldPenalty += orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
        }

        prevTask = taskId;
    }

    // Simulate reversal in a temporary schedule
    std::vector<int> newSchedule = schedule;
    std::reverse(newSchedule.begin() + i, newSchedule.begin() + j + 1);

    // Calculate new penalties
    currentTime = 0;
    prevTask = -1;
    for (int k = 0; k < i; ++k) {
        int taskId = newSchedule[k];
        if (k > 0) {
            currentTime += setupTimes[newSchedule[k - 1]][taskId];
        }
        currentTime += orders[taskId].processingTime;
        prevTask = taskId;
    }

    for (int k = i; k <= j; ++k) {
        int taskId = newSchedule[k];
        if (prevTask != -1) {
            currentTime += setupTimes[prevTask][taskId];
        }
        currentTime += orders[taskId].processingTime;

        if (currentTime > orders[taskId].dueTime) {
            newPenalty += orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
        }

        prevTask = taskId;
    }

    // Update deltaCost with penalty changes
    deltaCost += (newPenalty - oldPenalty);

    return deltaCost;
}

// Reinsertion Neighborhood Function
bool reinsertionNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes, double& totalCost) {
    int n = schedule.size();

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j || i == j - 1) continue;

            double deltaCost = calculateReinsertionDeltaCost(schedule, orders, setupTimes, i, j);

            if (deltaCost < -IMPROVEMENT_THRESHOLD) {
                // Perform the reinsertion
                int task = schedule[i];
                schedule.erase(schedule.begin() + i);
                int insertPos = (j > i) ? j - 1 : j;
                schedule.insert(schedule.begin() + insertPos, task);
                totalCost += deltaCost;

                // Ensure totalCost remains non-negative
                if (totalCost < 0.0) {
                    std::cerr << "Warning: Total cost became negative after reinsertion. Resetting to zero." << std::endl;
                    totalCost = 0.0;
                }

                return true;  // Improvement found
            }
        }
    }

    return false;  // No improvement found
}

// Helper Function for Reinsertion Neighborhood
double calculateReinsertionDeltaCost(const std::vector<int>& schedule, const std::vector<Order>& orders,
                                     const std::vector<std::vector<int>>& setupTimes, int i, int j) {
    int n = schedule.size();

    int task = schedule[i];

    // Identify affected positions
    int prev_i = (i > 0) ? schedule[i - 1] : -1;
    int next_i = (i + 1 < n) ? schedule[i + 1] : -1;
    int insertPos = (j > i) ? j - 1 : j;
    int prev_j = (insertPos > 0) ? schedule[insertPos - 1] : -1;
    int next_j = (insertPos < n) ? schedule[insertPos] : -1;

    double deltaCost = 0.0;

    // Remove old setup times
    if (prev_i != -1) deltaCost -= setupTimes[prev_i][task];
    if (next_i != -1) deltaCost -= setupTimes[task][next_i];
    if (prev_i != -1 && next_i != -1) deltaCost += setupTimes[prev_i][next_i];

    // Add new setup times
    if (prev_j != -1) deltaCost += setupTimes[prev_j][task];
    if (next_j != -1) deltaCost += setupTimes[task][next_j];
    if (prev_j != -1 && next_j != -1) deltaCost -= setupTimes[prev_j][next_j];

    // Calculate changes in penalties for affected tasks
    std::vector<int> affectedIndices = {prev_i, task, next_i, prev_j, next_j};
    std::set<int> uniqueIndices;
    for (int idx : affectedIndices) {
        if (idx >= 0 && idx < n) uniqueIndices.insert(idx);
    }

    double oldPenalty = 0.0;
    double newPenalty = 0.0;

    // Calculate old penalties
    double currentTime = 0.0;
    int prevTask = -1;
    for (int k = 0; k < n; ++k) {
        int taskId = schedule[k];
        if (prevTask != -1) {
            currentTime += setupTimes[prevTask][taskId];
        }
        currentTime += orders[taskId].processingTime;

        if (uniqueIndices.count(taskId) > 0) {
            if (currentTime > orders[taskId].dueTime) {
                oldPenalty += orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            }
        }

        prevTask = taskId;
    }

    // Simulate the reinsertion in a temporary schedule
    std::vector<int> newSchedule = schedule;
    newSchedule.erase(newSchedule.begin() + i);
    newSchedule.insert(newSchedule.begin() + insertPos, task);

    // Calculate new penalties
    currentTime = 0.0;
    prevTask = -1;
    for (int k = 0; k < n; ++k) {
        int taskId = newSchedule[k];
        if (prevTask != -1) {
            currentTime += setupTimes[prevTask][taskId];
        }
        currentTime += orders[taskId].processingTime;

        if (uniqueIndices.count(taskId) > 0) {
            if (currentTime > orders[taskId].dueTime) {
                newPenalty += orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            }
        }

        prevTask = taskId;
    }

    // Update deltaCost with penalty changes
    deltaCost += (newPenalty - oldPenalty);

    return deltaCost;
}

// Or-Opt Neighborhood Function
bool orOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                       const std::vector<std::vector<int>>& setupTimes, double& totalCost) {
    int n = schedule.size();

    for (int l = 1; l <= n; ++l) {  // Or-Opt with sequences of length 1 to n
        for (int i = 0; i <= n - l; ++i) {
            for (int j = 0; j < n; ++j) {
                if (j >= i && j <= i + l - 1) continue;

                double deltaCost = calculateOrOptDeltaCost(schedule, orders, setupTimes, i, j, l);

                if (deltaCost < -IMPROVEMENT_THRESHOLD) {
                    // Perform the Or-Opt move
                    std::vector<int> sequence(schedule.begin() + i, schedule.begin() + i + l);
                    schedule.erase(schedule.begin() + i, schedule.begin() + i + l);
                    int insertPos = (j > i) ? j - l : j;
                    schedule.insert(schedule.begin() + insertPos, sequence.begin(), sequence.end());
                    totalCost += deltaCost;

                    // Ensure totalCost remains non-negative
                    if (totalCost < 0.0) {
                        std::cerr << "Warning: Total cost became negative after Or-Opt. Resetting to zero." << std::endl;
                        totalCost = 0.0;
                    }

                    return true;  // Improvement found
                }
            }
        }
    }

    return false;  // No improvement found
}

// Helper Function for Or-Opt Neighborhood
double calculateOrOptDeltaCost(const std::vector<int>& schedule, const std::vector<Order>& orders,
                               const std::vector<std::vector<int>>& setupTimes, int i, int j, int l) {
    int n = schedule.size();

    if (l <= 0 || i < 0 || j < 0 || i + l > n || j + l > n) {
        // Invalid parameters
        return 0.0;
    }

    // Identify sequence to move
    std::vector<int> sequence(schedule.begin() + i, schedule.begin() + i + l);

    // Identify affected positions
    int prev_i = (i > 0) ? schedule[i - 1] : -1;
    int next_i = (i + l < n) ? schedule[i + l] : -1;
    int insertPos = (j > i) ? j - l : j;
    int prev_j = (insertPos > 0) ? schedule[insertPos - 1] : -1;
    int next_j = (insertPos < n) ? schedule[insertPos] : -1;

    double deltaCost = 0.0;

    // Remove old setup times
    if (prev_i != -1) deltaCost -= setupTimes[prev_i][sequence[0]];
    if (next_i != -1) deltaCost -= setupTimes[sequence[l - 1]][next_i];
    if (prev_i != -1 && next_i != -1) deltaCost += setupTimes[prev_i][next_i];

    // Add new setup times
    if (prev_j != -1) deltaCost += setupTimes[prev_j][sequence[0]];
    if (next_j != -1) deltaCost += setupTimes[sequence[l - 1]][next_j];
    if (prev_j != -1 && next_j != -1) deltaCost -= setupTimes[prev_j][next_j];

    // Calculate changes in penalties for affected tasks
    std::vector<int> affectedIndices = {prev_i, sequence[0], sequence[l - 1], next_i, prev_j, next_j};
    std::set<int> uniqueIndices;
    for (int idx : affectedIndices) {
        if (idx >= 0 && idx < n) uniqueIndices.insert(idx);
    }

    double oldPenalty = 0.0;
    double newPenalty = 0.0;

    // Calculate old penalties
    double currentTime = 0.0;
    int prevTask = -1;
    for (int k = 0; k < n; ++k) {
        int taskId = schedule[k];
        if (prevTask != -1) {
            currentTime += setupTimes[prevTask][taskId];
        }
        currentTime += orders[taskId].processingTime;

        if (uniqueIndices.count(taskId) > 0) {
            if (currentTime > orders[taskId].dueTime) {
                oldPenalty += orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            }
        }

        prevTask = taskId;
    }

    // Simulate the Or-Opt move in a temporary schedule
    std::vector<int> newSchedule = schedule;
    newSchedule.erase(newSchedule.begin() + i, newSchedule.begin() + i + l);
    newSchedule.insert(newSchedule.begin() + insertPos, sequence.begin(), sequence.end());

    // Calculate new penalties
    currentTime = 0.0;
    prevTask = -1;
    for (int k = 0; k < n; ++k) {
        int taskId = newSchedule[k];
        if (prevTask != -1) {
            currentTime += setupTimes[prevTask][taskId];
        }
        currentTime += orders[taskId].processingTime;

        if (uniqueIndices.count(taskId) > 0) {
            if (currentTime > orders[taskId].dueTime) {
                newPenalty += orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            }
        }

        prevTask = taskId;
    }

    // Update deltaCost with penalty changes
    deltaCost += (newPenalty - oldPenalty);

    return deltaCost;
}
