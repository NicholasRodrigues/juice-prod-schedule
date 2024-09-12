#include <iostream>
#include <vector>
#include <filesystem>
#include "algorithm.h"
#include "parser.h"

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

        std::vector<int> schedule = advancedGreedyAlgorithmWithDynamicWeight(orders, setupTimes, finalSetupTimeWeight, totalPenaltyCost, totalCost);

        std::cout << "Dynamic advanced greedy schedule for " << filename << ": ";
        for (int i : schedule) {
            std::cout << i << " ";
        }
        std::cout << std::endl;

        std::cout << "Total penalty cost for " << filename << ": " << totalPenaltyCost << std::endl;
        std::cout << "Total cost for " << filename << ": " << totalCost << std::endl;
    }

    return 0;
}
