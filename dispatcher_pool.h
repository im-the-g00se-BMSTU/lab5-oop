#ifndef DISPATCHER_POOL_H
#define DISPATCHER_POOL_H

#include "dispatcher.h"

#include <cstdlib>
#include <vector>

class DispatcherPool {
private:
    std::vector<Dispatcher*> dispatchers;

    int distanceToRequest(const Dispatcher* dispatcher, int floor) const {
        int distance = Dispatcher::floorCount;
        if (dispatcher)
            distance = std::abs(dispatcher->currentFloor() - floor);
        return distance;
    }

public:
    static constexpr int invalidIndex = -1;

    void add(Dispatcher* dispatcher) {
        if (dispatcher)
            dispatchers.push_back(dispatcher);
    }

    int count() const {
        return static_cast<int>(dispatchers.size());
    }

    bool isEmpty() const {
        return dispatchers.empty();
    }

    bool isIndexValid(int index) const {
        return index >= 0 && index < count();
    }

    Dispatcher* at(int index) const {
        Dispatcher* dispatcher = nullptr;
        if (isIndexValid(index))
            dispatcher = dispatchers[index];
        return dispatcher;
    }

    int selectNearestForHall(int floor) const {
        int selectedIndex = invalidIndex;
        int bestDistance = Dispatcher::floorCount;
        for (int index = 0; index < count(); ++index) {
            int distance = distanceToRequest(at(index), floor);
            if (at(index)->canServeHallRequest(floor) && distance < bestDistance) {
                selectedIndex = index;
                bestDistance = distance;
            }
        }
        return selectedIndex;
    }

    int selectNearestNotStuck(int floor) const {
        int selectedIndex = invalidIndex;
        int bestDistance = Dispatcher::floorCount;
        for (int index = 0; index < count(); ++index) {
            int distance = distanceToRequest(at(index), floor);
            if (!at(index)->isStuck() && distance < bestDistance) {
                selectedIndex = index;
                bestDistance = distance;
            }
        }
        return selectedIndex;
    }
};

#endif // DISPATCHER_POOL_H
