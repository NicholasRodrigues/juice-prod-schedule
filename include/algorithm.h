#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <vector>
#include <functional>
#include <random>
#include "order.h"

// Constants
constexpr double IMPROVEMENT_THRESHOLD = 1.0;
constexpr int MAX_NO_IMPROVEMENT_ITERATIONS = 600;
constexpr int PERTURBATION_STRENGTH_MIN = 1;
constexpr int PERTURBATION_STRENGTH_MAX = 57;

constexpr int GRASP_ITERATIONS = 200;
constexpr int RCL_SIZE = 15;

std::vector<int> GRASP(const std::vector<Order> &orders,
                       const std::vector<std::vector<int>> &setupTimes,
                       const std::vector<int> &initialSetupTimes,
                       double &totalPenaltyCost);

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &pair) const
    {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

// Function declarations
double calculatePriority(const Order &order, int setupTime);

int getSetupTime(int currentTask, int nextTask, const std::vector<std::vector<int>> &setupTimes,
                 const std::vector<int> &initialSetupTimes);
bool acceptSolution(double currentPenalty, double newPenalty, double threshold);

double calculateTotalPenalty(const std::vector<int> &schedule,
                             const std::vector<Order> &orders,
                             const std::vector<std::vector<int>> &setupTimes,
                             const std::vector<int> &initialSetupTimes);

std::vector<int> greedyAlgorithm(const std::vector<Order> &orders,
                                 const std::vector<std::vector<int>> &setupTimes,
                                 const std::vector<int> &initialSetupTimes,
                                 double &totalPenaltyCost);

std::vector<int> adaptiveRVND(std::vector<int> &schedule, const std::vector<Order> &orders,
                              const std::vector<std::vector<int>> &setupTimes,
                              const std::vector<int> &initialSetupTimes, double &currentPenalty);

void adaptiveShuffle(
    std::vector<std::function<bool(std::vector<int> &, const std::vector<Order> &,
                                   const std::vector<std::vector<int>> &,
                                   const std::vector<int> &)>> &neighborhoods,
    std::vector<double> &neighborhoodWeights,
    std::mt19937 &g);

void perturbSolution(std::vector<int> &schedule, int perturbationStrength);

std::vector<int> ILS(const std::vector<int> &initialSchedule, const std::vector<Order> &orders,
                     const std::vector<std::vector<int>> &setupTimes,
                     const std::vector<int> &initialSetupTimes);

#endif // ALGORITHM_H
