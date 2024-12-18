#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <vector>
#include <functional>
#include <random>
#include "order.h"
#include "schedule_data.h"

// Constants
constexpr double IMPROVEMENT_THRESHOLD = 1.0;
constexpr int MAX_NO_IMPROVEMENT_ITERATIONS = 240;
void printImprovementStatistics();
constexpr int GRASP_ITERATIONS = 10;
constexpr int RCL_SIZE = 15;
constexpr int TABU_TENURE = 100;
constexpr int MAX_TABU_LIST_SIZE = 1000;

// algorithm.h
extern int swap_improvement_count;
extern int block_exchange_improvement_count;
extern int two_opt_improvement_count;
extern int block_shift_improvement_count;

size_t computeScheduleHash(const std::vector<int>& schedule);


double calculateTotalPenaltyForSchedule(const std::vector<int>& schedule,
                                 const std::vector<Order>& orders,
                                 const std::vector<std::vector<int>>& setupTimes,
                                 const std::vector<int>& initialSetupTimes);

std::vector<int> GRASP(const std::vector<Order>& orders,
                       const std::vector<std::vector<int>>& setupTimes,
                       const std::vector<int>& initialSetupTimes,
                       double& totalPenaltyCost,
                       std::mt19937& rng);

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &pair) const
    {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

struct TaskPriority {
    int taskId;
    double priority;

    // Custom comparator to use priority queue as a max-heap
    bool operator<(const TaskPriority& other) const {
        return priority < other.priority; // Higher priority tasks should come first
    }
};
struct ComparePriority {
    bool operator()(const TaskPriority& lhs, const TaskPriority& rhs) const {
        return lhs.priority < rhs.priority;
    }
};
// Function declarations
double calculatePriority(const Order &order, int setupTime);


void calculateTotalPenalty(ScheduleData &scheduleData, const std::vector<Order> &orders,
                           const std::vector<std::vector<int>> &setupTimes,
                           const std::vector<int> &initialSetupTimes);

void RVND(ScheduleData& scheduleData, const std::vector<Order>& orders,
                  const std::vector<std::vector<int>>& setupTimes,
                  const std::vector<int>& initialSetupTimes, std::mt19937& rng);

void perturbSolution(std::vector<int>& schedule, std::mt19937& rng);

std::vector<int> ILS(const std::vector<int>& initialSchedule,
                     const std::vector<Order>& orders,
                     const std::vector<std::vector<int>>& setupTimes,
                     const std::vector<int>& initialSetupTimes,
                     double& currentPenaltyCost,
                     std::mt19937& rng);

std::vector<int> greedyConstruction(const std::vector<Order> &orders,
                                    const std::vector<std::vector<int>> &setupTimes,
                                    const std::vector<int> &initialSetupTimes,
                                    double alpha,
                                    std::mt19937* rng);
#endif // ALGORITHM_H
