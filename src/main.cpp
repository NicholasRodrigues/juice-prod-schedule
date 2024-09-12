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

        std::vector<int> schedule = advancedGreedyAlgorithmWithDynamicWeight(orders, setupTimes, finalSetupTimeWeight);

        std::cout << "Dynamic advanced greedy schedule for " << filename << ": ";
        for (int i : schedule) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
