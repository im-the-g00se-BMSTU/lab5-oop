#ifndef LIFT_REQUEST_H
#define LIFT_REQUEST_H

enum class LiftRequestOrigin {
    Hall,
    Cabin
};

class LiftRequest {
private:
    int floorNumber;
    LiftRequestOrigin origin;

public:
    LiftRequest(int floor, LiftRequestOrigin requestOrigin)
        : floorNumber(floor),
        origin(requestOrigin) {}

    int floor() const {
        return floorNumber;
    }

    LiftRequestOrigin requestOrigin() const {
        return origin;
    }

    bool matchesFloor(int floor) const {
        return floorNumber == floor;
    }
};

#endif // LIFT_REQUEST_H
