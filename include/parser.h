#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "order.h"

void parseInputFile(const std::string& filename, std::vector<Order>& orders,
                    std::vector<std::vector<int>>& setupTimes,
                    std::vector<int>& initialSetupTimes);

#endif // PARSER_H
