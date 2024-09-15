#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include "order.h"

void parseInputFile(const std::string& filename, std::vector<Order>& orders,
                    std::vector<std::vector<int>>& setupTimes);

#endif // PARSER_H
