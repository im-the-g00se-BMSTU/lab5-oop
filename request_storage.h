#ifndef REQUEST_STORAGE_H
#define REQUEST_STORAGE_H

#include <vector>

class RequestStorage {
private:
    std::vector<int> pendingFloors;

public:
    bool isEmpty() const {
        return pendingFloors.empty();
    }

    bool containsFloor(int floor) const {
        bool found = false;
        for (int pendingFloor : pendingFloors)
            found = found || pendingFloor == floor;
        return found;
    }

    void add(int floor) {
        if (!containsFloor(floor))
            pendingFloors.push_back(floor);
    }

    void eraseFloor(int floor) {
        std::vector<int> remainingFloors;
        for (int pendingFloor : pendingFloors)
            if (pendingFloor != floor)
                remainingFloors.push_back(pendingFloor);
        pendingFloors = remainingFloors;
    }

    const std::vector<int>& requests() const {
        return pendingFloors;
    }
};

#endif // REQUEST_STORAGE_H
