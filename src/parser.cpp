#include "parser.h"
#include <fstream>
#include <iostream>

void parseInputFile(const std::string& filename, std::vector<Order>& orders,
                    std::vector<std::vector<int>>& setupTimes) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    int numOrders;
    file >> numOrders;

    orders.resize(numOrders);
    setupTimes.resize(numOrders, std::vector<int>(numOrders));

    // Read processing times
    for (int i = 0; i < numOrders; ++i) {
        file >> orders[i].processingTime;
        orders[i].id = i;
    }

    // Read due times
    for (int i = 0; i < numOrders; ++i) {
        file >> orders[i].dueTime;
    }

    // Read penalty rates
    for (int i = 0; i < numOrders; ++i) {
        file >> orders[i].penaltyRate;
        // **Ensure penaltyRate is non-negative**
        if (orders[i].penaltyRate < 0) {
            std::cerr << "Error: Negative penalty rate for order " << i << std::endl;
            orders[i].penaltyRate = 0.0;
        }
    }

    // Read setup times matrix
    for (int i = 0; i < numOrders; ++i) {
        for (int j = 0; j < numOrders; ++j) {
            file >> setupTimes[i][j];
            // **Ensure setup times are non-negative**
            if (setupTimes[i][j] < 0) {
                std::cerr << "Error: Negative setup time between orders " << i << " and " << j << std::endl;
                setupTimes[i][j] = 0;
            }
        }
    }

    file.close();
}