#ifndef REQUEST_STORAGE_H
#define REQUEST_STORAGE_H

#include "lift_request.h"

#include <vector>

class RequestStorage {
private:
    std::vector<LiftRequest> pendingRequests;

public:
    bool isEmpty() const {
        return pendingRequests.empty();
    }

    bool containsFloor(int floor) const {
        bool found = false;
        for (const LiftRequest& request : pendingRequests)
            found = found || request.matchesFloor(floor);
        return found;
    }

    void add(int floor) {
        if (!containsFloor(floor))
            pendingRequests.emplace_back(floor);
    }

    void eraseFloor(int floor) {
        std::vector<LiftRequest> remainingRequests;
        for (const LiftRequest& request : pendingRequests)
            if (!request.matchesFloor(floor))
                remainingRequests.push_back(request);
        pendingRequests = remainingRequests;
    }

    const std::vector<LiftRequest>& requests() const {
        return pendingRequests;
    }
};

#endif // REQUEST_STORAGE_H
