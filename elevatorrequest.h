#ifndef ELEVATORREQUEST_H
#define ELEVATORREQUEST_H

enum class RequestSource{
    FLOOR,
    CABIN
};

class ElevatorRequest {
private:
    int requestFloor;
    RequestSource requestSource;
public:
    ElevatorRequest(int floor, RequestSource source) : requestFloor(floor), requestSource(source) {}
    int getFloor() const {
        return requestFloor;
    }
    RequestSource getSource() const {
        return requestSource;
    }
    bool isForFloor(int floor) const {
        return requestFloor == floor;
    }
};


#endif // ELEVATORREQUEST_H
