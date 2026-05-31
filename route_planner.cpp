#include "route_planner.h"

bool RoutePlanner::isAhead(int floor, int currentFloor, int direction) const {
    return direction == LiftConstants::noDirection && floor != currentFloor
           || (direction == LiftConstants::upDirection && floor > currentFloor)
           || (direction == LiftConstants::downDirection && floor < currentFloor);
}

bool RoutePlanner::isFurtherInDirection(int candidateFloor, int selectedFloor, int direction) const {
    return (direction == LiftConstants::upDirection && candidateFloor > selectedFloor)
        || (direction == LiftConstants::downDirection && candidateFloor < selectedFloor);
}

int RoutePlanner::chooseAlongDirection(int currentFloor, int direction, const std::vector<LiftRequest>& requests) const {
    int destination = currentFloor;
    for (const LiftRequest& request : requests)
        if (isAhead(request.floor(), currentFloor, direction) && isFurtherInDirection(request.floor(), destination, direction))
            destination = request.floor();
    return destination;
}

int RoutePlanner::chooseNearest(int currentFloor, const std::vector<LiftRequest>& requests) const {
    int destination = currentFloor;
    int bestDistance = LiftConstants::floorCount;
    for (const LiftRequest& request : requests) {
        int distance = std::abs(request.floor() - currentFloor);
        if (distance < bestDistance) {
            bestDistance = distance;
            destination = request.floor();
        }
    }
    return destination;
}

int RoutePlanner::nextDestination(int currentFloor, int direction, const std::vector<LiftRequest>& requests) const {
    int destination = chooseAlongDirection(currentFloor, direction, requests);
    if (destination == currentFloor)
        destination = chooseNearest(currentFloor, requests);
    return destination;
}

bool RoutePlanner::shouldServeFloor(int floor, const std::vector<LiftRequest>& requests) const {
    bool shouldServe = false;
    for (const LiftRequest& request : requests)
        shouldServe = shouldServe || request.matchesFloor(floor);
    return shouldServe;
}
