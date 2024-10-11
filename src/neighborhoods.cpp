#include "neighborhoods.h"
#include "algorithm.h"
#include <algorithm>
#include <set>
#include <iostream>
#include "schedule_data.h"

// Swap Neighborhood Function
bool swapNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                      const std::vector<std::vector<int>> &setupTimes,
                      const std::vector<int> &initialSetupTimes) {

    int n = scheduleData.schedule.size();
    int best_i = -1, best_j = -1;
    bool improvementFound = false;
    double bestPenalty = scheduleData.totalPenalty;

    ScheduleData tempScheduleData;

    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            tempScheduleData = scheduleData;

            // Swap positions i and j in the temporary schedule
            std::swap(tempScheduleData.schedule[i], tempScheduleData.schedule[j]);

            // Recalculate total penalty for the temporary schedule
            calculateTotalPenalty(tempScheduleData, orders, setupTimes, initialSetupTimes);
            double newPenalty = tempScheduleData.totalPenalty;

            if (newPenalty < bestPenalty) {
                bestPenalty = newPenalty;
                best_i = i;
                best_j = j;
                improvementFound = true;
            }
        }
    }

    if (improvementFound) {
        // Apply the best swap to the actual schedule
        std::swap(scheduleData.schedule[best_i], scheduleData.schedule[best_j]);

        // Recalculate total penalty for the actual schedule
        calculateTotalPenalty(scheduleData, orders, setupTimes, initialSetupTimes);

        // Increment the improvement counter
        swap_improvement_count++;
        return true;
    }

    return false;
}



double computeDeltaPenaltySwap(const ScheduleData &scheduleData, const std::vector<Order> &orders,
                               const std::vector<std::vector<int>> &setupTimes,
                               const std::vector<int> &initialSetupTimes,
                               int i, int j)
{
    int n = scheduleData.schedule.size();

    // Swapping tasks at positions i and j
    std::vector<int> newSchedule = scheduleData.schedule;
    std::swap(newSchedule[i], newSchedule[j]);

    // Determine the range of positions affected by the swap
    int startPos = (i > 0) ? i - 1 : 0;
    int endPos = std::min(n - 1, j + 1);

    // Recompute arrival times and penalties for affected positions
    int currentTime;
    int currentTask;

    if (startPos == 0)
    {
        currentTime = 0;
        currentTask = -1;
    }
    else
    {
        currentTime = scheduleData.arrivalTimes[startPos - 1];
        currentTask = newSchedule[startPos - 1];
    }

    double deltaPenalty = 0.0;

    for (int idx = startPos; idx <= endPos; ++idx)
    {
        int taskId = newSchedule[idx];
        const Order &order = orders[taskId];

        int setupTime = (currentTask >= 0) ? setupTimes[currentTask][taskId] : initialSetupTimes[taskId];
        currentTime += setupTime + order.processingTime;

        double oldPenalty = scheduleData.penalties[idx];

        double penalty = 0.0;
        if (currentTime > order.dueTime)
        {
            penalty = order.penaltyRate * (currentTime - order.dueTime);
        }

        deltaPenalty += penalty - oldPenalty;

        currentTask = taskId;
    }

    return deltaPenalty;
}

void updateScheduleDataSwap(ScheduleData &scheduleData, const std::vector<Order> &orders,
                            const std::vector<std::vector<int>> &setupTimes,
                            const std::vector<int> &initialSetupTimes,
                            int i, int j)
{
    int n = scheduleData.schedule.size();

    // Determine the range of positions affected by the swap
    int startPos = (i > 0) ? i - 1 : 0;
    int endPos = std::min(n - 1, j + 1);

    // Recompute arrival times and penalties for affected positions
    int currentTime;
    int currentTask;

    if (startPos == 0)
    {
        currentTime = 0;
        currentTask = -1;
    }
    else
    {
        currentTime = scheduleData.arrivalTimes[startPos - 1];
        currentTask = scheduleData.schedule[startPos - 1];
    }

    for (int idx = startPos; idx <= endPos; ++idx)
    {
        int taskId = scheduleData.schedule[idx];
        const Order &order = orders[taskId];

        int setupTime = (currentTask >= 0) ? setupTimes[currentTask][taskId] : initialSetupTimes[taskId];
        currentTime += setupTime + order.processingTime;

        scheduleData.arrivalTimes[idx] = currentTime;

        double penalty = 0.0;
        if (currentTime > order.dueTime)
        {
            penalty = order.penaltyRate * (currentTime - order.dueTime);
        }
        scheduleData.penalties[idx] = penalty;

        currentTask = taskId;
    }
}



// Block Exchange Neighborhood (Exchanges two blocks)
bool blockExchangeNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                               const std::vector<std::vector<int>> &setupTimes,
                               const std::vector<int> &initialSetupTimes) {

    int n = scheduleData.schedule.size();
    int best_i = -1, best_j = -1, best_l = -1;
    bool improvementFound = false;
    double bestPenalty = scheduleData.totalPenalty;

    ScheduleData tempScheduleData;

    // Consider block sizes between 2 and 4
    for (int l = 1; l <= 10; ++l) {
        for (int i = 0; i <= n - 2 * l; ++i) {
            for (int j = i + l; j <= n - l; ++j) {
                tempScheduleData = scheduleData;

                // Perform block exchange in the temporary schedule
                std::swap_ranges(tempScheduleData.schedule.begin() + i, tempScheduleData.schedule.begin() + i + l,
                                 tempScheduleData.schedule.begin() + j);

                // Recalculate total penalty for the temporary schedule
                calculateTotalPenalty(tempScheduleData, orders, setupTimes, initialSetupTimes);
                double newPenalty = tempScheduleData.totalPenalty;

                if (newPenalty < bestPenalty) {
                    bestPenalty = newPenalty;
                    best_i = i;
                    best_j = j;
                    best_l = l;
                    improvementFound = true;
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

        // Increment the improvement counter
        block_exchange_improvement_count++;
        return true;
    }

    return false;
}


double computeDeltaPenaltyBlockExchange(const ScheduleData &scheduleData, const std::vector<Order> &orders,
                                        const std::vector<std::vector<int>> &setupTimes,
                                        const std::vector<int> &initialSetupTimes,
                                        int i, int j, int l)
{
    int n = scheduleData.schedule.size();

    // Create a copy of the schedule and perform the block exchange
    std::vector<int> newSchedule = scheduleData.schedule;
    std::swap_ranges(newSchedule.begin() + i, newSchedule.begin() + i + l,
                     newSchedule.begin() + j);

    // Determine the range of positions affected by the block exchange
    int startPos = (i > 0) ? i - 1 : 0;
    int endPos = std::min(n - 1, std::max(i + l, j + l));

    // Recompute arrival times and penalties for affected positions
    int currentTime;
    int currentTask;

    if (startPos == 0)
    {
        currentTime = 0;
        currentTask = -1;
    }
    else
    {
        currentTime = scheduleData.arrivalTimes[startPos - 1];
        currentTask = newSchedule[startPos - 1];
    }

    double deltaPenalty = 0.0;

    for (int idx = startPos; idx <= endPos; ++idx)
    {
        int taskId = newSchedule[idx];
        const Order &order = orders[taskId];

        int setupTime = (currentTask >= 0) ? setupTimes[currentTask][taskId] : initialSetupTimes[taskId];
        currentTime += setupTime + order.processingTime;

        double oldPenalty = scheduleData.penalties[idx];

        double penalty = 0.0;
        if (currentTime > order.dueTime)
        {
            penalty = order.penaltyRate * (currentTime - order.dueTime);
        }

        deltaPenalty += penalty - oldPenalty;

        currentTask = taskId;
    }

    return deltaPenalty;
}


void updateScheduleDataBlockExchange(ScheduleData &scheduleData, const std::vector<Order> &orders,
                                     const std::vector<std::vector<int>> &setupTimes,
                                     const std::vector<int> &initialSetupTimes,
                                     int i, int j, int l)
{
    int n = scheduleData.schedule.size();

    // Perform the block exchange
    std::swap_ranges(scheduleData.schedule.begin() + i, scheduleData.schedule.begin() + i + l,
                     scheduleData.schedule.begin() + j);

    // Determine the range of positions affected by the block exchange
    int startPos = (i > 0) ? i - 1 : 0;
    int endPos = std::min(n - 1, std::max(i + l, j + l));

    // Recompute arrival times and penalties for affected positions
    int currentTime;
    int currentTask;

    if (startPos == 0)
    {
        currentTime = 0;
        currentTask = -1;
    }
    else
    {
        currentTime = scheduleData.arrivalTimes[startPos - 1];
        currentTask = scheduleData.schedule[startPos - 1];
    }

    for (int idx = startPos; idx <= endPos; ++idx)
    {
        int taskId = scheduleData.schedule[idx];
        const Order &order = orders[taskId];

        int setupTime = (currentTask >= 0) ? setupTimes[currentTask][taskId] : initialSetupTimes[taskId];
        currentTime += setupTime + order.processingTime;

        scheduleData.arrivalTimes[idx] = currentTime;

        double penalty = 0.0;
        if (currentTime > order.dueTime)
        {
            penalty = order.penaltyRate * (currentTime - order.dueTime);
        }
        scheduleData.penalties[idx] = penalty;

        currentTask = taskId;
    }
}


// Block Shift Neighborhood (Shifts a block to another position)
// Block Shift Neighborhood (Shifts a block of jobs to another position)
bool blockShiftNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                            const std::vector<std::vector<int>> &setupTimes,
                            const std::vector<int> &initialSetupTimes)
{
    int n = scheduleData.schedule.size();
    int best_i = -1, best_j = -1, best_l = -1;
    bool improvementFound = false;
    double bestPenalty = scheduleData.totalPenalty;

    ScheduleData tempScheduleData;

    // Consider block sizes from 1 to 5
    for (int l = 1; l <= 10; ++l)
    {
        for (int i = 0; i <= n - l; ++i)
        {
            for (int j = 0; j <= n - l; ++j)
            {
                if (j >= i && j <= i + l - 1) continue;  // Skip overlapping positions

                tempScheduleData = scheduleData;

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

                // Recalculate total penalty for the temporary schedule
                calculateTotalPenalty(tempScheduleData, orders, setupTimes, initialSetupTimes);
                double newPenalty = tempScheduleData.totalPenalty;

                if (newPenalty < bestPenalty)
                {
                    block_shift_improvement_count++;
                    bestPenalty = newPenalty;
                    best_i = i;
                    best_j = j;
                    best_l = l;
                    improvementFound = true;
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

double computeDeltaPenaltyBlockShift(const ScheduleData &scheduleData, const std::vector<Order> &orders,
                                     const std::vector<std::vector<int>> &setupTimes,
                                     const std::vector<int> &initialSetupTimes,
                                     int i, int j, int l)
{
    int n = scheduleData.schedule.size();

    // Create a copy of the schedule and perform the block shift
    std::vector<int> newSchedule = scheduleData.schedule;
    std::vector<int> block(newSchedule.begin() + i, newSchedule.begin() + i + l);
    newSchedule.erase(newSchedule.begin() + i, newSchedule.begin() + i + l);

    // Adjust the insertion position if necessary
    if (j > i)
    {
        j -= l;
    }

    newSchedule.insert(newSchedule.begin() + j, block.begin(), block.end());

    // Determine the range of positions affected by the block shift
    int startPos = (std::min(i, j) > 0) ? std::min(i, j) - 1 : 0;
    int endPos = std::min(n - 1, std::max(i + l, j + l));

    // Recompute arrival times and penalties for affected positions
    int currentTime;
    int currentTask;

    if (startPos == 0)
    {
        currentTime = 0;
        currentTask = -1;
    }
    else
    {
        currentTime = scheduleData.arrivalTimes[startPos - 1];
        currentTask = newSchedule[startPos - 1];
    }

    double deltaPenalty = 0.0;

    for (int idx = startPos; idx <= endPos; ++idx)
    {
        int taskId = newSchedule[idx];
        const Order &order = orders[taskId];

        int setupTime = (currentTask >= 0) ? setupTimes[currentTask][taskId] : initialSetupTimes[taskId];
        currentTime += setupTime + order.processingTime;

        double oldPenalty = scheduleData.penalties[idx];

        double penalty = 0.0;
        if (currentTime > order.dueTime)
        {
            penalty = order.penaltyRate * (currentTime - order.dueTime);
        }

        deltaPenalty += penalty - oldPenalty;

        currentTask = taskId;
    }

    return deltaPenalty;
}

void updateScheduleDataBlockShift(ScheduleData &scheduleData, const std::vector<Order> &orders,
                                  const std::vector<std::vector<int>> &setupTimes,
                                  const std::vector<int> &initialSetupTimes,
                                  int i, int j, int l)
{
    int n = scheduleData.schedule.size();

    // Perform the block shift
    std::vector<int> block(scheduleData.schedule.begin() + i, scheduleData.schedule.begin() + i + l);
    scheduleData.schedule.erase(scheduleData.schedule.begin() + i, scheduleData.schedule.begin() + i + l);

    // Adjust the insertion position if necessary
    if (j > i)
    {
        j -= l;
    }

    scheduleData.schedule.insert(scheduleData.schedule.begin() + j, block.begin(), block.end());

    // Determine the range of positions affected by the block shift
    int startPos = (std::min(i, j) > 0) ? std::min(i, j) - 1 : 0;
    int endPos = std::min(n - 1, std::max(i + l, j + l));

    // Recompute arrival times and penalties for affected positions
    int currentTime;
    int currentTask;

    if (startPos == 0)
    {
        currentTime = 0;
        currentTask = -1;
    }
    else
    {
        currentTime = scheduleData.arrivalTimes[startPos - 1];
        currentTask = scheduleData.schedule[startPos - 1];
    }

    for (int idx = startPos; idx <= endPos; ++idx)
    {
        int taskId = scheduleData.schedule[idx];
        const Order &order = orders[taskId];

        int setupTime = (currentTask >= 0) ? setupTimes[currentTask][taskId] : initialSetupTimes[taskId];
        currentTime += setupTime + order.processingTime;

        scheduleData.arrivalTimes[idx] = currentTime;

        double penalty = 0.0;
        if (currentTime > order.dueTime)
        {
            penalty = order.penaltyRate * (currentTime - order.dueTime);
        }
        scheduleData.penalties[idx] = penalty;

        currentTask = taskId;
    }
}


// 2-Opt Neighborhood (Reverses two segments of the schedule)
bool twoOptNeighborhood(ScheduleData &scheduleData, const std::vector<Order> &orders,
                        const std::vector<std::vector<int>> &setupTimes,
                        const std::vector<int> &initialSetupTimes) {

    int n = scheduleData.schedule.size();
    int best_i = -1, best_j = -1;
    bool improvementFound = false;
    double bestPenalty = scheduleData.totalPenalty;

    ScheduleData tempScheduleData;

    for (int i = 0; i < n - 1; ++i) {
        // Limit j to ensure the block size does not exceed 10
        int max_j = std::min(n - 1, i + 4); // i + 9 ensures block size <= 10
        for (int j = i + 1; j <= max_j; ++j) {
            tempScheduleData = scheduleData;

            // Apply the 2-opt move in the temporary schedule
            std::reverse(tempScheduleData.schedule.begin() + i, tempScheduleData.schedule.begin() + j + 1);

            // Recalculate total penalty for the temporary schedule
            calculateTotalPenalty(tempScheduleData, orders, setupTimes, initialSetupTimes);
            double newPenalty = tempScheduleData.totalPenalty;

            if (newPenalty < bestPenalty) {
                bestPenalty = newPenalty;
                best_i = i;
                best_j = j;
                improvementFound = true;
            }
        }
    }

    if (improvementFound) {
        // Apply the best 2-opt move to the actual schedule
        std::reverse(scheduleData.schedule.begin() + best_i, scheduleData.schedule.begin() + best_j + 1);

        // Recalculate total penalty for the actual schedule
        calculateTotalPenalty(scheduleData, orders, setupTimes, initialSetupTimes);

        // Increment the improvement counter
        two_opt_improvement_count++;
        return true;
    }

    return false;
}



double computeDeltaPenaltyTwoOpt(const ScheduleData &scheduleData, const std::vector<Order> &orders,
                                 const std::vector<std::vector<int>> &setupTimes,
                                 const std::vector<int> &initialSetupTimes,
                                 int i, int j)
{
    int n = scheduleData.schedule.size();

    // Create a copy of the schedule and perform the 2-opt move
    std::vector<int> newSchedule = scheduleData.schedule;
    std::reverse(newSchedule.begin() + i, newSchedule.begin() + j + 1);

    // Determine the range of positions affected by the 2-opt move
    int startPos = (i > 0) ? i - 1 : 0;
    int endPos = j + 1 < n ? j + 1 : j;

    // Recompute arrival times and penalties for affected positions
    int currentTime;
    int currentTask;

    if (startPos == 0)
    {
        currentTime = 0;
        currentTask = -1;
    }
    else
    {
        currentTime = scheduleData.arrivalTimes[startPos - 1];
        currentTask = newSchedule[startPos - 1];
    }

    double deltaPenalty = 0.0;

    for (int idx = startPos; idx <= endPos; ++idx)
    {
        int taskId = newSchedule[idx];
        const Order &order = orders[taskId];

        int setupTime = (currentTask >= 0) ? setupTimes[currentTask][taskId] : initialSetupTimes[taskId];
        currentTime += setupTime + order.processingTime;

        double oldPenalty = scheduleData.penalties[idx];

        double penalty = 0.0;
        if (currentTime > order.dueTime)
        {
            penalty = order.penaltyRate * (currentTime - order.dueTime);
        }

        deltaPenalty += penalty - oldPenalty;

        currentTask = taskId;
    }

    return deltaPenalty;
}

void updateScheduleDataTwoOpt(ScheduleData &scheduleData, const std::vector<Order> &orders,
                              const std::vector<std::vector<int>> &setupTimes,
                              const std::vector<int> &initialSetupTimes,
                              int i, int j)
{
    int n = scheduleData.schedule.size();

    // Perform the 2-opt move
    std::reverse(scheduleData.schedule.begin() + i, scheduleData.schedule.begin() + j + 1);

    // Determine the range of positions affected by the 2-opt move
    int startPos = (i > 0) ? i - 1 : 0;
    int endPos = j + 1 < n ? j + 1 : j;

    // Recompute arrival times and penalties for affected positions
    int currentTime;
    int currentTask;

    if (startPos == 0)
    {
        currentTime = 0;
        currentTask = -1;
    }
    else
    {
        currentTime = scheduleData.arrivalTimes[startPos - 1];
        currentTask = scheduleData.schedule[startPos - 1];
    }

    for (int idx = startPos; idx <= endPos; ++idx)
    {
        int taskId = scheduleData.schedule[idx];
        const Order &order = orders[taskId];

        int setupTime = (currentTask >= 0) ? setupTimes[currentTask][taskId] : initialSetupTimes[taskId];
        currentTime += setupTime + order.processingTime;

        scheduleData.arrivalTimes[idx] = currentTime;

        double penalty = 0.0;
        if (currentTime > order.dueTime)
        {
            penalty = order.penaltyRate * (currentTime - order.dueTime);
        }
        scheduleData.penalties[idx] = penalty;

        currentTask = taskId;
    }
}
