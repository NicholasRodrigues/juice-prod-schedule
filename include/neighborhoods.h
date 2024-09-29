#ifndef NEIGHBORHOODS_H
#define NEIGHBORHOODS_H

#include <vector>
#include "order.h"

// Function declarations for neighborhoods
bool swapNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                      const std::vector<std::vector<int>>& setupTimes, double& totalCost);

bool twoOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                        const std::vector<std::vector<int>>& setupTimes, double& totalCost);

bool reinsertionNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes, double& totalCost);

bool orOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                       const std::vector<std::vector<int>>& setupTimes, double& totalCost);

// Helper function declarations for calculating delta costs
double calculateSwapDeltaCost(const std::vector<int>& schedule, const std::vector<Order>& orders,
                              const std::vector<std::vector<int>>& setupTimes, int i, int j);

double calculateTwoOptDeltaCost(const std::vector<int>& schedule, const std::vector<Order>& orders,
                                const std::vector<std::vector<int>>& setupTimes, int i, int j);

double calculateReinsertionDeltaCost(const std::vector<int>& schedule, const std::vector<Order>& orders,
                                     const std::vector<std::vector<int>>& setupTimes, int i, int j);

double calculateOrOptDeltaCost(const std::vector<int>& schedule, const std::vector<Order>& orders,
                               const std::vector<std::vector<int>>& setupTimes, int i, int j, int l);

double calculatePenalty(const std::vector<int>& schedule, const std::vector<Order>& orders,
                        const std::vector<std::vector<int>>& setupTimes, int idx);

#endif // NEIGHBORHOODS_H
