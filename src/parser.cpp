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
    }

    // Read setup times matrix
    for (int i = 0; i < numOrders; ++i) {
        for (int j = 0; j < numOrders; ++j) {
            file >> setupTimes[i][j];
        }
    }

    file.close();
}
