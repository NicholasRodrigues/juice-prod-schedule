#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <vector>
#include <functional>
#include <random>
#include "order.h"

// Constants
constexpr double IMPROVEMENT_THRESHOLD = 1.0;
constexpr int MAX_NO_IMPROVEMENT_ITERATIONS = 1000;
constexpr int PERTURBATION_STRENGTH_MIN = 1;
constexpr int PERTURBATION_STRENGTH_MAX = 57;

void initializeMatrixU(int n);

// Encode solution for O(1) lookup in Tabu List
std::string encodeSolution(const std::vector<int>& schedule);

// Calculate the total penalty for a given schedule
double calculateTotalPenalty(const std::vector<int>& schedule, const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes,
                             const std::vector<int>& initialSetupTimes);

// Randomized Insertion Heuristic for generating initial solution
std::vector<int> randomizedInsertionHeuristic(const std::vector<Order>& orders,
                                              const std::vector<std::vector<int>>& setupTimes);

// Randomized Variable Neighborhood Descent (RVND) algorithm
std::vector<int> RVND(std::vector<int>& schedule, const std::vector<Order>& orders,
                      const std::vector<std::vector<int>>& setupTimes);

// Perturbation function for ILS using Double-Bridge move
void perturbSolution(std::vector<int>& schedule);

// Main ILS-RVND algorithm
std::vector<int> ILS_RVND(int maxIter, int maxIterLS, const std::vector<Order>& orders,
                          const std::vector<std::vector<int>>& setupTimes);
double calculateTotalPenalty(const std::vector<int>& schedule,
                             const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes,
                             const std::vector<int>& initialSetupTimes);

#endif // ALGORITHM_H
