#include "manager_strategy.h"

// ======== protected ========

int ManagerStrategy::distanceToRequest(const Dispatcher* dispatcher, int floor) const {
    int distance = Constants::floorCount;
    if (dispatcher)
        distance = Constants::distanceBetweenFloors(dispatcher->currentFloor(), floor);
    return distance;
}

bool ManagerStrategy::randomChanceGenerator(int chanceDenominator) const {
    bool isGenerated = false;
    if (chanceDenominator > 0)
        isGenerated = std::rand() % chanceDenominator == 0;
    return isGenerated;
}

int ManagerStrategy::selectNearestDispatcher(const std::vector<Dispatcher*>& dispatchers, int floor) const {
    int selectedIndex = Constants::invalidLiftIndex;
    int bestDistance = Constants::floorCount;
    for (int index = 0; index < static_cast<int>(dispatchers.size()); ++index) {
        int distance = distanceToRequest(dispatchers[index], floor);
        if (dispatchers[index]->canServeHallRequest(floor) && distance < bestDistance) {
            selectedIndex = index;
            bestDistance = distance;
        }
    }
    return selectedIndex;
}

// ======== private ========

int StudentLiftStrategy::selectAvailableDispatcher(const std::vector<Dispatcher*>& dispatchers, int floor) const {
    int selectedIndex = selectNearestDispatcher(dispatchers, floor);
    if (selectedIndex != Constants::invalidLiftIndex && makeDispatcherStuckByChance(*dispatchers[selectedIndex]))
        selectedIndex = selectAvailableDispatcher(dispatchers, floor);
    return selectedIndex;
}

bool StudentLiftStrategy::makeDispatcherStuckByChance(Dispatcher& dispatcher) const {
    bool isStuck = randomChanceGenerator(Constants::studentStuckChanceDenominator);
    if (isStuck)
        dispatcher.makeStuck();
    return isStuck;
}

void StudentLiftStrategy::reportIgnoredFloor(int floor) {
    emit messageBoxRequested(QString::fromUtf8("Лифты не останавливаются на этаже ")
                       + QString::number(floor)
                       + QString::fromUtf8(". Приносим временные извинения за искренние неудобства."));
}

// ======== public ========

int StudentLiftStrategy::selectDispatcher(const std::vector<Dispatcher*>& dispatchers, int floor) const {
    int selectedIndex = Constants::invalidLiftIndex;
    if (floor == Constants::studentIgnoredFloor && !dispatchers.empty())
        selectedIndex = 0;
    else
        selectedIndex = selectAvailableDispatcher(dispatchers, floor);
    return selectedIndex;
}

bool StudentLiftStrategy::handleHallRequest(Dispatcher& dispatcher, int, int floor) {
    bool isAccepted = floor != Constants::studentIgnoredFloor;
    if (isAccepted)
        dispatcher.addRequest(floor);
    else
        reportIgnoredFloor(floor);
    return isAccepted;
}

bool StudentLiftStrategy::handleCabinRequest(Dispatcher& dispatcher, int, int floor) {
    bool isAccepted = floor != Constants::studentIgnoredFloor && !dispatcher.isStuck();
    if (isAccepted && makeDispatcherStuckByChance(dispatcher))
        isAccepted = false;
    if (isAccepted)
        dispatcher.addRequest(floor);
    else if (floor == Constants::studentIgnoredFloor)
        reportIgnoredFloor(floor);
    return isAccepted;
}

// ======== public ========

int TeacherLiftStrategy::selectDispatcher(const std::vector<Dispatcher*>& dispatchers, int floor) const {
    int selectedIndex = Constants::invalidLiftIndex;
    int bestDistance = Constants::floorCount;
    for (int index = 0; index < static_cast<int>(dispatchers.size()); ++index) {
        int distance = distanceToRequest(dispatchers[index], floor);
        if (!dispatchers[index]->isStuck() && distance < bestDistance) {
            selectedIndex = index;
            bestDistance = distance;
        }
    }
    return selectedIndex;
}

bool TeacherLiftStrategy::handleHallRequest(Dispatcher& dispatcher, int liftIndex, int floor) {
    bool isAccepted = true;
    bool shouldCancelRequest = randomChanceGenerator(Constants::teacherSecurityChanceDenominator);
    if (!shouldCancelRequest)
        dispatcher.addRequest(floor);
    else {
        isAccepted = false;
        emit messageBoxRequested(QString::fromUtf8("В преподавательском лифте был обнаружен студент"));
        emit liftAnimationStarted(liftIndex, floor, ":/security.gif");
        QTimer::singleShot(Constants::teacherSecurityAnimationIntervalMs, &dispatcher, [this, liftIndex, floor]() {
            emit liftAnimationStopped(liftIndex, floor);
        });
    }
    return isAccepted;
}

bool TeacherLiftStrategy::handleCabinRequest(Dispatcher& dispatcher, int, int floor) {
    dispatcher.addRequest(floor);
    return true;
}
