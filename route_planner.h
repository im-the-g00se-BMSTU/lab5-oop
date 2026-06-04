#ifndef ROUTE_PLANNER_H
#define ROUTE_PLANNER_H

#include "constants.h"

#include <vector>

class RoutePlanner {
private:
    int chooseAlongDirection(int currentFloor, int direction, const std::vector<int>& requests) const;
    int chooseNearest(int currentFloor, const std::vector<int>& requests) const;

public:
    bool isAhead(int floor, int currentFloor, int direction) const;
    int nextDestination(int currentFloor, int direction, const std::vector<int>& requests) const;
};

#endif // ROUTE_PLANNER_H
