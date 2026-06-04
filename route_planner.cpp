#include "route_planner.h"

bool RoutePlanner::isAhead(int floor, int currentFloor, int direction) const {
    return direction == Constants::noDirection && floor != currentFloor
           || (direction == Constants::upDirection && floor > currentFloor)
           || (direction == Constants::downDirection && floor < currentFloor);
}

bool RoutePlanner::isFurtherInDirection(int candidateFloor, int selectedFloor, int direction) const {
    return (direction == Constants::upDirection && candidateFloor > selectedFloor)
        || (direction == Constants::downDirection && candidateFloor < selectedFloor);
}

int RoutePlanner::chooseAlongDirection(int currentFloor, int direction, const std::vector<int>& requests) const {
    int destination = currentFloor;
    for (int requestFloor : requests)
        if (isAhead(requestFloor, currentFloor, direction) && isFurtherInDirection(requestFloor, destination, direction))
            destination = requestFloor;
    return destination;
}

int RoutePlanner::chooseNearest(int currentFloor, const std::vector<int>& requests) const {
    int destination = currentFloor;
    int bestDistance = Constants::floorCount;
    for (int requestFloor : requests) {
        int distance = Constants::distanceBetweenFloors(requestFloor, currentFloor);
        if (distance < bestDistance) {
            bestDistance = distance;
            destination = requestFloor;
        }
    }
    return destination;
}

int RoutePlanner::nextDestination(int currentFloor, int direction, const std::vector<int>& requests) const {
    int destination = chooseAlongDirection(currentFloor, direction, requests);
    if (destination == currentFloor)
        destination = chooseNearest(currentFloor, requests);
    return destination;
}

bool RoutePlanner::shouldServeFloor(int floor, const std::vector<int>& requests) const {
    bool shouldServe = false;
    for (int requestFloor : requests)
        shouldServe = shouldServe || requestFloor == floor;
    return shouldServe;
}
