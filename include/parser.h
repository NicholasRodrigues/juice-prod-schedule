#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "algorithm.h"

void parseInputFile(const std::string& filename, std::vector<Order>& orders, std::vector<std::vector<int>>& setupTimes);

#endif
