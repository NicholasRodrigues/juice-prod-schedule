#include "neighborhoods.h"
#include "algorithm.h"
#include <algorithm>
#include <set>
#include <iostream>
#include "schedule_data.h"


// swap Neighborhood (Exchanges two blocks or single jobs in the schedule)
bool swapNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                               const std::vector<std::vector<int>> &setupTimes,
                               const std::vector<int> &initialSetupTimes, std::unordered_map<std::vector<int>, double, VectorHash> &taboo) {

    int n = scheduleData.schedule.size();
    int best_i = -1, best_j = -1, best_l = -1;
    bool improvementFound = false;
    double bestPenalty = scheduleData.totalPenalty;

    // Consider block sizes between 2 and 4
    for (int l = 1; l <= 10; ++l) {
        for (int i = 0; i <= n - 2 * l; ++i) {
            for (int j = i + l; j <= n - l; ++j) {
                ScheduleData tempScheduleData = scheduleData;

                // Perform block exchange in the temporary schedule
                std::swap_ranges(tempScheduleData.schedule.begin() + i, tempScheduleData.schedule.begin() + i + l,
                                 tempScheduleData.schedule.begin() + j);

                if (taboo.find(tempScheduleData.schedule) != taboo.end()) { //checks if it exists in the taboo list
                    continue;
                }
                calculateTotalPenalty(tempScheduleData, orders, setupTimes, initialSetupTimes);
                if (const double newPenalty = tempScheduleData.totalPenalty; newPenalty == bestPenalty) {
                    taboo[tempScheduleData.schedule] = newPenalty;
                }
                else if (newPenalty < bestPenalty) {
                    bestPenalty = newPenalty;
                    best_i = i;
                    best_j = j;
                    best_l = l;
                    improvementFound = true;
                    taboo.clear();
                    taboo[tempScheduleData.schedule] = newPenalty;
                }
            }
        }
    }

    if (improvementFound) {
        // Apply the best block exchange to the actual schedule
        std::swap_ranges(scheduleData.schedule.begin() + best_i, scheduleData.schedule.begin() + best_i + best_l,
                         scheduleData.schedule.begin() + best_j);

        // Recalculate total penalty for the actual schedule
        calculateTotalPenalty(scheduleData, orders, setupTimes, initialSetupTimes);

        return true;
    }

    return false;
}

// Reinsertion Neighborhood (Shifts a block of jobs, or a single one to another position)
bool reinsertionNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                            const std::vector<std::vector<int>> &setupTimes,
                            const std::vector<int> &initialSetupTimes, std::unordered_map<std::vector<int>, double, VectorHash> &taboo)
{
    int n = scheduleData.schedule.size();
    int best_i = -1, best_j = -1, best_l = -1;
    bool improvementFound = false;
    double bestPenalty = scheduleData.totalPenalty;

    // Consider block sizes from 1 to 5
    for (int l = 1; l <= 10; ++l)
    {
        for (int i = 0; i <= n - l; ++i)
        {
            for (int j = 0; j <= n - l; ++j)
            {
                if (j >= i && j <= i + l - 1) continue;  // Skip overlapping positions

                ScheduleData tempScheduleData = scheduleData;

                // Perform block shift in the temporary schedule
                std::vector<int> block(tempScheduleData.schedule.begin() + i, tempScheduleData.schedule.begin() + i + l);
                tempScheduleData.schedule.erase(tempScheduleData.schedule.begin() + i, tempScheduleData.schedule.begin() + i + l);

                // Adjust the insertion position if necessary
                int adjusted_j = j;
                if (j > i)
                {
                    adjusted_j -= l;
                }

                tempScheduleData.schedule.insert(tempScheduleData.schedule.begin() + adjusted_j, block.begin(), block.end());

                if (taboo.find(tempScheduleData.schedule) != taboo.end()) { //checks if it exists in the taboo list
                    continue;
                }
                calculateTotalPenalty(tempScheduleData, orders, setupTimes, initialSetupTimes);
                if (const double newPenalty = tempScheduleData.totalPenalty; newPenalty == bestPenalty) {
                    taboo[tempScheduleData.schedule] = newPenalty;
                }
                else if (newPenalty < bestPenalty) {
                    bestPenalty = newPenalty;
                    best_i = i;
                    best_j = j;
                    best_l = l;
                    improvementFound = true;
                    taboo.clear();
                    taboo[tempScheduleData.schedule] = newPenalty;
                }
            }
        }
    }

    if (improvementFound)
    {
        // Apply the best block shift to the actual schedule
        std::vector<int> block(scheduleData.schedule.begin() + best_i, scheduleData.schedule.begin() + best_i + best_l);
        scheduleData.schedule.erase(scheduleData.schedule.begin() + best_i, scheduleData.schedule.begin() + best_i + best_l);

        // Adjust the insertion position if necessary
        int adjusted_j = best_j;
        if (best_j > best_i)
        {
            adjusted_j -= best_l;
        }

        scheduleData.schedule.insert(scheduleData.schedule.begin() + adjusted_j, block.begin(), block.end());

        // Recalculate total penalty for the actual schedule
        calculateTotalPenalty(scheduleData, orders, setupTimes, initialSetupTimes);

        return true;
    }

    return false;
}

// 2-Opt Neighborhood (Reverses two segments of the schedule)
bool twoOptNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                        const std::vector<std::vector<int>> &setupTimes,
                        const std::vector<int> &initialSetupTimes, std::unordered_map<std::vector<int>, double, VectorHash> &taboo) {

    int n = scheduleData.schedule.size();
    int best_i = -1, best_j = -1;
    bool improvementFound = false;
    double bestPenalty = scheduleData.totalPenalty;

    for (int i = 0; i < n - 1; ++i) {
        // Limit j to ensure the block size does not exceed 10
        const int max_j = std::min(n - 1, i + 4); // i + 9 ensures block size <= 10
        for (int j = i + 1; j <= max_j; ++j) {
            ScheduleData tempScheduleData = scheduleData;

            // Apply the 2-opt move in the temporary schedule
            std::reverse(tempScheduleData.schedule.begin() + i, tempScheduleData.schedule.begin() + j + 1);

            // Recalculate total penalty for the temporary schedule
            if (taboo.find(tempScheduleData.schedule) != taboo.end()) { //checks if it exists in the taboo list
                continue;
            }
            calculateTotalPenalty(tempScheduleData, orders, setupTimes, initialSetupTimes);
            if (const double newPenalty = tempScheduleData.totalPenalty; newPenalty == bestPenalty) {
                taboo[tempScheduleData.schedule] = newPenalty;
            }
            else if (newPenalty < bestPenalty) {
                bestPenalty = newPenalty;
                best_i = i;
                best_j = j;
                improvementFound = true;
                taboo.clear();
                taboo[tempScheduleData.schedule] = newPenalty;
            }
        }
    }

    if (improvementFound) {
        // Apply the best 2-opt move to the actual schedule
        std::reverse(scheduleData.schedule.begin() + best_i, scheduleData.schedule.begin() + best_j + 1);

        // Recalculate total penalty for the actual schedule
        calculateTotalPenalty(scheduleData, orders, setupTimes, initialSetupTimes);

        return true;
    }

    return false;
}

