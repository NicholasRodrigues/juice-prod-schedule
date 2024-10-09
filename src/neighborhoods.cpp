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
                      const std::vector<int>& initialSetupTimes, double& currentPenalty) {
    int n = schedule.size();
    double bestPenalty = currentPenalty;
    int best_i = -1, best_j = -1;
    bool improvementFound = false;

    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            std::swap(schedule[i], schedule[j]);
            double newPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);

            if (newPenalty < bestPenalty) {
                bestPenalty = newPenalty;
                best_i = i;
                best_j = j;
                improvementFound = true;
            }

            std::swap(schedule[i], schedule[j]);  // Restore original
        }
    }

    if (improvementFound) {
        std::swap(schedule[best_i], schedule[best_j]);
        currentPenalty = bestPenalty;
        return true;
    }

    return false;
}

// Block Exchange Neighborhood (Exchanges two blocks)
bool blockExchangeNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                               const std::vector<std::vector<int>>& setupTimes,
                               const std::vector<int>& initialSetupTimes, double& currentPenalty) {
    int n = schedule.size();
    double bestPenalty = currentPenalty;
    int best_i = -1, best_j = -1, best_l = -1;
    bool improvementFound = false;

    // Consider block sizes between 2 and 4
    for (int l = 2; l <= std::min(4, n / 2); ++l) {
        for (int i = 0; i <= n - 2 * l; ++i) {
            for (int j = i + l; j <= n - l; ++j) {
                if (i + l <= n && j + l <= n) {  // Boundary check
                    std::vector<int> newSchedule = schedule;
                    // Swap two blocks of length l
                    std::swap_ranges(newSchedule.begin() + i, newSchedule.begin() + i + l,
                                     newSchedule.begin() + j);

                    double newPenalty = calculateTotalPenalty(newSchedule, orders, setupTimes, initialSetupTimes);

                    if (newPenalty < bestPenalty) {
                        bestPenalty = newPenalty;
                        best_i = i;
                        best_j = j;
                        best_l = l;
                        improvementFound = true;
                    }
                }
            }
        }
    }

    if (improvementFound) {
        std::swap_ranges(schedule.begin() + best_i, schedule.begin() + best_i + best_l,
                         schedule.begin() + best_j);
        currentPenalty = bestPenalty;
        return true;
    }

    return false;
}

// Block Shift Neighborhood (Shifts a block to another position)
bool blockShiftNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                            const std::vector<std::vector<int>>& setupTimes,
                            const std::vector<int>& initialSetupTimes, double& currentPenalty) {
    int n = schedule.size();
    double bestPenalty = currentPenalty;
    int best_i = -1, best_j = -1, best_l = -1;
    bool improvementFound = false;

    for (int l = 2; l <= std::min(5, n); ++l) {  // Consider block lengths
        for (int i = 0; i <= n - l; ++i) {
            for (int j = 0; j < n; ++j) {
                if (j >= i && j <= i + l - 1) continue;  // Skip invalid moves

                if (i + l <= n && j + l <= n) {  // Boundary check
                    std::vector<int> newSchedule = schedule;
                    std::vector<int> block(newSchedule.begin() + i, newSchedule.begin() + i + l);
                    newSchedule.erase(newSchedule.begin() + i, newSchedule.begin() + i + l);
                    newSchedule.insert(newSchedule.begin() + j, block.begin(), block.end());

                    double newPenalty = calculateTotalPenalty(newSchedule, orders, setupTimes, initialSetupTimes);

                    if (newPenalty < bestPenalty) {
                        bestPenalty = newPenalty;
                        best_i = i;
                        best_j = j;
                        best_l = l;
                        improvementFound = true;
                    }
                }
            }
        }
    }

    if (improvementFound) {
        std::vector<int> block(schedule.begin() + best_i, schedule.begin() + best_i + best_l);
        schedule.erase(schedule.begin() + best_i, schedule.begin() + best_i + best_l);
        schedule.insert(schedule.begin() + best_j, block.begin(), block.end());
        currentPenalty = bestPenalty;
        return true;
    }

    return false;
}

// Reinsertion Neighborhood (subsequence insertion)
bool reinsertionNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes,
                             const std::vector<int>& initialSetupTimes, double& currentPenalty) {
    int n = schedule.size();
    double bestPenalty = currentPenalty;
    int best_i = -1, best_j = -1, best_l = -1;
    bool improvementFound = false;

    for (int l = 1; l <= std::min(5, n); ++l) {  // Consider subsequences of varying lengths
        for (int i = 0; i <= n - l; ++i) {
            for (int j = 0; j <= n - l; ++j) {
                if (j >= i && j <= i + l - 1) continue;  // Skip invalid moves

                if (i + l <= n && j + l <= n) {  // Boundary check
                    std::vector<int> newSchedule = schedule;
                    std::vector<int> subsequence(newSchedule.begin() + i, newSchedule.begin() + i + l);
                    newSchedule.erase(newSchedule.begin() + i, newSchedule.begin() + i + l);
                    newSchedule.insert(newSchedule.begin() + j, subsequence.begin(), subsequence.end());

                    double newPenalty = calculateTotalPenalty(newSchedule, orders, setupTimes, initialSetupTimes);

                    if (newPenalty < bestPenalty) {
                        bestPenalty = newPenalty;
                        best_i = i;
                        best_j = j;
                        best_l = l;
                        improvementFound = true;
                    }
                }
            }
        }
    }

    if (improvementFound) {
        std::vector<int> subsequence(schedule.begin() + best_i, schedule.begin() + best_i + best_l);
        schedule.erase(schedule.begin() + best_i, schedule.begin() + best_i + best_l);
        schedule.insert(schedule.begin() + best_j, subsequence.begin(), subsequence.end());
        currentPenalty = bestPenalty;
        return true;
    }

    return false;
}

// 3-Opt Neighborhood (Reverses three segments of the schedule)
bool threeOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                          const std::vector<std::vector<int>>& setupTimes,
                          const std::vector<int>& initialSetupTimes, double& currentPenalty) {
    int n = schedule.size();
    double bestPenalty = currentPenalty;
    int best_i = -1, best_j = -1, best_k = -1;
    bool improvementFound = false;

    // Evaluate all possible 3-opt moves
    for (int i = 0; i < n - 2; ++i) {
        for (int j = i + 1; j < n - 1; ++j) {
            for (int k = j + 1; k < n; ++k) {
                if (i < n && j + 1 < n && k + 1 < n) {  // Boundary check
                    std::reverse(schedule.begin() + i, schedule.begin() + j + 1);
                    std::reverse(schedule.begin() + j + 1, schedule.begin() + k + 1);

                    double newPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);

                    if (newPenalty < bestPenalty) {
                        bestPenalty = newPenalty;
                        best_i = i;
                        best_j = j;
                        best_k = k;
                        improvementFound = true;
                    }

                    // Reverse back to restore original schedule
                    std::reverse(schedule.begin() + j + 1, schedule.begin() + k + 1);
                    std::reverse(schedule.begin() + i, schedule.begin() + j + 1);
                }
            }
        }
    }

    if (improvementFound) {
        std::reverse(schedule.begin() + best_i, schedule.begin() + best_j + 1);
        std::reverse(schedule.begin() + best_j + 1, schedule.begin() + best_k + 1);
        currentPenalty = bestPenalty;
        return true;
    }

    return false;
}

// 2-Opt Neighborhood (Reverses two segments of the schedule)
bool twoOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                        const std::vector<std::vector<int>>& setupTimes,
                        const std::vector<int>& initialSetupTimes, double& currentPenalty) {
    int n = schedule.size();
    double bestPenalty = currentPenalty;
    int best_i = -1, best_j = -1;
    bool improvementFound = false;

    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (i < n && j + 1 < n) {  // Boundary check
                std::reverse(schedule.begin() + i, schedule.begin() + j + 1);

                double newPenalty = calculateTotalPenalty(schedule, orders, setupTimes, initialSetupTimes);

                if (newPenalty < bestPenalty) {
                    bestPenalty = newPenalty;
                    best_i = i;
                    best_j = j;
                    improvementFound = true;
                }

                // Reverse back to restore the original schedule
                std::reverse(schedule.begin() + i, schedule.begin() + j + 1);
            }
        }
    }

    if (improvementFound) {
        std::reverse(schedule.begin() + best_i, schedule.begin() + best_j + 1);
        currentPenalty = bestPenalty;
        return true;
    }

    return false;
}
