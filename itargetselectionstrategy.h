#ifndef ITARGETSELECTIONSTRATEGY_H
#define ITARGETSELECTIONSTRATEGY_H

#include <vector>
#include "elevatorrequest.h"

class ITargetSelectionStrategy {
public:
    virtual ~ITargetSelectionStrategy() = default;
    virtual int selectTarget(int currentFloor, int direction, const std::vector<ElevatorRequest> &requests) const = 0;
    virtual bool shouldStop(int currentFloor, const std::vector<ElevatorRequest> &requests) const = 0;
};

#endif // ITARGETSELECTIONSTRATEGY_H
