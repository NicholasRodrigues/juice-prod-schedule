#ifndef NEIGHBORHOODS_H
#define NEIGHBORHOODS_H

#include <vector>
#include "order.h"

// Function declarations
bool swapNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                      const std::vector<std::vector<int>>& setupTimes,
                      const std::vector<int>& initialSetupTimes);

bool reinsertionNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes,
                             const std::vector<int>& initialSetupTimes);

bool twoOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                        const std::vector<std::vector<int>>& setupTimes,
                        const std::vector<int>& initialSetupTimes);

bool blockInsertNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes,
                             const std::vector<int>& initialSetupTimes, int blockSize);

bool blockReverseNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                              const std::vector<std::vector<int>>& setupTimes,
                              const std::vector<int>& initialSetupTimes, int blockSize);

#endif // NEIGHBORHOODS_H
