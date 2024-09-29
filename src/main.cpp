#include <iostream>
#include <vector>
#include <filesystem>
#include <unordered_map>
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
    std::vector<int> initialSetupTimes;

    std::string dataPath = "../data/";

    // Map of optimal penalties for each instance
    std::unordered_map<std::string, double> optimalPenalties = {
        {"n60A", 453},
        {"n60B", 1757},
        {"n60C", 0},
        {"n60D", 69102},
        {"n60E", 58935},
        {"n60F", 93045},
        {"n60G", 64632},
        {"n60H", 145007},
        {"n60I", 43286},
        {"n60J", 53555},
        {"n60K", 458752},
        {"n60L", 332941},
        {"n60M", 516926},
        {"n60N", 492572},
        {"n60O", 527459},
        {"n60P", 396183}
    };

    for (const auto& entry : fs::directory_iterator(dataPath)) {
        std::string filepath = entry.path().string();
        std::string filename = entry.path().filename().string();

        // Extract instance name (e.g., "n60A" from "n60A.txt")
        std::string instanceName = filename.substr(0, filename.find('.'));

        std::cout << "Processing file: " << filepath << std::endl;

        parseInputFile(filepath, orders, setupTimes, initialSetupTimes);

        double totalPenaltyCost = 0.0;
        auto start = std::chrono::high_resolution_clock::now();

        // Generate initial greedy schedule
        std::vector<int> initialSchedule = greedyAlgorithm(orders, setupTimes, initialSetupTimes, totalPenaltyCost);

        std::cout << "Initial greedy schedule: ";
        for (int i : initialSchedule) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "Total penalty cost (greedy): " << totalPenaltyCost << std::endl;

        // Calculate GAP for greedy solution
        double greedyGAP = 0.0;
        if (optimalPenalties.find(instanceName) != optimalPenalties.end()) {
            double optimalPenalty = optimalPenalties[instanceName];
            if (optimalPenalty == 0.0) {
                if (totalPenaltyCost == 0.0) {
                    greedyGAP = 0.0;
                } else {
                    greedyGAP = std::numeric_limits<double>::infinity();
                }
            } else {
                greedyGAP = ((totalPenaltyCost - optimalPenalty) / optimalPenalty) * 100.0;
            }
            std::cout << "GAP (greedy) for " << instanceName << ": " << std::fixed << std::setprecision(2) << greedyGAP << "%" << std::endl;
        } else {
            std::cout << "Optimal penalty not found for instance: " << instanceName << std::endl;
        }

        // Perform ILS optimization with Adaptive RVND
        std::vector<int> optimizedSchedule = ILS(initialSchedule, orders, setupTimes, initialSetupTimes);

        // Post-optimization cost calculation
        double totalPenaltyAfterOptimization = calculateTotalPenalty(optimizedSchedule, orders, setupTimes, initialSetupTimes);

        std::cout << "Optimized schedule: ";
        for (int i : optimizedSchedule) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "Total penalty cost after optimization: " << totalPenaltyAfterOptimization << std::endl;

        // Calculate GAP for optimized solution
        double optimizedGAP = 0.0;
        if (optimalPenalties.find(instanceName) != optimalPenalties.end()) {
            double optimalPenalty = optimalPenalties[instanceName];
            if (optimalPenalty == 0.0) {
                if (totalPenaltyAfterOptimization == 0.0) {
                    optimizedGAP = 0.0;
                } else {
                    optimizedGAP = std::numeric_limits<double>::infinity();
                }
            } else {
                optimizedGAP = ((totalPenaltyAfterOptimization - optimalPenalty) / optimalPenalty) * 100.0;
            }
            std::cout << "GAP (optimized) for " << instanceName << ": " << std::fixed << std::setprecision(2) << optimizedGAP << "%" << std::endl;
        } else {
            std::cout << "Optimal penalty not found for instance: " << instanceName << std::endl;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Total execution time for " << filename << ": " << elapsed.count() << " seconds" << std::endl;
        printMemoryUsage();

        orders.clear();
        setupTimes.clear();
        initialSetupTimes.clear();

        std::cout << "----------------------------------------" << std::endl;
    }

    return 0;
}
