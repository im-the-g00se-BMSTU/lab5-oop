#ifndef ELEVATORCONFIG_H
#define ELEVATORCONFIG_H

class ElevatorConfig {
public:
    static constexpr int FIRST_FLOOR = 1;
    static constexpr int LAST_FLOOR = 5;
    static constexpr int FLOOR_COUNT = 5;

    static bool isValidFloor(int floor) { //
        return floor >= FIRST_FLOOR && floor <= LAST_FLOOR;
    }
};

#endif // ELEVATORCONFIG_H
