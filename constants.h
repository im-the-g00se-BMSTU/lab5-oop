#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdlib>

namespace Constants {
// изменяемые
constexpr int studentLiftCount = 8;
constexpr int teacherLiftCount = 1;

constexpr int studentIgnoredFloor = 3;

constexpr int studentStuckChanceDenominator = 10;
constexpr int teacherSecurityChanceDenominator = 15;

constexpr int travelIntervalMs = 1000;
constexpr int doorOpenIntervalMs = 1000;
constexpr int doorCloseIntervalMs = 1000;
constexpr int floorWaitIntervalMs = 1500;
constexpr int teacherSecurityAnimationIntervalMs = 2000;

// неизменяемые
constexpr int floorCount = 5;
constexpr int maxLiftCount = 12;

constexpr int invalidLiftIndex = -1;

constexpr int noDirection = 0;
constexpr int upDirection = 1;
constexpr int downDirection = -1;

inline bool isFloorValid(int floor) {
    return floor >= 1 && floor <= floorCount;
}

inline int distanceBetweenFloors(int firstFloor, int secondFloor) {
    return std::abs(firstFloor - secondFloor);
}

static_assert(studentLiftCount + teacherLiftCount <= maxLiftCount,
              "Total lift count exceeds maximum allowed");

static_assert(!(studentLiftCount + teacherLiftCount == maxLiftCount &&
                (studentLiftCount % 2 != 0 || teacherLiftCount % 2 != 0)),
              "When using maxLiftCount lifts, both studentLiftCount and teacherLiftCount must be even numbers");
}

#endif // CONSTANTS_H
