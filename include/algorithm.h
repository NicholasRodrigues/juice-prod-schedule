#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <vector>
#include <functional>
#include <random>
#include "order.h"

// Constants
constexpr double IMPROVEMENT_THRESHOLD = 1.0;
constexpr int MAX_NO_IMPROVEMENT_ITERATIONS = 100;
constexpr int PERTURBATION_STRENGTH_MIN = 1;
constexpr int PERTURBATION_STRENGTH_MAX = 57;

constexpr int GRASP_ITERATIONS = 100;
constexpr int RCL_SIZE = 5;

// Function declarations
double calculateTotalPenalty(const std::vector<int> &schedule, const std::vector<Order> &orders,
                             const std::vector<std::vector<int>> &setupTimes,
                             const std::vector<int> &initialSetupTimes);

std::vector<int> greedyAlgorithm(const std::vector<Order> &orders,
                                 const std::vector<std::vector<int>> &setupTimes,
                                 const std::vector<int> &initialSetupTimes,
                                 double &totalPenaltyCost);

std::vector<int> adaptiveRVND(std::vector<int> &schedule, const std::vector<Order> &orders,
                              const std::vector<std::vector<int>> &setupTimes,
                              const std::vector<int> &initialSetupTimes);

void adaptiveShuffle(
    std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                   const std::vector<std::vector<int>> &,
                                   const std::vector<int> &)>> &neighborhoods,
    std::vector<double> &neighborhoodWeights,
    std::mt19937 &g);

void perturbSolution(std::vector<int> &schedule, int perturbationStrength);

std::vector<int> ILS(const std::vector<int> &initialSchedule, const std::vector<Order> &orders,
                     const std::vector<std::vector<int>> &setupTimes,
                     const std::vector<int> &initialSetupTimes);

#endif // ALGORITHM_H
