#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <vector>

struct Order {
    int id;
    int processingTime;
    int dueTime;
    int penaltyRate;
};

double calculateInitialWeight(const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes);

std::vector<int> advancedGreedyAlgorithmWithDynamicWeight(const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double finalSetupTimeWeight);

#endif
