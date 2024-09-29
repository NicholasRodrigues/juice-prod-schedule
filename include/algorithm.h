#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <vector>
#include "order.h"

constexpr int MAX_NO_IMPROVEMENT_ITERATIONS = 10000;
constexpr double IMPROVEMENT_THRESHOLD = 1e-6;
constexpr unsigned int RANDOM_SEED = 42;

void calculateTotalCost(const std::vector<int>& schedule, const std::vector<Order>& orders,
                        const std::vector<std::vector<int>>& setupTimes, double& totalCost, double& totalPenaltyCost);

double calculateTotalPenaltyCost(const std::vector<int>& schedule, const std::vector<Order>& orders,
                                 const std::vector<std::vector<int>>& setupTimes);

std::vector<int> RVND(std::vector<int>& schedule, const std::vector<Order>& orders,
                      const std::vector<std::vector<int>>& setupTimes, double& totalCost);

std::vector<int> advancedGreedyAlgorithmWithDynamicWeight(const std::vector<Order>& orders,
                                                          const std::vector<std::vector<int>>& setupTimes,
                                                          double finalSetupTimeWeight, double& totalPenaltyCost,
                                                          double& totalCost);

#endif // ALGORITHM_H
