#ifndef ROUTE_PLANNER_H
#define ROUTE_PLANNER_H

#include "direction.h"

#include <cstdlib>
#include <limits>
#include <vector>

class RoutePlanner {
private:
    int chooseAlongDirection(int currentFloor, Direction direction, const std::vector<int>& requests) const;
    int chooseNearest(int currentFloor, const std::vector<int>& requests) const;

public:
    bool isAhead(int floor, int currentFloor, Direction direction) const;
    int nextDestination(int currentFloor, Direction direction, const std::vector<int>& requests) const;
};

#endif // ROUTE_PLANNER_H
