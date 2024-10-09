#ifndef NEIGHBORHOODS_H
#define NEIGHBORHOODS_H

#include <vector>
#include "order.h"

// Function declarations
bool swapNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                      const std::vector<std::vector<int>>& setupTimes,
                      const std::vector<int>& initialSetupTimes, double& currentPenalty);

bool reinsertionNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes,
                             const std::vector<int>& initialSetupTimes, double& currentPenalty);

bool twoOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                        const std::vector<std::vector<int>>& setupTimes,
                        const std::vector<int>& initialSetupTimes, double& currentPenalty);

bool blockExchangeNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                               const std::vector<std::vector<int>>& setupTimes,
                               const std::vector<int>& initialSetupTimes, double& currentPenalty);

bool blockShiftNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                            const std::vector<std::vector<int>>& setupTimes,
                            const std::vector<int>& initialSetupTimes, double& currentPenalty);

bool threeOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
                          const std::vector<std::vector<int>>& setupTimes,
                          const std::vector<int>& initialSetupTimes, double& currentPenalty);


#endif // NEIGHBORHOODS_H
