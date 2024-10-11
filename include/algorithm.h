#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <vector>
#include <functional>
#include <random>
#include "order.h"

// Constants
constexpr double IMPROVEMENT_THRESHOLD = 1.0;
constexpr int MAX_NO_IMPROVEMENT_ITERATIONS = 240;
constexpr int PERTURBATION_STRENGTH_MIN = 1;
constexpr int PERTURBATION_STRENGTH_MAX = 57;

//double calculatePriority(const Order& order, int setupTime);
//
//std::vector<int> greedyAlgorithm(const std::vector<Order> &orders,
//                                 const std::vector<std::vector<int>> &setupTimes,
//                                 const std::vector<int> &initialSetupTimes,
//                                 double &totalPenaltyCost);
//// Function declarations
//double calculateTotalPenalty(const std::vector<int>& schedule, const std::vector<Order>& orders,
//                             const std::vector<std::vector<int>>& setupTimes,
//                             const std::vector<int>& initialSetupTimes);
//
//std::vector<int> greedyAlgorithm(const std::vector<Order>& orders,
//                                 const std::vector<std::vector<int>>& setupTimes,
//                                 const std::vector<int>& initialSetupTimes,
//                                 double& totalPenaltyCost);
//
//std::vector<int> adaptiveRVND(std::vector<int>& schedule, const std::vector<Order>& orders,
//                              const std::vector<std::vector<int>>& setupTimes,
//                              const std::vector<int>& initialSetupTimes);
//
//void adaptiveShuffle(
//    std::vector<std::function<bool(std::vector<int>&, const std::vector<Order>&,
//                                   const std::vector<std::vector<int>>&,
//                                   const std::vector<int>&)> >& neighborhoods,
//    std::vector<double>& neighborhoodWeights,
//    std::mt19937& g);
//
//void perturbSolution(std::vector<int>& schedule, int perturbationStrength);
//
//std::vector<int> ILS(const std::vector<int>& initialSchedule, const std::vector<Order>& orders,
//                     const std::vector<std::vector<int>>& setupTimes,
//                     const std::vector<int>& initialSetupTimes);

void initializeMatrixU(int n, std::mt19937& rng);

// Encode solution for O(1) lookup in Tabu List
std::string encodeSolution(const std::vector<int>& schedule);

// Calculate the total penalty for a given schedule
double calculateTotalPenalty(const std::vector<int>& schedule, const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes,
                             const std::vector<int>& initialSetupTimes);

// Randomized Insertion Heuristic for generating initial solution
std::vector<int> randomizedInsertionHeuristic(const std::vector<Order>& orders,
                                              const std::vector<std::vector<int>>& setupTimes,
                                              const std::vector<int>& initialSetupTimes,
                                              std::mt19937& rng);


// Randomized Variable Neighborhood Descent (RVND) algorithm
std::vector<int> RVND(std::vector<int>& schedule,
                      const std::vector<Order>& orders,
                      const std::vector<std::vector<int>>& setupTimes,
                      const std::vector<int>& initialSetupTimes);

// Perturbation function for ILS using Double-Bridge move
void perturbSolution(std::vector<int>& schedule);

// Main ILS-RVND algorithm
std::vector<int> ILS_RVND(int maxIter,
                          int maxIterLS,
                          const std::vector<Order>& orders,
                          const std::vector<std::vector<int>>& setupTimes,
                          const std::vector<int>& initialSetupTimes,
                          unsigned int seed);
double calculateTotalPenalty(const std::vector<int>& schedule,
                             const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes,
                             const std::vector<int>& initialSetupTimes);

#endif // ALGORITHM_H
