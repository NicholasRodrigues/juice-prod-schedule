#ifndef NEIGHBORHOODS_H
#define NEIGHBORHOODS_H

#include <vector>
#include "order.h"
#include "schedule_data.h"

// Function declarations
//bool swapNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
//                      const std::vector<std::vector<int>>& setupTimes,
//                      const std::vector<int>& initialSetupTimes, double& currentPenalty);

//bool reinsertionNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
//                             const std::vector<std::vector<int>>& setupTimes,
//                             const std::vector<int>& initialSetupTimes, double& currentPenalty);

//bool twoOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
//                        const std::vector<std::vector<int>>& setupTimes,
//                        const std::vector<int>& initialSetupTimes, double& currentPenalty);

//bool blockExchangeNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
//                               const std::vector<std::vector<int>>& setupTimes,
//                               const std::vector<int>& initialSetupTimes, double& currentPenalty);
//
//bool blockShiftNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
//                            const std::vector<std::vector<int>>& setupTimes,
//                            const std::vector<int>& initialSetupTimes, double& currentPenalty);

//bool threeOptNeighborhood(std::vector<int>& schedule, const std::vector<Order>& orders,
//                          const std::vector<std::vector<int>>& setupTimes,
//                          const std::vector<int>& initialSetupTimes, double& currentPenalty);

// Swap Neighborhood Function
bool swapNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                      const std::vector<std::vector<int>> &setupTimes,
                      const std::vector<int> &initialSetupTimes);

// Delta Penalty Calculation for Swap
double computeDeltaPenaltySwap(const ScheduleData &scheduleData, const std::vector<Order> &orders,
                               const std::vector<std::vector<int>> &setupTimes,
                               const std::vector<int> &initialSetupTimes,
                               int i, int j);

// Update Schedule Data after Swap
void updateScheduleDataSwap(ScheduleData &scheduleData, const std::vector<Order> &orders,
                            const std::vector<std::vector<int>> &setupTimes,
                            const std::vector<int> &initialSetupTimes,
                            int i, int j);

// Block Exchange Neighborhood Function
bool blockExchangeNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                               const std::vector<std::vector<int>> &setupTimes,
                               const std::vector<int> &initialSetupTimes);

// Delta Penalty Calculation for Block Exchange
double computeDeltaPenaltyBlockExchange(const ScheduleData &scheduleData, const std::vector<Order> &orders,
                                        const std::vector<std::vector<int>> &setupTimes,
                                        const std::vector<int> &initialSetupTimes,
                                        int i, int j, int l);

// Update Schedule Data after Block Exchange
void updateScheduleDataBlockExchange(ScheduleData &scheduleData, const std::vector<Order> &orders,
                                     const std::vector<std::vector<int>> &setupTimes,
                                     const std::vector<int> &initialSetupTimes,
                                     int i, int j, int l);

// Block Shift (Reinsertion) Neighborhood Function
bool blockShiftNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                            const std::vector<std::vector<int>> &setupTimes,
                            const std::vector<int> &initialSetupTimes);

// Delta Penalty Calculation for Block Shift
double computeDeltaPenaltyBlockShift(const ScheduleData &scheduleData, const std::vector<Order> &orders,
                                     const std::vector<std::vector<int>> &setupTimes,
                                     const std::vector<int> &initialSetupTimes,
                                     int i, int j, int l);

// Update Schedule Data after Block Shift
void updateScheduleDataBlockShift(ScheduleData &scheduleData, const std::vector<Order> &orders,
                                  const std::vector<std::vector<int>> &setupTimes,
                                  const std::vector<int> &initialSetupTimes,
                                  int i, int j, int l);

// 2-Opt Neighborhood Function
bool twoOptNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                        const std::vector<std::vector<int>> &setupTimes,
                        const std::vector<int> &initialSetupTimes);

// Delta Penalty Calculation for 2-Opt
double computeDeltaPenaltyTwoOpt(const ScheduleData &scheduleData, const std::vector<Order> &orders,
                                 const std::vector<std::vector<int>> &setupTimes,
                                 const std::vector<int> &initialSetupTimes,
                                 int i, int j);

// Update Schedule Data after 2-Opt
void updateScheduleDataTwoOpt(ScheduleData &scheduleData, const std::vector<Order> &orders,
                              const std::vector<std::vector<int>> &setupTimes,
                              const std::vector<int> &initialSetupTimes,
                              int i, int j);


#endif // NEIGHBORHOODS_H
