#ifndef NEIGHBORHOODS_H
#define NEIGHBORHOODS_H

#include <vector>
#include "order.h"
#include "schedule_data.h"
#include <unordered_map>

struct VectorHash {
    std::size_t operator()(const std::vector<int>& vec) const {
        std::size_t hash = 0;
        std::hash<int> hasher;  // Standard hash function for integers
        // Combine the hash of each element
        for (int v : vec) {
            hash ^= hasher(v) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};
// Swap Neighborhood Function
bool swapNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                      const std::vector<std::vector<int>> &setupTimes,
                      const std::vector<int> &initialSetupTimes, std::unordered_map<std::vector<int>, double, VectorHash> &taboo);
// (Reinsertion) Neighborhood Function
bool reinsertionNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                            const std::vector<std::vector<int>> &setupTimes,
                            const std::vector<int> &initialSetupTimes, std::unordered_map<std::vector<int>, double, VectorHash> &taboo);

// 2-Opt Neighborhood Function
bool twoOptNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                        const std::vector<std::vector<int>> &setupTimes,
                        const std::vector<int> &initialSetupTimes, std::unordered_map<std::vector<int>, double, VectorHash> &taboo);

#endif // NEIGHBORHOODS_H
