#include <iostream>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include "algorithm.h"
#include "parser.h"
// #include <sys/resource.h>

// void printMemoryUsage() {
//     struct rusage usage;
//     getrusage(RUSAGE_SELF, &usage);
//     std::cout << "Maximum resident set size: " << usage.ru_maxrss << " kilobytes" << std::endl;
// }

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    // Check if a file was passed as an argument
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <instance_file_path>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];
    if (!fs::exists(filepath))
    {
        std::cerr << "Error: File does not exist: " << filepath << std::endl;
        return 1;
    }

    std::vector<Order> orders;
    std::vector<std::vector<int>> setupTimes;
    std::vector<int> initialSetupTimes;

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
        {"n60P", 396183}};

    std::string filename = fs::path(filepath).filename().string();
    std::string instanceName = filename.substr(0, filename.find('.'));

    std::cout << "Processing file: " << filepath << std::endl;

    parseInputFile(filepath, orders, setupTimes, initialSetupTimes);

    double totalPenaltyCost = 0.0;
    auto start = std::chrono::high_resolution_clock::now();

//    // Generate initial greedy schedule
//    std::vector<int> initialSchedule = GRASP(orders, setupTimes, initialSetupTimes, totalPenaltyCost);
//
//    // Output in a parsable format for the shell script
//    std::cout << "INITIAL_SCHEDULE: ";
//    for (int i : initialSchedule)
//    {
//        std::cout << i << " ";
//    }
//    std::cout << std::endl;
//    std::cout << "TOTAL_PENALTY_GREEDY: " << totalPenaltyCost << std::endl;

    // Calculate GAP for greedy solution
//    double greedyGAP = 0.0;
//    if (optimalPenalties.find(instanceName) != optimalPenalties.end())
//    {
//        double optimalPenalty = optimalPenalties[instanceName];
//        greedyGAP = ((totalPenaltyCost - optimalPenalty) / optimalPenalty) * 100.0;
//        std::cout << "GAP_GREEDY: " << greedyGAP << "%" << std::endl;
//    }

    // Perform ILS optimization with Adaptive RVND
    std::vector<int> optimizedSchedule = GRASP(orders, setupTimes, initialSetupTimes, totalPenaltyCost);

    // Post-optimization cost calculation
    double totalPenaltyAfterOptimization = calculateTotalPenalty(optimizedSchedule, orders, setupTimes, initialSetupTimes);

    // Output optimized results
    std::cout << "OPTIMIZED_SCHEDULE: ";
    for (int i : optimizedSchedule)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "TOTAL_PENALTY_OPTIMIZED: " << totalPenaltyAfterOptimization << std::endl;

    // Calculate GAP for optimized solution
    double optimizedGAP = ((totalPenaltyAfterOptimization - optimalPenalties[instanceName]) / optimalPenalties[instanceName]) * 100.0;
    std::cout << "GAP_OPTIMIZED: " << optimizedGAP << "%" << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "EXECUTION_TIME: " << elapsed.count() << std::endl;

    // printMemoryUsage();

    return 0;
}
