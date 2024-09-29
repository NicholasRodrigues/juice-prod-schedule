#include "neighborhoods.h"
#include "algorithm.h"
#include <algorithm>
#include <set>
#include <iostream>

// Define improvement threshold
#ifndef IMPROVEMENT_THRESHOLD
#define IMPROVEMENT_THRESHOLD 1e-6
#endif

// Swap Neighborhood Function
bool swapNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                      const std::vector<std::vector<int>>& setupTimes,
                      const std::vector<int>& initialSetupTimes) {
    int n = schedule.size();
    bool improvementFound = false;

    double currentPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);

    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            // Swap tasks at positions i and j
            std::swap(schedule[i], schedule[j]);

            double newPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);

            if (newPenalty < currentPenalty - IMPROVEMENT_THRESHOLD) {
                // Improvement found, keep the swap
                improvementFound = true;
                return true;
            } else {
                // Swap back to restore original schedule
                std::swap(schedule[i], schedule[j]);
            }
        }
    }

    return improvementFound;  // No improvement found
}

// Reinsertion Neighborhood Function (Handles subsequences)
bool reinsertionNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes,
                             const std::vector<int>& initialSetupTimes) {
    int n = schedule.size();
    bool improvementFound = false;

    double currentPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);

    // Consider subsequences of varying lengths
    for (int l = 1; l <= std::min(5, n); ++l) {
        for (int i = 0; i <= n - l; ++i) {
            for (int j = 0; j <= n - l; ++j) {
                if (j >= i && j <= i + l - 1) continue;

                // Create a new schedule with the subsequence reinserted
                std::vector<int> newSchedule = schedule;
                std::vector<int> subsequence(newSchedule.begin() + i, newSchedule.begin() + i + l);
                newSchedule.erase(newSchedule.begin() + i, newSchedule.begin() + i + l);

                int insertPos = j;
                if (j > i) {
                    insertPos = j - l;
                }
                newSchedule.insert(newSchedule.begin() + insertPos, subsequence.begin(), subsequence.end());

                double newPenalty = calculateTotalPenalty(newSchedule, orders, setupTimes, initialSetupTimes);

                if (newPenalty < currentPenalty - IMPROVEMENT_THRESHOLD) {
                    // Apply the move
                    schedule = newSchedule;
                    improvementFound = true;
                    return true;  // Improvement found
                }
            }
        }
    }

    return improvementFound;  // No improvement found
}

// Or-Opt Neighborhood Function
bool orOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                       const std::vector<std::vector<int>>& setupTimes,
                       const std::vector<int>& initialSetupTimes) {
    int n = schedule.size();
    bool improvementFound = false;

    double currentPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);

    for (int l = 2; l <= 3; ++l) {
        for (int i = 0; i <= n - l; ++i) {
            for (int j = 0; j <= n - l; ++j) {
                if (j >= i && j <= i + l - 1) continue;

                // Create a new schedule with the subsequence reinserted
                std::vector<int> newSchedule = schedule;
                std::vector<int> subsequence(newSchedule.begin() + i, newSchedule.begin() + i + l);
                newSchedule.erase(newSchedule.begin() + i, newSchedule.begin() + i + l);

                int insertPos = j;
                if (j > i) {
                    insertPos = j - l;
                }
                newSchedule.insert(newSchedule.begin() + insertPos, subsequence.begin(), subsequence.end());

                double newPenalty = calculateTotalPenalty(newSchedule, orders, setupTimes, initialSetupTimes);

                if (newPenalty < currentPenalty - IMPROVEMENT_THRESHOLD) {
                    // Apply the move
                    schedule = newSchedule;
                    improvementFound = true;
                    return true;  // Improvement found
                }
            }
        }
    }

    return improvementFound;  // No improvement found
}
