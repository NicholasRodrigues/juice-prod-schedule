#include <iostream>
#include <vector>
#include <filesystem>
#include "algorithm.h"
#include "parser.h"
#include <sys/resource.h>

void printMemoryUsage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    std::cout << "Maximum resident set size: " << usage.ru_maxrss << " kilobytes" << std::endl;
}

namespace fs = std::filesystem;

int main() {
    std::vector<Order> orders;
    std::vector<std::vector<int>> setupTimes;

    std::string dataPath = "../data/";
    double finalSetupTimeWeight = 1.0;

    for (const auto& entry : fs::directory_iterator(dataPath)) {
        std::string filename = entry.path().string();

        std::cout << "Processing file: " << filename << std::endl;

        parseInputFile(filename, orders, setupTimes);

        double totalPenaltyCost = 0.0;
        double totalCost = 0.0;
        auto start = std::chrono::high_resolution_clock::now();
        // Generate initial greedy schedule
        std::vector<int> schedule = advancedGreedyAlgorithmWithDynamicWeight(orders, setupTimes,
                                                                             finalSetupTimeWeight,
                                                                             totalPenaltyCost, totalCost);

        std::cout << "Initial greedy schedule: ";
        for (int i : schedule) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "Total penalty cost: " << totalPenaltyCost << std::endl;
        std::cout << "Penalty: " << calculateTotalPenalty(schedule, orders, setupTimes) << std::endl;
        std::cout << "Total cost: " << totalCost << std::endl;

        // Perform RVND optimization
        double totalCostBeforeOptimization = totalCost;
        schedule = RVND(schedule, orders, setupTimes, totalCostBeforeOptimization);

        // Post-optimization cost calculations
        double totalCostAfterOptimization = 0.0;
        double totalPenaltyCostAfterOptimization = 0.0;
        calculateTotalCost(schedule, orders, setupTimes, totalCostAfterOptimization, totalPenaltyCostAfterOptimization);

        std::cout << "Optimized schedule: ";
        for (int i : schedule) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "Total cost after optimization: " << totalCostAfterOptimization << std::endl;
        std::cout << "Total penalty cost after optimization: " << totalPenaltyCostAfterOptimization << std::endl;
        std::cout << "Penalty after optimization: " << calculateTotalPenalty(schedule, orders, setupTimes) << std::endl;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Total execution time for " << filename << ": " << elapsed.count() << " seconds" << std::endl;
        printMemoryUsage();

        orders.clear();
        setupTimes.clear();
    }

    return 0;
}
