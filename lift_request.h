#ifndef LIFT_REQUEST_H
#define LIFT_REQUEST_H

class LiftRequest {
private:
    int floorNumber;

public:
    explicit LiftRequest(int floor)
        : floorNumber(floor) {}

    int floor() const {
        return floorNumber;
    }

    bool matchesFloor(int floor) const {
        return floorNumber == floor;
    }
};

#endif // LIFT_REQUEST_H
