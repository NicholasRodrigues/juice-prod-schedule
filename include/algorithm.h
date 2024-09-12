#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <vector>
#include <functional>

struct Order {
    int id;
    int processingTime;
    int dueTime;
    int penaltyRate;
};

double calculateInitialWeight(const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes);

double calculateTotalCost(const std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double& totalPenaltyCost);

std::vector<int> advancedGreedyAlgorithmWithDynamicWeight(const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double finalSetupTimeWeight, double& totalPenaltyCost, double& totalCost);

std::vector<int> RVND(std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double& totalCost);


#endif
