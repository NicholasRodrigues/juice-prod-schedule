#include <iostream>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <fstream>
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

    std::vector<Result> results;

    for (const auto &entry : fs::directory_iterator(filepath))
    {

        std::string filepath = entry.path().string();
        std::string filename = entry.path().filename().string();

        // Extract instance name (e.g., "n60A" from "n60A.txt")
        std::string instanceName = filename.substr(0, filename.find('.'));

        std::cout << "Processing file: " << filepath << std::endl;

        std::cout << "Optimal penalty: " << optimalPenalties[instanceName] << std::endl;

        parseInputFile(filepath, orders, setupTimes, initialSetupTimes);

        double totalPenaltyCost = 0.0;
        auto start = std::chrono::high_resolution_clock::now();

        // Generate initial greedy schedule
        std::vector<int> initialSchedule = GRASP(orders, setupTimes, initialSetupTimes, totalPenaltyCost);

        // Output in a parsable format for the shell script
        std::cout << "INITIAL_SCHEDULE: ";
        for (int i : initialSchedule)
        {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "TOTAL_PENALTY_GREEDY: " << totalPenaltyCost << std::endl;

        // Calculate GAP for greedy solution
        double greedyGAP = 0.0;
        if (optimalPenalties.find(instanceName) != optimalPenalties.end())
        {
            double optimalPenalty = optimalPenalties[instanceName];
            greedyGAP = ((totalPenaltyCost - optimalPenalty) / optimalPenalty) * 100.0;
            std::cout << "GAP_GREEDY: " << greedyGAP << "%" << std::endl;
        }

        // Perform ILS optimization with Adaptive RVND
        std::vector<int> optimizedSchedule = ILS(initialSchedule, orders, setupTimes, initialSetupTimes);

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
        std::cout << "EXECUTION_TIME: " << elapsed.count() << " seconds" << std::endl;

        results.push_back({instanceName, optimalPenalties[instanceName], totalPenaltyCost, totalPenaltyAfterOptimization, greedyGAP, optimizedGAP, elapsed.count()});
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
