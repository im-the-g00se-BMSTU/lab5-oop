#include "lift_manager.h"

LiftManager::LiftManager(int liftCount, const QString& liftType, QObject* parent)
    : QObject(parent),
    dispatcherCount(liftCount),
    dispatcherTypeName(liftType) {
    createDispatchers();
    connectDispatcherReports();
}

void LiftManager::createDispatchers() {
    try {
        for (int index = 0; index < dispatcherCount; ++index) {
            LiftDispatcher* dispatcher = new LiftDispatcher(this);
            dispatchers.push_back(dispatcher);
        }
    } catch (const std::bad_alloc&) {
        emit eventReported("Не удалось создать диспетчер: " + dispatcherTypeName);
    }
}

void LiftManager::connectDispatcherReports() {
    int liftIndex = 0;
    for (LiftDispatcher* dispatcher : dispatchers) {
        connect(dispatcher, &LiftDispatcher::eventReported, this, [this, liftIndex](const QString& message) {
            QString indexedMessage = dispatcherTypeName + " " + QString::number(liftIndex + 1) + ": " + message;
            emit eventReported(indexedMessage);
            emit liftEventReported(liftIndex, message);
        });
        ++liftIndex;
    }
}

bool LiftManager::isLiftIndexValid(int liftIndex) const {
    return liftIndex >= 0 && liftIndex < liftCount();
}

int LiftManager::selectDispatcher(int floor) const {
    int selectedIndex = LiftConstants::invalidLiftIndex;
    int bestDistance = LiftConstants::floorCount;
    for (int index = 0; index < liftCount(); ++index) {
        int distance = distanceToRequest(dispatchers[index], floor);
        if (dispatchers[index]->canServeHallRequest(floor) && distance < bestDistance) {
            selectedIndex = index;
            bestDistance = distance;
        }
    }
    return selectedIndex;
}

int LiftManager::distanceToRequest(const LiftDispatcher* dispatcher, int floor) const {
    return LiftConstants::distanceBetweenFloors(dispatcher->currentFloor(), floor);
}

int LiftManager::liftCount() const {
    return static_cast<int>(dispatchers.size());
}

LiftDispatcher* LiftManager::dispatcherAt(int liftIndex) const {
    LiftDispatcher* dispatcher = nullptr;
    if (isLiftIndexValid(liftIndex))
        dispatcher = dispatchers[liftIndex];
    return dispatcher;
}

void LiftManager::requestAvailableLift(int floor) {
    if (!LiftConstants::isFloorValid(floor))
        emit eventReported("Некорректный этаж вызова с этажа: " + QString::number(floor));
    else {
        int liftIndex = selectDispatcher(floor);
        if (liftIndex == LiftConstants::invalidLiftIndex)
            emit eventReported("Нет доступного лифта для этажа " + QString::number(floor));
        else {
            emit hallRequestAssigned(liftIndex, floor);
            dispatchers[liftIndex]->addRequest(floor);
        }
    }
}

void LiftManager::requestSpecificLift(int liftIndex, int floor) {
    if (!isLiftIndexValid(liftIndex))
        emit eventReported("Некорректный индекс лифта: " + QString::number(liftIndex));
    else if (!LiftConstants::isFloorValid(floor))
        emit eventReported("Некорректный этаж вызова из кабины: " + QString::number(floor));
    else
        dispatchers[liftIndex]->addRequest(floor);
}
