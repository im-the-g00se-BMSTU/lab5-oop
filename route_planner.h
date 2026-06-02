#ifndef ROUTE_PLANNER_H
#define ROUTE_PLANNER_H

#include "lift_request.h"
#include "lift_constants.h"

#include <vector>

class RoutePlanner {
private:
    bool isAhead(int floor, int currentFloor, int direction) const;
    bool isFurtherInDirection(int candidateFloor, int selectedFloor, int direction) const;
    int chooseAlongDirection(int currentFloor, int direction, const std::vector<LiftRequest>& requests) const;
    int chooseNearest(int currentFloor, const std::vector<LiftRequest>& requests) const;

public:
    bool canServeOnRoute(int currentFloor, int direction, int requestFloor) const;
    int nextDestination(int currentFloor, int direction, const std::vector<LiftRequest>& requests) const;
    bool shouldServeFloor(int floor, const std::vector<LiftRequest>& requests) const;
};

#endif // ROUTE_PLANNER_H
