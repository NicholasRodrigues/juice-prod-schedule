#include "algorithm.h"
#include "neighborhoods.h"
#include <iostream>
#include <chrono>
#include <unordered_set>
#include <vector>
#include <functional>
#include <algorithm>
#include <random>
#include <numeric>
#include <set>

double calculateTotalPenalty(const std::vector<int>& schedule,
                             const std::vector<Order>& orders,
                             const std::vector<std::vector<int>>& setupTimes,
                             const std::vector<int>& initialSetupTimes) {
    double totalPenaltyCost = 0.0;
    int currentTime = 0;
    int currentTask = -1;

    for (size_t i = 0; i < schedule.size(); ++i) {
        int taskId = schedule[i];
        const Order& order = orders[taskId];

        int setupTime = 0;
        if (currentTask >= 0) {
            setupTime = setupTimes[currentTask][taskId];
        } else {
            setupTime = initialSetupTimes[taskId];
        }
        currentTime += setupTime;
        currentTime += order.processingTime;

        if (currentTime > order.dueTime) {
            double penalty = order.penaltyRate * (currentTime - order.dueTime);
            totalPenaltyCost += penalty;
        }

        currentTask = taskId;
    }

    return totalPenaltyCost;
}


// Tabu list using solution encoding
std::unordered_set<std::string> tabuList;

// Symmetric matrix for solution encoding
std::vector<std::vector<int>> U; // Symmetric matrix

void initializeMatrixU(int n) {
    U.resize(n, std::vector<int>(n));
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(1, 1'000'000);

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            U[i][j] = U[j][i] = dist(rng);
        }
    }
}

std::string encodeSolution(const std::vector<int>& schedule) {
    int g = 0;
    for (size_t i = 1; i < schedule.size(); ++i) {
        g += U[schedule[i-1]][schedule[i]];
    }
    return std::to_string(g);
}

// Constructive Heuristic (Randomized Insertion)
std::vector<int> randomizedInsertionHeuristic(const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes) {
    int n = orders.size();
    std::vector<int> schedule;
    std::vector<bool> scheduled(n, false);

    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, n - 1);

    // Select 10% random jobs
    int initialJobs = std::max(1, n / 10);
    for (int i = 0; i < initialJobs; ++i) {
        int randomJob;
        do {
            randomJob = dist(rng);
        } while (scheduled[randomJob]);

        schedule.push_back(randomJob);
        scheduled[randomJob] = true;
    }

    // Complete sequence with insertion heuristic
    while (schedule.size() < n) {
        int bestTask = -1;
        int bestInsertPos = 0;
        double bestCost = std::numeric_limits<double>::infinity();

        for (int unscheduledJob = 0; unscheduledJob < n; ++unscheduledJob) {
            if (scheduled[unscheduledJob]) continue;

            for (size_t pos = 0; pos <= schedule.size(); ++pos) {
                std::vector<int> tempSchedule = schedule;
                tempSchedule.insert(tempSchedule.begin() + pos, unscheduledJob);

                double currentCost = calculateTotalPenalty(tempSchedule, orders, setupTimes, setupTimes[0]);
                if (currentCost < bestCost) {
                    bestCost = currentCost;
                    bestTask = unscheduledJob;
                    bestInsertPos = pos;
                }
            }
        }

        schedule.insert(schedule.begin() + bestInsertPos, bestTask);
        scheduled[bestTask] = true;
    }

    return schedule;
}

// RVND (Randomized Variable Neighborhood Descent)
std::vector<int> RVND(std::vector<int>& schedule, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes) {
    std::vector<std::function<bool(std::vector<int>&, const std::vector<Order>&,
    const std::vector<std::vector<int>>&,
    const std::vector<int>&)>> neighborhoods = {
            reinsertionNeighborhood,
            twoOptNeighborhood,
            swapNeighborhood,
            [&](std::vector<int>& sched, const std::vector<Order>& ord, const std::vector<std::vector<int>>& setup, const std::vector<int>& initial) {
                return blockInsertNeighborhood(sched, ord, setup, initial, 2); // Example block size 2
            },
            [&](std::vector<int>& sched, const std::vector<Order>& ord, const std::vector<std::vector<int>>& setup, const std::vector<int>& initial) {
                return blockReverseNeighborhood(sched, ord, setup, initial, 3); // Example block size 3
            }
    };

    bool improvement = true;
    std::mt19937 rng(std::random_device{}());

    while (improvement) {
        improvement = false;
        std::shuffle(neighborhoods.begin(), neighborhoods.end(), rng);

        for (auto& neighborhood : neighborhoods) {
            if (neighborhood(schedule, orders, setupTimes, setupTimes[0])) {
                improvement = true;
                break;
            }
        }
    }

    return schedule;
}

// Double-Bridge Perturbation for ILS
void perturbSolution(std::vector<int>& schedule) {
    int n = schedule.size();
    if (n < 8) return;

    std::mt19937 rng(std::random_device{}());
    int segmentSize = n / 4;
    std::uniform_int_distribution<int> posDist(1, n - 1);

    int pos1 = posDist(rng) % (n - 3 * segmentSize);
    int pos2 = pos1 + segmentSize;
    int pos3 = pos2 + segmentSize;
    int pos4 = pos3 + segmentSize;

    if (pos4 >= n) {
        pos1 = 0;
        pos2 = pos1 + segmentSize;
        pos3 = pos2 + segmentSize;
        pos4 = pos3 + segmentSize;
    }

    std::vector<int> part1(schedule.begin() + pos1, schedule.begin() + pos2);
    std::vector<int> part2(schedule.begin() + pos2, schedule.begin() + pos3);
    std::vector<int> part3(schedule.begin() + pos3, schedule.begin() + pos4);

    schedule.erase(schedule.begin() + pos1, schedule.begin() + pos4);
    schedule.insert(schedule.begin() + pos1, part3.begin(), part3.end());
    schedule.insert(schedule.begin() + pos1 + part3.size(), part1.begin(), part1.end());
    schedule.insert(schedule.begin() + pos1 + part3.size() + part1.size(), part2.begin(), part2.end());
}

// ILS-RVND Implementation
// ILS-RVND Implementation with structured output
// ILS-RVND Implementation with structured output for the best solution
// ILS-RVND Implementation with structured output for the best solution
std::vector<int> ILS_RVND(int maxIter, int maxIterLS, const std::vector<Order>& orders, const std::vector<std::vector<int>>& setupTimes) {
    double bestPenalty = std::numeric_limits<double>::infinity();
    std::vector<int> bestSolution;
    initializeMatrixU(orders.size());

    for (int i = 0; i < maxIter; ++i) {
        int iterILS = 0;

        // Initial Solution
        std::vector<int> s = randomizedInsertionHeuristic(orders, setupTimes);
        double currentPenalty = calculateTotalPenalty(s, orders, setupTimes, setupTimes[0]);

        while (iterILS < maxIterLS) {
            perturbSolution(s); // Diversification (Perturb)
            s = RVND(s, orders, setupTimes); // Intensification (LS)
            double newPenalty = calculateTotalPenalty(s, orders, setupTimes, setupTimes[0]);

            // Solution comparison with encoded version for O(1) lookup
            std::string encodedSolution = encodeSolution(s);
            if (tabuList.find(encodedSolution) != tabuList.end()) {
                perturbSolution(s);
                continue;
            }
            tabuList.insert(encodedSolution);

            if (newPenalty < currentPenalty) {
                currentPenalty = newPenalty;
                iterILS = 0; // Reset ILS iteration counter
            } else {
                iterILS++;
            }
        }

        // Update best solution if improved
        if (currentPenalty < bestPenalty) {
            bestPenalty = currentPenalty;
            bestSolution = s;

            // Structured output: Best solution update
            std::cout << "=============================================" << std::endl;
            std::cout << "ILS iteration " << i + 1 << ": Best solution updated" << std::endl;
            std::cout << "Best Penalty: " << bestPenalty << std::endl;
            std::cout << "Best Schedule: [";
            for (size_t j = 0; j < bestSolution.size(); ++j) {
                std::cout << bestSolution[j];
                if (j != bestSolution.size() - 1) std::cout << ", ";
            }
            std::cout << "]" << std::endl;
            std::cout << "=============================================" << std::endl;
        }
    }

    double finalPenalty = calculateTotalPenalty(bestSolution, orders, setupTimes, setupTimes[0]); // Recalculate the penalty for the best solution
    std::cout << "Final Best Solution After All Iterations: " << std::endl;
    std::cout << "Best Penalty: " << finalPenalty << std::endl;
    std::cout << "Best Schedule: [";
    for (size_t j = 0; j < bestSolution.size(); ++j) {
        std::cout << bestSolution[j];
        if (j != bestSolution.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    return bestSolution;
}

