#ifndef NEIGHBORHOODS_H
#define NEIGHBORHOODS_H

#include <vector>
#include "order.h"


bool swapNeighborhood(std::vector<int> &schedule, const std::vector<Order> &orders,
                      const std::vector<std::vector<int>> &setupTimes, double &totalCost);


bool reinsertionNeighborhood(std::vector<int> &schedule, const std::vector<Order> &orders,
                             const std::vector<std::vector<int>> &setupTimes, double &totalCost);


bool blockSwapNeighborhood(std::vector<int> &schedule, const std::vector<Order> &orders,
                           const std::vector<std::vector<int>> &setupTimes, double &totalCost);

double calculateSwapCostDifference(const std::vector<int> &schedule, const std::vector<Order> &orders,
                                   const std::vector<std::vector<int>> &setupTimes, int i, int j);


double calculateJanelsReinsertionCostDifference(const std::vector<int> &schedule, const std::vector<Order> &orders,
                                                const std::vector<std::vector<int>> &setupTimes, int i, int j);


double calculateBlockSwapCostDifference(const std::vector<int> &schedule, const std::vector<Order> &orders,
                                        const std::vector<std::vector<int>> &setupTimes, int start1, int end1, int start2, int end2);

#endif // NEIGHBORHOODS_H