#ifndef DIRECTIONALTARGETSELECTION_H
#define DIRECTIONALTARGETSELECTION_H

#include "itargetselectionstrategy.h"
#include <cmath>

class DirectionalTargetSelection : public ITargetSelectionStrategy {
private:
    bool isInDirection(int floor, int currentFloor, int direction) const {
        return (!direction && floor != currentFloor) || (direction > 0  && floor > currentFloor)  || (direction < 0  && floor < currentFloor);
    }

    bool isBetterDirectedTarget(int floor, int selectedFloor, int currentFloor, int direction) const {
        return selectedFloor == currentFloor || (direction > 0 && floor > selectedFloor) || (direction < 0 && floor < selectedFloor);
    }

    int selectDirectedTarget(int currentFloor, int direction, const std::vector<ElevatorRequest>& requests) const {
        int selectedTarget = currentFloor;
        for (const ElevatorRequest& request : requests) {
            int floor = request.getFloor();
            if (isInDirection(floor, currentFloor, direction) && isBetterDirectedTarget(floor, selectedTarget, currentFloor, direction)) {
                selectedTarget = floor;
            }
        }
        return selectedTarget;
    }

    int selectNearestTarget(int currentFloor, const std::vector<ElevatorRequest>& requests) const {
        int selectedTarget = currentFloor;
        int startIndex = 0;
        if (!requests.empty()) {
            selectedTarget = requests[0].getFloor();
            startIndex = 1;
        }
        for (size_t i = startIndex; i < requests.size(); ++i) {
            int floor = requests[i].getFloor(), currentDistance = std::abs(floor - currentFloor);
            int selectedDistance = std::abs(selectedTarget - currentFloor);
            if (currentDistance < selectedDistance) {
                selectedTarget = floor;
            }
        }
        return selectedTarget;
    }

public:
    int selectTarget(int currentFloor, int direction, const std::vector<ElevatorRequest>& requests) const override {
        int target = (direction) ? selectDirectedTarget(currentFloor, direction, requests) : currentFloor;
        return (target == currentFloor) ? selectNearestTarget(currentFloor, requests) : target;
    }

    bool shouldStop(int currentFloor, const std::vector<ElevatorRequest>& requests) const override {
        bool result = false;
        for (size_t i = 0; i < requests.size() && !result; ++i) {
            result = requests[i].isForFloor(currentFloor);
        }
        return result;
    }
};

#endif // DIRECTIONALTARGETSELECTION_H
