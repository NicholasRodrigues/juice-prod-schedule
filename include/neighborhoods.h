#ifndef NEIGHBORHOODS_H
#define NEIGHBORHOODS_H

#include <vector>
#include "order.h"
#include "schedule_data.h"

// Swap Neighborhood Function
bool swapNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                      const std::vector<std::vector<int>> &setupTimes,
                      const std::vector<int> &initialSetupTimes);
// (Reinsertion) Neighborhood Function
bool reinsertionNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                            const std::vector<std::vector<int>> &setupTimes,
                            const std::vector<int> &initialSetupTimes);

// 2-Opt Neighborhood Function
bool twoOptNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                        const std::vector<std::vector<int>> &setupTimes,
                        const std::vector<int> &initialSetupTimes);
struct TabuMove {
    int i;  // Índice inicial do primeiro bloco
    int j;  // Índice inicial do segundo bloco
    int l;  // Tamanho do bloco

    bool operator==(const TabuMove& other) const {
        return i == other.i && j == other.j && l == other.l;
    }
};
#endif // NEIGHBORHOODS_H
