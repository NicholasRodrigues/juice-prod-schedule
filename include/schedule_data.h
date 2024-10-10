// schedule_data.h
#ifndef SCHEDULE_DATA_H
#define SCHEDULE_DATA_H

#include <vector>

struct ScheduleData {
    std::vector<int> schedule;
    std::vector<int> arrivalTimes;
    std::vector<double> penalties;
    double totalPenalty;
};

#endif // SCHEDULE_DATA_H
