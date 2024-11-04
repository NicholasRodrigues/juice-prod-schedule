#include <iostream>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include "algorithm.h"
#include "parser.h"
#include "schedule_data.h"
#include <random>
#include <chrono>
#include <string>

namespace fs = std::filesystem;

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

int main(int argc, char *argv[])
{
    srand(time(NULL));

    if (argc < 2 || argc > 3)
    {
        std::cerr << "Usage: " << argv[0] << " <instance_file_path> [seed]" << std::endl;
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

    std::string filename = fs::path(filepath).filename().string();
    std::string instanceName = filename.substr(0, filename.find('.'));

    std::cout << "Processing file: " << filepath << std::endl;

    parseInputFile(filepath, orders, setupTimes, initialSetupTimes);

    double optimalPenalty = optimalPenalties[instanceName];
    if (optimalPenalty <= 0)
    {
        std::cerr << "Optimal penalty not defined or invalid for instance: " << instanceName << std::endl;
        // Output N/A for all metrics
        std::cout << "CONSTRUCTION_PENALTY: N/A" << std::endl;
        std::cout << "CONSTRUCTION_TIME: N/A seconds" << std::endl;
        std::cout << "CONSTRUCTION_GAP: N/A%" << std::endl;
        std::cout << "CONSTRUCTION_SCHEDULE: N/A" << std::endl;

        std::cout << "RVND_PENALTY: N/A" << std::endl;
        std::cout << "RVND_TIME: N/A seconds" << std::endl;
        std::cout << "RVND_GAP: N/A%" << std::endl;
        std::cout << "RVND_SCHEDULE: N/A" << std::endl;

        std::cout << "ILS_GRASP_PENALTY: N/A" << std::endl;
        std::cout << "ILS_GRASP_TIME: N/A seconds" << std::endl;
        std::cout << "ILS_GRASP_GAP: N/A%" << std::endl;
        std::cout << "ILS_GRASP_SCHEDULE: N/A" << std::endl;

        std::cout << "OPTIMAL_PENALTY: " << optimalPenalty << std::endl;
        std::cout << "SEED_USED: N/A" << std::endl;

        return 1;
    }

    // Initialize random number generator
    unsigned int seed;
    if (argc == 3)
    {
        seed = std::stoul(argv[2]);
    }
    else
    {
        std::random_device rd;
        seed = rd();
    }
    std::mt19937 rng(seed);

    // Variables to store metrics
    double constructionPenalty = 0.0;
    double rvndPenalty = 0.0;
    double ils_graspPenalty = 0.0;

    double constructionTime = 0.0;
    double rvndTime = 0.0;
    double ils_graspTime = 0.0;

    double constructionGap = 0.0;
    double rvndGap = 0.0;
    double ils_graspGap = 0.0;

    std::string best_schedule_construction = "N/A";
    std::string best_schedule_rvnd = "N/A";
    std::string best_schedule_ils_grasp = "N/A";

    ScheduleData constructionData;  // Declare this outside the try block so it's available globally

    // ----------------------------
    // 1. Construction Heuristic
    // ----------------------------
    bool construction_success = false;
    auto start_construction = std::chrono::high_resolution_clock::now();
    try
    {
        std::vector<int> constructionSchedule = greedyConstruction(orders, setupTimes, initialSetupTimes, 0, nullptr);
        auto end_construction = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_construction = end_construction - start_construction;
        constructionTime = elapsed_construction.count();

        constructionData.schedule = constructionSchedule;  // Populate constructionData
        calculateTotalPenalty(constructionData, orders, setupTimes, initialSetupTimes);
        constructionPenalty = constructionData.totalPenalty;
        constructionGap = ((constructionPenalty - optimalPenalty) / optimalPenalty) * 100;

        // Convert schedule to 1-based indexing for output
        best_schedule_construction = "";
        for (size_t j = 0; j < constructionSchedule.size(); ++j)
        {
            best_schedule_construction += std::to_string(constructionSchedule[j] + 1);
            if (j != constructionSchedule.size() - 1)
                best_schedule_construction += ",";
        }

        construction_success = true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "CONSTRUCTION_ERROR: " << e.what() << std::endl;
    }

    // ----------------------------
    // Output Construction Metrics
    // ----------------------------
    std::cout << "CONSTRUCTION_PENALTY: " << (construction_success ? std::to_string(constructionPenalty) : "N/A") << std::endl;
    std::cout << "CONSTRUCTION_TIME: " << (construction_success ? std::to_string(constructionTime) : "N/A") << " seconds" << std::endl;
    std::cout << "CONSTRUCTION_GAP: " << (construction_success ? std::to_string(constructionGap) : "N/A") << "%" << std::endl;
    std::cout << "CONSTRUCTION_SCHEDULE: " << (construction_success ? best_schedule_construction : "N/A") << std::endl;

    // ----------------------------
    // 2. RVND Heuristic
    // ----------------------------
    bool rvnd_success = false;
    auto start_rvnd = std::chrono::high_resolution_clock::now();
    try
    {
        RVND(constructionData, orders, setupTimes, initialSetupTimes, rng);  // Use constructionData here
        auto end_rvnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_rvnd = end_rvnd - start_rvnd;
        rvndTime = elapsed_rvnd.count();

        rvndPenalty = constructionData.totalPenalty;
        rvndGap = ((rvndPenalty - optimalPenalty) / optimalPenalty) * 100;

        // Convert schedule to 1-based indexing for output
        best_schedule_rvnd = "";
        for (size_t j = 0; j < constructionData.schedule.size(); ++j)
        {
            best_schedule_rvnd += std::to_string(constructionData.schedule[j] + 1);
            if (j != constructionData.schedule.size() - 1)
                best_schedule_rvnd += ",";
        }

        rvnd_success = true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "RVND_ERROR: " << e.what() << std::endl;
    }

    // ----------------------------
    // Output RVND Metrics
    // ----------------------------
    std::cout << "RVND_PENALTY: " << (rvnd_success ? std::to_string(rvndPenalty) : "N/A") << std::endl;
    std::cout << "RVND_TIME: " << (rvnd_success ? std::to_string(rvndTime) : "N/A") << " seconds" << std::endl;
    std::cout << "RVND_GAP: " << (rvnd_success ? std::to_string(rvndGap) : "N/A") << "%" << std::endl;
    std::cout << "RVND_SCHEDULE: " << (rvnd_success ? best_schedule_rvnd : "N/A") << std::endl;

    // ----------------------------
    // 3. ILS+GRASP Heuristic
    // ----------------------------
    bool ils_grasp_success = false;
    auto start_ils_grasp = std::chrono::high_resolution_clock::now();
    try
    {
        std::vector<int> ils_graspSchedule = GRASP(orders, setupTimes, initialSetupTimes, ils_graspPenalty, rng);
        auto end_ils_grasp = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_ils_grasp = end_ils_grasp - start_ils_grasp;
        ils_graspTime = elapsed_ils_grasp.count();

        ScheduleData ils_graspData;
        ils_graspData.schedule = ils_graspSchedule;
        calculateTotalPenalty(ils_graspData, orders, setupTimes, initialSetupTimes);
        ils_graspPenalty = ils_graspData.totalPenalty;
        ils_graspGap = ((ils_graspPenalty - optimalPenalty) / optimalPenalty) * 100;

        // Convert schedule to 1-based indexing for output
        best_schedule_ils_grasp = "";
        for (size_t j = 0; j < ils_graspSchedule.size(); ++j)
        {
            best_schedule_ils_grasp += std::to_string(ils_graspSchedule[j] + 1);
            if (j != ils_graspSchedule.size() - 1)
                best_schedule_ils_grasp += ",";
        }

        ils_grasp_success = true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "ILS_GRASP_ERROR: " << e.what() << std::endl;
    }

    // ----------------------------
    // Output ILS+GRASP Metrics
    // ----------------------------
    std::cout << "ILS_GRASP_PENALTY: " << (ils_grasp_success ? std::to_string(ils_graspPenalty) : "N/A") << std::endl;
    std::cout << "ILS_GRASP_TIME: " << (ils_grasp_success ? std::to_string(ils_graspTime) : "N/A") << " seconds" << std::endl;
    std::cout << "ILS_GRASP_GAP: " << (ils_grasp_success ? std::to_string(ils_graspGap) : "N/A") << "%" << std::endl;
    std::cout << "ILS_GRASP_SCHEDULE: " << (ils_grasp_success ? best_schedule_ils_grasp : "N/A") << std::endl;

    // Output Optimal Penalty
    std::cout << "OPTIMAL_PENALTY: " << optimalPenalty << std::endl;

    // Print the seed used
    std::cout << "SEED_USED: " << seed << std::endl;

    return 0;
}
