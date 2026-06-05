#include "manager_strategy.h"

// ======== private ========

int StudentLiftStrategy::selectAvailableDispatcher(int floor) const {
    int selectedIndex = dispatcherPool.selectNearestForHall(floor);
    Dispatcher* dispatcher = dispatcherPool.at(selectedIndex);
    if (dispatcher && makeDispatcherStuckByChance(*dispatcher))
        selectedIndex = selectAvailableDispatcher(floor);
    return selectedIndex;
}

bool StudentLiftStrategy::makeDispatcherStuckByChance(Dispatcher& dispatcher) const {
    bool isStuck = randomChanceGenerator(stuckChanceDenominator);
    if (isStuck)
        dispatcher.makeStuck();
    return isStuck;
}

void StudentLiftStrategy::reportIgnoredFloor(int floor) {
    MessageBoxReporter::show(QString::fromUtf8("Лифты не останавливаются на этаже ")
                       + QString::number(floor)
                       + QString::fromUtf8(". Приносим временные извинения за искренние неудобства."));
}

// ======== public ========

StudentLiftStrategy::StudentLiftStrategy(DispatcherPool& dispatcherPool)
    : ManagerStrategy(dispatcherPool) {}

int StudentLiftStrategy::selectDispatcher(int floor) const {
    int selectedIndex = DispatcherPool::invalidIndex;
    if (floor == ignoredFloor && !dispatcherPool.isEmpty())
        selectedIndex = 0;
    else
        selectedIndex = selectAvailableDispatcher(floor);
    return selectedIndex;
}

bool StudentLiftStrategy::handleHallRequest(Dispatcher& dispatcher, int, int floor) {
    bool isAccepted = floor != ignoredFloor;
    if (isAccepted)
        dispatcher.addRequest(floor);
    else
        reportIgnoredFloor(floor);
    return isAccepted;
}

bool StudentLiftStrategy::handleCabinRequest(Dispatcher& dispatcher, int, int floor) {
    bool isAccepted = floor != ignoredFloor && !dispatcher.isStuck();
    if (isAccepted && makeDispatcherStuckByChance(dispatcher))
        isAccepted = false;
    if (isAccepted)
        dispatcher.addRequest(floor);
    else if (floor == ignoredFloor)
        reportIgnoredFloor(floor);
    return isAccepted;
}

// ======== public ========

TeacherLiftStrategy::TeacherLiftStrategy(DispatcherPool& dispatcherPool)
    : ManagerStrategy(dispatcherPool) {}

int TeacherLiftStrategy::selectDispatcher(int floor) const {
    return dispatcherPool.selectNearestNotStuck(floor);
}

bool TeacherLiftStrategy::handleHallRequest(Dispatcher& dispatcher, int liftIndex, int floor) {
    bool shouldCancelRequest = randomChanceGenerator(securityChanceDenominator);
    dispatcher.addRequest(floor);
    if (shouldCancelRequest) {
        MessageBoxReporter::show(QString::fromUtf8("В преподавательском лифте был обнаружен студент"));
        emit liftAnimationStarted(liftIndex, floor, ":/security.gif");
        QTimer::singleShot(securityAnimationIntervalMs, &dispatcher, [this, liftIndex, floor]() {
            emit liftAnimationStopped(liftIndex, floor);
        });
    }
    return true;
}

bool TeacherLiftStrategy::handleCabinRequest(Dispatcher& dispatcher, int, int floor) {
    dispatcher.addRequest(floor);
    return true;
}
