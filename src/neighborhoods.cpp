#include "neighborhoods.h"
#include <algorithm>
#include <vector>
#include <iostream>

const double IMPROVEMENT_THRESHOLD = 1e-6; // Ajuste conforme necessário

bool swapNeighborhood(std::vector<int> &schedule, const std::vector<Order> &orders,
                      const std::vector<std::vector<int>> &setupTimes, double &totalCost)
{
    int n = schedule.size();

    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            double deltaCost = calculateSwapCostDifference(schedule, orders, setupTimes, i, j);

            if (deltaCost < -IMPROVEMENT_THRESHOLD)
            {
                std::swap(schedule[i], schedule[j]);
                totalCost += deltaCost;
                return true;
            }
        }
    }

    return false;
}

double calculateSwapCostDifference(const std::vector<int> &schedule, const std::vector<Order> &orders,
                                   const std::vector<std::vector<int>> &setupTimes, int i, int j)
{
    int n = schedule.size();

    int start = std::min(i, j);

    int currentTime = 0;
    int currentTask = -1;
    for (int k = 0; k < start; ++k)
    {
        int taskId = schedule[k];
        if (currentTask >= 0)
        {
            currentTime += setupTimes[currentTask][taskId];
        }
        currentTime += orders[taskId].processingTime;
        currentTask = taskId;
    }

    int oldCurrentTime = currentTime;
    int oldCurrentTask = currentTask;

    int end = std::max(i, j);
    double oldCost = 0.0;
    currentTime = oldCurrentTime;
    currentTask = oldCurrentTask;
    for (int k = start; k <= end; ++k)
    {
        int taskId = schedule[k];
        if (currentTask >= 0)
        {
            int setup = setupTimes[currentTask][taskId];
            currentTime += setup;
            oldCost += setup;
        }
        currentTime += orders[taskId].processingTime;

        // Penalidade
        if (currentTime > orders[taskId].dueTime)
        {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            oldCost += penalty;
        }

        currentTask = taskId;
    }

    std::vector<int> newSchedule = schedule;
    std::swap(newSchedule[i], newSchedule[j]);

    double newCost = 0.0;
    currentTime = oldCurrentTime;
    currentTask = oldCurrentTask;
    for (int k = start; k <= end; ++k)
    {
        int taskId = newSchedule[k];
        if (currentTask >= 0)
        {
            int setup = setupTimes[currentTask][taskId];
            currentTime += setup;
            newCost += setup;
        }
        currentTime += orders[taskId].processingTime;

        // Penalidade
        if (currentTime > orders[taskId].dueTime)
        {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            newCost += penalty;
        }

        currentTask = taskId;
    }

    double deltaCost = newCost - oldCost;
    return deltaCost;
}

bool reinsertionNeighborhood(std::vector<int> &schedule, const std::vector<Order> &orders,
                             const std::vector<std::vector<int>> &setupTimes, double &totalCost)
{
    int n = schedule.size();

    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j <= n; ++j)
        {
            if (i == j || i == j - 1)
                continue;

            double deltaCost = calculateJanelsReinsertionCostDifference(schedule, orders, setupTimes, i, j);

            if (deltaCost < -IMPROVEMENT_THRESHOLD)
            {
                int task = schedule[i];
                schedule.erase(schedule.begin() + i);
                int insertPos = (j > i) ? j - 1 : j;
                schedule.insert(schedule.begin() + insertPos, task);
                totalCost += deltaCost;
                return true; 
            }
        }
    }

    return false; 
}

double calculateJanelsReinsertionCostDifference(const std::vector<int> &schedule, const std::vector<Order> &orders,
                                                const std::vector<std::vector<int>> &setupTimes, int i, int j)
{
    int n = schedule.size();

    int start = std::min(i, (j == n) ? n - 1 : j);

    int currentTime = 0;
    int currentTask = -1;
    for (int k = 0; k < start; ++k)
    {
        int taskId = schedule[k];
        if (currentTask >= 0)
        {
            currentTime += setupTimes[currentTask][taskId];
        }
        currentTime += orders[taskId].processingTime;
        currentTask = taskId;
    }

    int oldCurrentTime = currentTime;
    int oldCurrentTask = currentTask;

    double oldCost = 0.0;
    currentTime = oldCurrentTime;
    currentTask = oldCurrentTask;
    for (int k = start; k < n; ++k)
    {
        int taskId = schedule[k];
        if (currentTask >= 0)
        {
            int setup = setupTimes[currentTask][taskId];
            currentTime += setup;
            oldCost += setup;
        }
        currentTime += orders[taskId].processingTime;

        // Penalidade
        if (currentTime > orders[taskId].dueTime)
        {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            oldCost += penalty;
        }

        currentTask = taskId;
    }

    std::vector<int> newSchedule = schedule;
    int task = newSchedule[i];
    newSchedule.erase(newSchedule.begin() + i);
    int insertPos = (j > i) ? j - 1 : j;
    newSchedule.insert(newSchedule.begin() + insertPos, task);

    double newCost = 0.0;
    currentTime = oldCurrentTime;
    currentTask = oldCurrentTask;
    for (int k = start; k < n; ++k)
    {
        int taskId = newSchedule[k];
        if (currentTask >= 0)
        {
            int setup = setupTimes[currentTask][taskId];
            currentTime += setup;
            newCost += setup;
        }
        currentTime += orders[taskId].processingTime;

        if (currentTime > orders[taskId].dueTime)
        {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            newCost += penalty;
        }

        currentTask = taskId;
    }

    double deltaCost = newCost - oldCost;

    const double PENALTY_RATE_THRESHOLD = 10.0; 

    if (orders[task].penaltyRate < PENALTY_RATE_THRESHOLD)
    {
        return 0.0;
    }

    return deltaCost;
}

bool blockSwapNeighborhood(std::vector<int> &schedule, const std::vector<Order> &orders,
                           const std::vector<std::vector<int>> &setupTimes, double &totalCost)
{
    int n = schedule.size();

    const int MIN_BLOCK_SIZE = 2;
    const int MAX_BLOCK_SIZE = 5; 

    for (int blockSize = MIN_BLOCK_SIZE; blockSize <= MAX_BLOCK_SIZE; ++blockSize)
    {
        for (int i = 0; i <= n - blockSize; ++i)
        {
            for (int j = i + blockSize; j <= n - blockSize; ++j)
            {
                double deltaCost = calculateBlockSwapCostDifference(schedule, orders, setupTimes, i, i + blockSize - 1, j, j + blockSize - 1);

                if (deltaCost < -IMPROVEMENT_THRESHOLD)
                {
                    std::vector<int> block1(schedule.begin() + i, schedule.begin() + i + blockSize);
                    std::vector<int> block2(schedule.begin() + j, schedule.begin() + j + blockSize);

                    schedule.erase(schedule.begin() + j, schedule.begin() + j + blockSize);
                    schedule.erase(schedule.begin() + i, schedule.begin() + i + blockSize);

                    schedule.insert(schedule.begin() + i, block2.begin(), block2.end());
                    schedule.insert(schedule.begin() + j, block1.begin(), block1.end());

                    totalCost += deltaCost;
                    return true; 
                }
            }
        }
    }

    return false; 
}

double calculateBlockSwapCostDifference(const std::vector<int> &schedule, const std::vector<Order> &orders,
                                        const std::vector<std::vector<int>> &setupTimes, int start1, int end1, int start2, int end2)
{
    std::vector<int> newSchedule = schedule;

    std::vector<int> block1(newSchedule.begin() + start1, newSchedule.begin() + end1 + 1);
    std::vector<int> block2(newSchedule.begin() + start2, newSchedule.begin() + end2 + 1);

    newSchedule.erase(newSchedule.begin() + start2, newSchedule.begin() + end2 + 1);
    newSchedule.erase(newSchedule.begin() + start1, newSchedule.begin() + end1 + 1);

    newSchedule.insert(newSchedule.begin() + start1, block2.begin(), block2.end());
    newSchedule.insert(newSchedule.begin() + start2, block1.begin(), block1.end());

    double oldCost = 0.0;
    double newCost = 0.0;
    int currentTime = 0;
    int currentTask = -1;

    for (size_t k = 0; k < schedule.size(); ++k)
    {
        int taskId = schedule[k];
        if (currentTask >= 0)
        {
            oldCost += setupTimes[currentTask][taskId];
            currentTime += setupTimes[currentTask][taskId];
        }
        currentTime += orders[taskId].processingTime;

        if (currentTime > orders[taskId].dueTime)
        {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            oldCost += penalty;
        }

        currentTask = taskId;
    }

    currentTime = 0;
    currentTask = -1;
    for (size_t k = 0; k < newSchedule.size(); ++k)
    {
        int taskId = newSchedule[k];
        if (currentTask >= 0)
        {
            newCost += setupTimes[currentTask][taskId];
            currentTime += setupTimes[currentTask][taskId];
        }
        currentTime += orders[taskId].processingTime;

        if (currentTime > orders[taskId].dueTime)
        {
            double penalty = orders[taskId].penaltyRate * (currentTime - orders[taskId].dueTime);
            newCost += penalty;
        }

        currentTask = taskId;
    }

    double deltaCost = newCost - oldCost;
    return deltaCost;
}