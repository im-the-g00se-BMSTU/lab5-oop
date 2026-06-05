#include "route_planner.h"

// ======== public ========

bool RoutePlanner::isAhead(int floor, int currentFloor, Direction direction) const {
    return direction == Direction::None && floor != currentFloor
           || (direction == Direction::Up && floor > currentFloor)
           || (direction == Direction::Down && floor < currentFloor);
}

// ======== private ========

int RoutePlanner::chooseAlongDirection(int currentFloor, Direction direction, const std::vector<int>& requests) const {
    int destination = currentFloor;
    for (int requestFloor : requests)
        if (isAhead(requestFloor, currentFloor, direction)
            && (direction == Direction::Up && requestFloor > destination
                || direction == Direction::Down && requestFloor < destination))
            destination = requestFloor;
    return destination;
}

int RoutePlanner::chooseNearest(int currentFloor, const std::vector<int>& requests) const {
    int destination = currentFloor;
    int bestDistance = std::numeric_limits<int>::max();
    for (int requestFloor : requests) {
        int distance = std::abs(requestFloor - currentFloor);
        if (distance < bestDistance) {
            bestDistance = distance;
            destination = requestFloor;
        }
    }
    return destination;
}

// ======== public ========

int RoutePlanner::nextDestination(int currentFloor, Direction direction, const std::vector<int>& requests) const {
    int destination = chooseAlongDirection(currentFloor, direction, requests);
    if (destination == currentFloor)
        destination = chooseNearest(currentFloor, requests);
    return destination;
}
