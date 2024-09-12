#ifndef NEIGHBORHOODS_H
#define NEIGHBORHOODS_H

#include <vector>
#include "algorithm.h"

// Swap neighborhood function with dynamic cost update
bool swapNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double& totalCost);

// 2-opt neighborhood function with dynamic cost update
bool twoOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double& totalCost);

// Reinsertion neighborhood function with dynamic cost update
bool reinsertionNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double& totalCost);

// Utility function to dynamically calculate cost difference for swap
double calculateSwapCostDiff(int i, int j, std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double currentCost);

// Utility function to dynamically calculate cost difference for 2-opt
double calculateTwoOptCostDiff(int i, int j, std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double currentCost);

// Utility function to dynamically calculate cost difference for reinsertion
double calculateReinsertionCostDiff(int i, int newPos, std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes, double currentCost);

#endif
