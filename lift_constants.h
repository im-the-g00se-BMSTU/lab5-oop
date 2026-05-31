#ifndef LIFT_CONSTANTS_H
#define LIFT_CONSTANTS_H

namespace LiftConstants {
constexpr int firstFloor = 1;
constexpr int lastFloor = 5;
constexpr int floorCount = 5;

constexpr int noDirection = 0;
constexpr int upDirection = 1;
constexpr int downDirection = -1;

constexpr int travelIntervalMs = 1000;
constexpr int doorOpenIntervalMs = 1000;
constexpr int doorCloseIntervalMs = 1000;
constexpr int floorWaitIntervalMs = 1500;

inline bool isFloorValid(int floor) {
    return floor >= firstFloor && floor <= lastFloor;
}
}

#endif // LIFT_CONSTANTS_H
