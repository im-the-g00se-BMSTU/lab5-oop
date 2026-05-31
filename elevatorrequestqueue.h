    #ifndef ELEVATORREQUESTQUEUE_H
#define ELEVATORREQUESTQUEUE_H

#include "elevatorrequest.h"

#include <vector>

class ElevatorRequestQueue {
private:
    std::vector<ElevatorRequest> requests;

public:
    bool hasRequests() const {
        return !requests.empty();
    }

    bool hasRequestForFloor(int floor) const {
        bool result = false;
        for (size_t i = 0; i < requests.size() && !result; ++i)
            result = requests[i].isForFloor(floor); //
        return result;
    }

    void addRequest(int floor, RequestSource source) {
        if (!hasRequestForFloor(floor))
            requests.push_back(ElevatorRequest(floor, source));
    }

    void removeRequestsForFloor(int floor) {
        std::vector<ElevatorRequest> filteredRequests;
        for (const ElevatorRequest& request : requests) {
            if (!request.isForFloor(floor))
                filteredRequests.push_back(request);
        }
        requests = filteredRequests;
    }

    const std::vector<ElevatorRequest>& getRequests() const {
        return requests;
    }
};


#endif // ELEVATORREQUESTQUEUE_H
