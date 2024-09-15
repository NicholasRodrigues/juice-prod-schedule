#include "neighborhoods.h"
#include "algorithm.h"
#include <algorithm>

// Swap Neighborhood Function
bool swapNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                      const std::vector<std::vector<int>>& setupTimes, double& totalCost) {
    int n = schedule.size();

    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double deltaCost = calculateSwapCostDifference(schedule, orders, setupTimes, i, j);

            if (deltaCost < -IMPROVEMENT_THRESHOLD) {
                // Apply the swap
                std::swap(schedule[i], schedule[j]);
                totalCost += deltaCost;
                return true;  // Improvement found
            }
        }
    }

    return false;  // No improvement found
}

// Helper Function for Swap Neighborhood
double calculateSwapCostDifference(const std::vector<int>& schedule, const std::vector<Order>& orders,
                                   const std::vector<std::vector<int>>& setupTimes, int i, int j) {
    int n = schedule.size();

    // Determine the start position for recalculating cost
    int start = std::min(i, j);

    // Compute cumulative currentTime up to 'start'
    int currentTime = 0;
    int currentTask = -1;
    for (int k = 0; k < start; ++k) {
        int taskId = schedule[k];
        if (currentTask >= 0) {
            currentTime += setupTimes[currentTask][taskId];
        }
        currentTime += orders[taskId].processingTime;
        currentTask = taskId;
    }

    int oldCurrentTime = currentTime;
    int oldCurrentTask = currentTask;

    // Calculate old cost from 'start' to 'end'
    int end = std::max(i, j);
    double oldCost = 0.0;
    currentTime = oldCurrentTime;
    currentTask = oldCurrentTask;
    for (int k = start; k <= end; ++k) {
        int taskId = schedule[k];
        if (currentTask >= 0) {
            int setup = setupTimes[currentTask][taskId];
            currentTime += setup;
            oldCost += setup;
        }
        currentTime += orders[taskId].processingTime;

        // Penalty
        if (currentTime > orders[taskId].dueTime) {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            oldCost += penalty;
        }

        currentTask = taskId;
    }

    // Perform the swap
    std::vector<int> newSchedule = schedule;
    std::swap(newSchedule[i], newSchedule[j]);

    // Calculate new cost from 'start' to 'end'
    double newCost = 0.0;
    currentTime = oldCurrentTime;
    currentTask = oldCurrentTask;
    for (int k = start; k <= end; ++k) {
        int taskId = newSchedule[k];
        if (currentTask >= 0) {
            int setup = setupTimes[currentTask][taskId];
            currentTime += setup;
            newCost += setup;
        }
        currentTime += orders[taskId].processingTime;

        // Penalty
        if (currentTime > orders[taskId].dueTime) {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            newCost += penalty;
        }

        currentTask = taskId;
    }

    double deltaCost = newCost - oldCost;
    return deltaCost;
}

// Two-Opt Neighborhood Function
bool twoOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                        const std::vector<std::vector<int>>& setupTimes, double& totalCost) {
    int n = schedule.size();

    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double deltaCost = calculateTwoOptCostDifference(schedule, orders, setupTimes, i, j);

            if (deltaCost < -IMPROVEMENT_THRESHOLD) {
                // Perform the 2-opt move
                std::reverse(schedule.begin() + i, schedule.begin() + j + 1);
                totalCost += deltaCost;
                return true;  // Improvement found
            }
        }
    }

    return false;  // No improvement found
}

// Helper Function for Two-Opt Neighborhood
double calculateTwoOptCostDifference(const std::vector<int>& schedule, const std::vector<Order>& orders,
                                     const std::vector<std::vector<int>>& setupTimes, int i, int j) {
    int n = schedule.size();

    // Determine the start position for recalculating cost
    int start = i;

    // Compute cumulative currentTime up to 'start'
    int currentTime = 0;
    int currentTask = -1;
    for (int k = 0; k < start; ++k) {
        int taskId = schedule[k];
        if (currentTask >= 0) {
            currentTime += setupTimes[currentTask][taskId];
        }
        currentTime += orders[taskId].processingTime;
        currentTask = taskId;
    }

    int oldCurrentTime = currentTime;
    int oldCurrentTask = currentTask;

    // Calculate old cost from 'start' to 'n - 1'
    double oldCost = 0.0;
    currentTime = oldCurrentTime;
    currentTask = oldCurrentTask;
    for (int k = start; k < n; ++k) {
        int taskId = schedule[k];
        if (currentTask >= 0) {
            int setup = setupTimes[currentTask][taskId];
            currentTime += setup;
            oldCost += setup;
        }
        currentTime += orders[taskId].processingTime;

        // Penalty
        if (currentTime > orders[taskId].dueTime) {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            oldCost += penalty;
        }

        currentTask = taskId;
    }

    // Perform the 2-opt move
    std::vector<int> newSchedule = schedule;
    std::reverse(newSchedule.begin() + i, newSchedule.begin() + j + 1);

    // Calculate new cost from 'start' to 'n - 1'
    double newCost = 0.0;
    currentTime = oldCurrentTime;
    currentTask = oldCurrentTask;
    for (int k = start; k < n; ++k) {
        int taskId = newSchedule[k];
        if (currentTask >= 0) {
            int setup = setupTimes[currentTask][taskId];
            currentTime += setup;
            newCost += setup;
        }
        currentTime += orders[taskId].processingTime;

        // Penalty
        if (currentTime > orders[taskId].dueTime) {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            newCost += penalty;
        }

        currentTask = taskId;
    }

    double deltaCost = newCost - oldCost;
    return deltaCost;
}

// Reinsertion Neighborhood Function
bool reinsertionNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes, double& totalCost) {
    int n = schedule.size();

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (i == j || i == j - 1) continue;

            double deltaCost = calculateReinsertionCostDifference(schedule, orders, setupTimes, i, j);

            if (deltaCost < -IMPROVEMENT_THRESHOLD) {
                // Perform the reinsertion
                int task = schedule[i];
                schedule.erase(schedule.begin() + i);
                int insertPos = (j > i) ? j - 1 : j;
                schedule.insert(schedule.begin() + insertPos, task);
                totalCost += deltaCost;
                return true;  // Improvement found
            }
        }
    }

    return false;  // No improvement found
}

// Helper Function for Reinsertion Neighborhood
double calculateReinsertionCostDifference(const std::vector<int>& schedule, const std::vector<Order>& orders,
                                          const std::vector<std::vector<int>>& setupTimes, int i, int j) {
    int n = schedule.size();

    // Determine the start position for recalculating cost
    int start = std::min(i, j == n ? n - 1 : j);

    // Compute cumulative currentTime up to 'start'
    int currentTime = 0;
    int currentTask = -1;
    for (int k = 0; k < start; ++k) {
        int taskId = schedule[k];
        if (currentTask >= 0) {
            currentTime += setupTimes[currentTask][taskId];
        }
        currentTime += orders[taskId].processingTime;
        currentTask = taskId;
    }

    int oldCurrentTime = currentTime;
    int oldCurrentTask = currentTask;

    // Calculate old cost from 'start' to 'n - 1'
    double oldCost = 0.0;
    currentTime = oldCurrentTime;
    currentTask = oldCurrentTask;
    for (int k = start; k < n; ++k) {
        int taskId = schedule[k];
        if (currentTask >= 0) {
            int setup = setupTimes[currentTask][taskId];
            currentTime += setup;
            oldCost += setup;
        }
        currentTime += orders[taskId].processingTime;

        // Penalty
        if (currentTime > orders[taskId].dueTime) {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            oldCost += penalty;
        }

        currentTask = taskId;
    }

    // Perform the reinsertion
    std::vector<int> newSchedule = schedule;
    int task = newSchedule[i];
    newSchedule.erase(newSchedule.begin() + i);
    int insertPos = (j > i) ? j - 1 : j;
    newSchedule.insert(newSchedule.begin() + insertPos, task);

    // Calculate new cost from 'start' to 'n - 1'
    double newCost = 0.0;
    currentTime = oldCurrentTime;
    currentTask = oldCurrentTask;
    for (int k = start; k < n; ++k) {
        int taskId = newSchedule[k];
        if (currentTask >= 0) {
            int setup = setupTimes[currentTask][taskId];
            currentTime += setup;
            newCost += setup;
        }
        currentTime += orders[taskId].processingTime;

        // Penalty
        if (currentTime > orders[taskId].dueTime) {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            newCost += penalty;
        }

        currentTask = taskId;
    }

    double deltaCost = newCost - oldCost;
    return deltaCost;
}

// Or-Opt Neighborhood Function
bool orOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                       const std::vector<std::vector<int>>& setupTimes, double& totalCost) {
    int n = schedule.size();

    for (int l = 1; l <= 3; ++l) {  // Or-Opt with sequences of length 1 to 3
        for (int i = 0; i <= n - l; ++i) {
            for (int j = 0; j <= n - l; ++j) {
                if (j >= i && j <= i + l - 1) continue;

                double deltaCost = calculateOrOptCostDifference(schedule, orders, setupTimes, i, j, l);

                if (deltaCost < -IMPROVEMENT_THRESHOLD) {
                    // Lets go to the or-opt boys
                    std::vector<int> sequence(schedule.begin() + i, schedule.begin() + i + l);
                    schedule.erase(schedule.begin() + i, schedule.begin() + i + l);
                    int insertPos = (j > i) ? j - l + 1 : j;
                    schedule.insert(schedule.begin() + insertPos, sequence.begin(), sequence.end());
                    totalCost += deltaCost;
                    return true;  // Improvement found
                }
            }
        }
    }

    return false;  // No improvement found
}

// Helper Function for Or-Opt Neighborhood
double calculateOrOptCostDifference(const std::vector<int>& schedule, const std::vector<Order>& orders,
                                    const std::vector<std::vector<int>>& setupTimes, int i, int j, int l) {
    int n = schedule.size();

    // Determine positions affected by the move
    int start = std::min(i, j);
    int end = std::max(i + l - 1, j + l - 1);
    if (end >= n) end = n - 1;

    // Compute cumulative currentTime up to 'start'
    int currentTime = 0;
    int currentTask = -1;
    for (int k = 0; k < start; ++k) {
        int taskId = schedule[k];
        if (currentTask >= 0) {
            currentTime += setupTimes[currentTask][taskId];
        }
        currentTime += orders[taskId].processingTime;
        currentTask = taskId;
    }

    int oldCurrentTime = currentTime;
    int oldCurrentTask = currentTask;

    // Calculate old cost from 'start' to 'end'
    double oldCost = 0.0;
    currentTime = oldCurrentTime;
    currentTask = oldCurrentTask;

    for (int k = start; k <= end; ++k) {
        int taskId = schedule[k];
        if (currentTask >= 0) {
            int setupTime = setupTimes[currentTask][taskId];
            currentTime += setupTime;
            oldCost += setupTime;
        }
        currentTime += orders[taskId].processingTime;

        // Penalty
        if (currentTime > orders[taskId].dueTime) {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            oldCost += penalty;
        }

        currentTask = taskId;
    }

    // Perform the Or-Opt move
    std::vector<int> newSchedule = schedule;
    std::vector<int> sequence(newSchedule.begin() + i, newSchedule.begin() + i + l);
    newSchedule.erase(newSchedule.begin() + i, newSchedule.begin() + i + l);
    int insertPos = (j > i) ? j - l + 1 : j;
    newSchedule.insert(newSchedule.begin() + insertPos, sequence.begin(), sequence.end());

    // Calculate new cost from 'start' to 'end'
    double newCost = 0.0;
    currentTime = oldCurrentTime;
    currentTask = oldCurrentTask;

    for (int k = start; k <= end; ++k) {
        int taskId = newSchedule[k];
        if (currentTask >= 0) {
            int setupTime = setupTimes[currentTask][taskId];
            currentTime += setupTime;
            newCost += setupTime;
        }
        currentTime += orders[taskId].processingTime;

        // Penalty
        if (currentTime > orders[taskId].dueTime) {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            newCost += penalty;
        }

        currentTask = taskId;
    }

    double deltaCost = newCost - oldCost;
    return deltaCost;
}
