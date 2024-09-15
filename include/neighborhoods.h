#ifndef NEIGHBORHOODS_H
#define NEIGHBORHOODS_H

#include <vector>
#include "order.h"

// Function declarations
bool swapNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                      const std::vector<std::vector<int>>& setupTimes, double& totalCost);

bool twoOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                        const std::vector<std::vector<int>>& setupTimes, double& totalCost);

bool reinsertionNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes, double& totalCost);

bool orOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                       const std::vector<std::vector<int>>& setupTimes, double& totalCost);

// Helper functions
double calculateSwapCostDifference(const std::vector<int>& schedule, const std::vector<Order>& orders,
                                   const std::vector<std::vector<int>>& setupTimes, int i, int j);

double calculateTwoOptCostDifference(const std::vector<int>& schedule, const std::vector<Order>& orders,
                                     const std::vector<std::vector<int>>& setupTimes, int i, int j);

double calculateReinsertionCostDifference(const std::vector<int>& schedule, const std::vector<Order>& orders,
                                          const std::vector<std::vector<int>>& setupTimes, int i, int j);

double calculateOrOptCostDifference(const std::vector<int>& schedule, const std::vector<Order>& orders,
                                    const std::vector<std::vector<int>>& setupTimes, int i, int j, int l);

#endif // NEIGHBORHOODS_H
