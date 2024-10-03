// #include <iostream>
// #include <vector>
// #include <filesystem>
// #include <unordered_map>
// #include <fstream>
// #include "algorithm.h"
// #include "parser.h"

// namespace fs = std::filesystem;

// struct Result
// {
//     std::string instanceName;
//     double optimalPenalty;
//     double GRASPPenalty;
//     double optimizedPenalty;
//     double GRASPGAP;
//     double optimizedGAP;
//     double executionTime;
// };

// int main(int argc, char *argv[])
// {
//     // Check if a file was passed as an argument
//     if (argc != 2)
//     {
//         std::cerr << "Usage: " << argv[0] << " <instance_file_path>" << std::endl;
//         return 1;
//     }

//     std::string filepath = argv[1];
//     if (!fs::exists(filepath))
//     {
//         std::cerr << "Error: File does not exist: " << filepath << std::endl;
//         return 1;
//     }

//     std::vector<Order> orders;
//     std::vector<std::vector<int>> setupTimes;
//     std::vector<int> initialSetupTimes;

//     // Map of optimal penalties for each instance
//     std::unordered_map<std::string, double> optimalPenalties = {
//         {"n60A", 453},
//         {"n60B", 1757},
//         {"n60C", 0},
//         {"n60D", 69102},
//         {"n60E", 58935},
//         {"n60F", 93045},
//         {"n60G", 64632},
//         {"n60H", 145007},
//         {"n60I", 43286},
//         {"n60J", 53555},
//         {"n60K", 458752},
//         {"n60L", 332941},
//         {"n60M", 516926},
//         {"n60N", 492572},
//         {"n60O", 527459},
//         {"n60P", 396183}};

//     std::vector<Result> results;

//     for (const auto &entry : fs::directory_iterator(filepath))
//     {

//         std::string filepath = entry.path().string();
//         std::string filename = entry.path().filename().string();

//         // Extract instance name (e.g., "n60A" from "n60A.txt")
//         std::string instanceName = filename.substr(0, filename.find('.'));

//         std::cout << "Processing file: " << filepath << std::endl;

//         std::cout << "Optimal penalty: " << optimalPenalties[instanceName] << std::endl;

//         parseInputFile(filepath, orders, setupTimes, initialSetupTimes);

//         double totalPenaltyCost = 0.0;
//         auto start = std::chrono::high_resolution_clock::now();

//         // Generate initial greedy schedule
//         std::vector<int> initialSchedule = GRASP(orders, setupTimes, initialSetupTimes, totalPenaltyCost);

//         // Output in a parsable format for the shell script
//         std::cout << "INITIAL_SCHEDULE: ";
//         for (int i : initialSchedule)
//         {
//             std::cout << i << " ";
//         }
//         std::cout << std::endl;
//         std::cout << "TOTAL_PENALTY_GREEDY: " << totalPenaltyCost << std::endl;

//         // Calculate GAP for greedy solution
//         double greedyGAP = 0.0;
//         if (optimalPenalties.find(instanceName) != optimalPenalties.end())
//         {
//             double optimalPenalty = optimalPenalties[instanceName];
//             greedyGAP = ((totalPenaltyCost - optimalPenalty) / optimalPenalty) * 100.0;
//             std::cout << "GAP_GREEDY: " << greedyGAP << "%" << std::endl;
//         }

//         // Perform ILS optimization with Adaptive RVND
//         std::vector<int> optimizedSchedule = ILS(initialSchedule, orders, setupTimes, initialSetupTimes);

//         // Post-optimization cost calculation
//         double totalPenaltyAfterOptimization = calculateTotalPenalty(optimizedSchedule, orders, setupTimes, initialSetupTimes);

//         // Output optimized results
//         std::cout << "OPTIMIZED_SCHEDULE: ";
//         for (int i : optimizedSchedule)
//         {
//             std::cout << i << " ";
//         }
//         std::cout << std::endl;
//         std::cout << "TOTAL_PENALTY_OPTIMIZED: " << totalPenaltyAfterOptimization << std::endl;

//         // Calculate GAP for optimized solution
//         double optimizedGAP = ((totalPenaltyAfterOptimization - optimalPenalties[instanceName]) / optimalPenalties[instanceName]) * 100.0;
//         std::cout << "GAP_OPTIMIZED: " << optimizedGAP << "%" << std::endl;

//         auto end = std::chrono::high_resolution_clock::now();
//         std::chrono::duration<double> elapsed = end - start;
//         std::cout << "EXECUTION_TIME: " << elapsed.count() << " seconds" << std::endl;

//         results.push_back({instanceName, optimalPenalties[instanceName], totalPenaltyCost, totalPenaltyAfterOptimization, greedyGAP, optimizedGAP, elapsed.count()});
//     }

//     // Output results in a table format

//     std::cout << std::setw(10) << "Instance"
//               << std::setw(15) << "Optimal Cost"
//               << std::setw(15) << "GRASP Cost"
//               << std::setw(20) << "Optimized Cost"
//               << std::setw(15) << "GAP (GRASP)"
//               << std::setw(20) << "GAP (Optimized)"
//               << std::setw(15) << "Exec Time (s)"
//               << std::endl;

//     for (const auto &result : results)
//     {
//         std::cout << std::setw(10) << result.instanceName
//                   << std::setw(15) << result.optimalPenalty
//                   << std::setw(15) << result.GRASPPenalty
//                   << std::setw(20) << result.optimizedPenalty
//                   << std::setw(15) << result.GRASPGAP
//                   << std::setw(20) << result.optimizedGAP
//                   << std::setw(15) << result.executionTime
//                   << std::endl;
//     }

//     // Save results to the results/summary.txt file

//     std::ofstream file("results/summary.txt");
//     if (file.is_open())
//     {
//         file << std::setw(10) << "Instance"
//              << std::setw(15) << "Optimal Cost"
//              << std::setw(15) << "GRASP Cost"
//              << std::setw(20) << "Optimized Cost"
//              << std::setw(15) << "GAP (GRASP)"
//              << std::setw(20) << "GAP (Optimized)"
//              << std::setw(15) << "Exec Time (s)"
//              << std::endl;
//         for (const auto &result : results)
//         {
//             file << std::setw(10) << result.instanceName
//                  << std::setw(15) << result.optimalPenalty
//                  << std::setw(15) << result.GRASPPenalty
//                  << std::setw(20) << result.optimizedPenalty
//                  << std::setw(15) << result.GRASPGAP
//                  << std::setw(20) << result.optimizedGAP
//                  << std::setw(15) << result.executionTime
//                  << std::endl;
//         }
//     }
//     else
//     {
//         std::cerr << "Error: Unable to open file for writing: summary.txt" << std::endl;
//     }
//     file.close();
//     return 0;
// }

#include <iostream>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <fstream>
#include <limits>
#include "algorithm.h"
#include "parser.h"

namespace fs = std::filesystem;

struct Result
{
    std::string instanceName;
    double optimalPenalty;
    double GRASPPenalty;
    double optimizedPenalty;
    double GRASPGAP;
    double optimizedGAP;
    double executionTime;
};

bool isValidNumber(double value)
{
    return !(std::isnan(value) || std::isinf(value));
}

void accumulateResults(Result &avgResult, const Result &currentResult, int count)
{
    if (isValidNumber(currentResult.GRASPPenalty))
        avgResult.GRASPPenalty += currentResult.GRASPPenalty / count;
    if (isValidNumber(currentResult.optimizedPenalty))
        avgResult.optimizedPenalty += currentResult.optimizedPenalty / count;
    if (isValidNumber(currentResult.GRASPGAP))
        avgResult.GRASPGAP += currentResult.GRASPGAP / count;
    if (isValidNumber(currentResult.optimizedGAP))
        avgResult.optimizedGAP += currentResult.optimizedGAP / count;
    if (isValidNumber(currentResult.executionTime))
        avgResult.executionTime += currentResult.executionTime / count;
}

int main(int argc, char *argv[])
{
    // Check for file path and benchmark flag
    if (argc < 2 || argc > 3)
    {
        std::cerr << "Usage: " << argv[0] << " <instance_file_path> [benchmark]" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];
    bool benchmark = argc == 3 && std::string(argv[2]) == "benchmark";

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

    std::vector<Result> results;
    const int benchmarkRuns = 10;

    for (const auto &entry : fs::directory_iterator(filepath))
    {
        std::string filepath = entry.path().string();
        std::string filename = entry.path().filename().string();
        std::string instanceName = filename.substr(0, filename.find('.'));

        if (optimalPenalties.find(instanceName) == optimalPenalties.end())
        {
            std::cerr << "Error: Optimal penalty not found for instance: " << instanceName << std::endl;
            continue;
        }

        std::cout << "Processing file: " << filepath << std::endl;

        double optimalPenalty = optimalPenalties[instanceName];
        parseInputFile(filepath, orders, setupTimes, initialSetupTimes);

        Result avgResult = {instanceName, optimalPenalty, 0.0, 0.0, 0.0, 0.0, 0.0};

        int runs = benchmark ? benchmarkRuns : 1;
        for (int run = 0; run < runs; ++run)
        {
            double totalPenaltyCost = 0.0;
            auto start = std::chrono::high_resolution_clock::now();

            std::vector<int> initialSchedule = GRASP(orders, setupTimes, initialSetupTimes, totalPenaltyCost);

            double greedyGAP = ((totalPenaltyCost - optimalPenalty) / optimalPenalty) * 100.0;

            std::vector<int> optimizedSchedule = ILS(initialSchedule, orders, setupTimes, initialSetupTimes);
            double totalPenaltyAfterOptimization = calculateTotalPenalty(optimizedSchedule, orders, setupTimes, initialSetupTimes);

            double optimizedGAP = ((totalPenaltyAfterOptimization - optimalPenalty) / optimalPenalty) * 100.0;

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end - start;

            Result currentResult = {instanceName, optimalPenalty, totalPenaltyCost, totalPenaltyAfterOptimization, greedyGAP, optimizedGAP, elapsed.count()};
            accumulateResults(avgResult, currentResult, runs);

            if (!benchmark)
            {
                results.push_back(currentResult);
                break;
            }
        }

        if (benchmark)
        {
            results.push_back(avgResult);
        }
    }

    // Output results in a table format
    std::cout << std::setw(10) << "Instance"
              << std::setw(15) << "Optimal Cost"
              << std::setw(15) << "GRASP Cost"
              << std::setw(20) << "Optimized Cost"
              << std::setw(15) << "GAP (GRASP)"
              << std::setw(20) << "GAP (Optimized)"
              << std::setw(15) << "Exec Time (s)"
              << std::endl;

    for (const auto &result : results)
    {
        std::cout << std::setw(10) << result.instanceName
                  << std::setw(15) << result.optimalPenalty
                  << std::setw(15) << result.GRASPPenalty
                  << std::setw(20) << result.optimizedPenalty
                  << std::setw(15) << result.GRASPGAP
                  << std::setw(20) << result.optimizedGAP
                  << std::setw(15) << result.executionTime
                  << std::endl;
    }

    // Save results to the results/summary.txt file
    std::ofstream file("results/summary.txt");
    if (file.is_open())
    {
        file << std::setw(10) << "Instance"
             << std::setw(15) << "Optimal Cost"
             << std::setw(15) << "GRASP Cost"
             << std::setw(20) << "Optimized Cost"
             << std::setw(15) << "GAP (GRASP)"
             << std::setw(20) << "GAP (Optimized)"
             << std::setw(15) << "Exec Time (s)"
             << std::endl;
        for (const auto &result : results)
        {
            file << std::setw(10) << result.instanceName
                 << std::setw(15) << result.optimalPenalty
                 << std::setw(15) << result.GRASPPenalty
                 << std::setw(20) << result.optimizedPenalty
                 << std::setw(15) << result.GRASPGAP
                 << std::setw(20) << result.optimizedGAP
                 << std::setw(15) << result.executionTime
                 << std::endl;
        }
    }
    else
    {
        std::cerr << "Error: Unable to open file for writing: summary.txt" << std::endl;
    }
    file.close();
    return 0;
}
