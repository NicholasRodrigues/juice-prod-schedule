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
    double currentPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);
    double bestPenalty = currentPenalty;
    int best_i = -1, best_j = -1;  // Indices of the best swap
    bool improvementFound = false;

    // Evaluate all possible swaps
    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            // Swap tasks at positions i and j
            std::swap(schedule[i], schedule[j]);

            double newPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);

            if (newPenalty < bestPenalty) {
                // Found a better penalty
                bestPenalty = newPenalty;
                best_i = i;
                best_j = j;
                improvementFound = true;
            }

            // Swap back to restore original schedule
            std::swap(schedule[i], schedule[j]);
        }
    }

    // Apply the best swap found
    if (improvementFound) {
        std::swap(schedule[best_i], schedule[best_j]);
        return true;  // Improvement was made
    }

    return false;  // No improvement found
}


// Reinsertion Neighborhood Function (Handles subsequences)
bool reinsertionNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes,
                             const std::vector<int>& initialSetupTimes) {
    int n = schedule.size();
    double currentPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);
    double bestPenalty = currentPenalty;
    int best_i = -1, best_j = -1, best_l = -1;  // Indices and length for the best move
    bool improvementFound = false;

    // Consider subsequences of varying lengths
    for (int l = 1; l <= std::min(3, n); ++l) {
        for (int i = 0; i <= n - l; ++i) {
            for (int j = 0; j <= n - l; ++j) {
                if (j >= i && j <= i + l - 1) continue;  // Skip invalid moves

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

                if (newPenalty < bestPenalty) {
                    // Found a better penalty
                    bestPenalty = newPenalty;
                    best_i = i;
                    best_j = j;
                    best_l = l;
                    improvementFound = true;
                }
            }
        }
    }

    // Apply the best reinsertion move found
    if (improvementFound) {
        // Extract the subsequence
        std::vector<int> subsequence(schedule.begin() + best_i, schedule.begin() + best_i + best_l);
        schedule.erase(schedule.begin() + best_i, schedule.begin() + best_i + best_l);

        // Adjust insertion position if necessary
        int insertPos = best_j;
        if (best_j > best_i) {
            insertPos = best_j - best_l;
        }

        // Insert the subsequence at the new position
        schedule.insert(schedule.begin() + insertPos, subsequence.begin(), subsequence.end());
        return true;  // Improvement was made
    }

    return false;  // No improvement found
}

bool twoOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                        const std::vector<std::vector<int>>& setupTimes,
                        const std::vector<int>& initialSetupTimes) {
    int n = schedule.size();
    double currentPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);
    double bestPenalty = currentPenalty;
    int best_i = -1, best_j = -1;  // Indices for the best 2-opt move
    bool improvementFound = false;

    // Limit the block size to a maximum of 10
    int maxBlocks = std::min(10, n);

    // Evaluate all possible 2-opt moves with the limit
    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < std::min(i + maxBlocks, n); ++j) {
            // Reverse the subsequence from i to j
            std::reverse(schedule.begin() + i, schedule.begin() + j + 1);

            double newPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);

            if (newPenalty < bestPenalty) {
                // Found a better penalty
                bestPenalty = newPenalty;
                best_i = i;
                best_j = j;
                improvementFound = true;
            }

            // Reverse back to restore original schedule
            std::reverse(schedule.begin() + i, schedule.begin() + j + 1);
        }
    }

    // Apply the best 2-opt move found
    if (improvementFound) {
        std::reverse(schedule.begin() + best_i, schedule.begin() + best_j + 1);
        return true;  // Improvement was made
    }

    return false;  // No improvement found
}


bool blockInsertNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes,
                             const std::vector<int>& initialSetupTimes, int blockSize) {
    int n = schedule.size();
    double currentPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);
    double bestPenalty = currentPenalty;
    int best_i = -1, best_j = -1;  // Indices for the best block move
    bool improvementFound = false;

    for (int i = 0; i <= n - blockSize; ++i) {
        for (int j = 0; j <= n - blockSize; ++j) {
            if (j >= i && j <= i + blockSize - 1) continue;  // Skip invalid moves

            std::vector<int> newSchedule = schedule;
            std::vector<int> block(newSchedule.begin() + i, newSchedule.begin() + i + blockSize);
            newSchedule.erase(newSchedule.begin() + i, newSchedule.begin() + i + blockSize);

            int insertPos = (j > i) ? j - blockSize : j;
            newSchedule.insert(newSchedule.begin() + insertPos, block.begin(), block.end());

            double newPenalty = calculateTotalPenalty(newSchedule, orders, setupTimes, initialSetupTimes);

            if (newPenalty < bestPenalty) {
                bestPenalty = newPenalty;
                best_i = i;
                best_j = insertPos;
                improvementFound = true;
            }
        }
    }

    // Apply the best block insertion found
    if (improvementFound) {
        std::vector<int> block(schedule.begin() + best_i, schedule.begin() + best_i + blockSize);
        schedule.erase(schedule.begin() + best_i, schedule.begin() + best_i + blockSize);
        schedule.insert(schedule.begin() + best_j, block.begin(), block.end());
        return true;
    }

    return false;
}

bool blockReverseNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                              const std::vector<std::vector<int>>& setupTimes,
                              const std::vector<int>& initialSetupTimes, int blockSize) {
    int n = schedule.size();
    double currentPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);
    double bestPenalty = currentPenalty;
    int best_i = -1;  // Index for the best block reversal
    bool improvementFound = false;

    for (int i = 0; i <= n - blockSize; ++i) {
        std::vector<int> newSchedule = schedule;
        std::reverse(newSchedule.begin() + i, newSchedule.begin() + i + blockSize);

        double newPenalty = calculateTotalPenalty(newSchedule, orders, setupTimes, initialSetupTimes);

        if (newPenalty < bestPenalty) {
            bestPenalty = newPenalty;
            best_i = i;
            improvementFound = true;
        }
    }

    // Apply the best block reversal found
    if (improvementFound) {
        std::reverse(schedule.begin() + best_i, schedule.begin() + best_i + blockSize);
        return true;
    }

    return false;
}