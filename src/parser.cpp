#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "parser.h"
#include "order.h"

void parseInputFile(const std::string& filename, std::vector<Order>& orders,
                    std::vector<std::vector<int>>& setupTimes,
                    std::vector<int>& initialSetupTimes) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    int numOrders;
    file >> numOrders;

    // Skip the empty line
    std::string line;
    std::getline(file, line); // To move to the next line after numOrders
    std::getline(file, line); // Empty line

    orders.resize(numOrders);

    // Read processing times (array t)
    for (int i = 0; i < numOrders; ++i) {
        file >> orders[i].processingTime;
        orders[i].id = i;
    }

    // Read due times (array p)
    for (int i = 0; i < numOrders; ++i) {
        file >> orders[i].dueTime;
    }

    // Read penalty rates (array w)
    for (int i = 0; i < numOrders; ++i) {
        file >> orders[i].penaltyRate;
        if (orders[i].penaltyRate < 0) {
            std::cerr << "Error: Negative penalty rate for order " << i << std::endl;
            orders[i].penaltyRate = 0.0;
        }
    }

    // Skip the empty line before the setup times matrix
    std::getline(file, line);
    std::getline(file, line); // Empty line

    // Read initial setup times (s0j)
    initialSetupTimes.resize(numOrders);
    for (int j = 0; j < numOrders; ++j) {
        if (!(file >> initialSetupTimes[j])) {
            std::cerr << "Error reading initial setup time for order " << j << std::endl;
            initialSetupTimes[j] = 0;
        }
    }

    // Read setup times matrix (sij)
    setupTimes.resize(numOrders, std::vector<int>(numOrders));

    for (int i = 0; i < numOrders; ++i) {
        for (int j = 0; j < numOrders; ++j) {
            if (!(file >> setupTimes[i][j])) {
                std::cerr << "Error reading setup time between orders " << i << " and " << j << std::endl;
                setupTimes[i][j] = 0;
            }
            if (setupTimes[i][j] < 0) {
                std::cerr << "Error: Negative setup time between orders " << i << " and " << j << std::endl;
                setupTimes[i][j] = 0;
            }
        }
    }

    file.close();
}
