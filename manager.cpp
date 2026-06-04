#include "manager.h"

Manager::Manager(
    int liftCount,
    const QString& liftType,
    ManagerStrategy* serviceStrategy,
    QObject* parent
)
    : QObject(parent),
    serviceStrategy(serviceStrategy),
    dispatcherCount(liftCount),
    dispatcherTypeName(liftType) {
    createDispatchers();
    connectDispatcherReports();
    connectStrategyReports();
}

Manager::~Manager() {
    delete serviceStrategy;
}

void Manager::createDispatchers() {
    try {
        for (int index = 0; index < dispatcherCount; ++index) {
            Dispatcher* dispatcher = new Dispatcher(this);
            dispatchers.push_back(dispatcher);
        }
    } catch (const std::bad_alloc&) {
        emit eventReported("Не удалось создать диспетчер: " + dispatcherTypeName);
    }
}

void Manager::connectDispatcherReports() {
    int liftIndex = 0;
    for (Dispatcher* dispatcher : dispatchers) {
        connect(dispatcher, &Dispatcher::eventReported, this, [this, liftIndex](const QString& message) {
            QString indexedMessage = dispatcherTypeName + " " + QString::number(liftIndex + 1) + ": " + message;
            emit eventReported(indexedMessage);
            emit liftEventReported(liftIndex, message);
        });
        ++liftIndex;
    }
}

void Manager::connectStrategyReports() {
    if (serviceStrategy) {
        connect(serviceStrategy, &ManagerStrategy::eventReported,
                this, &Manager::eventReported);
        connect(serviceStrategy, &ManagerStrategy::liftAnimationStarted,
                this, &Manager::liftAnimationStarted);
        connect(serviceStrategy, &ManagerStrategy::liftAnimationStopped,
                this, &Manager::liftAnimationStopped);
    }
}

bool Manager::isLiftIndexValid(int liftIndex) const {
    return liftIndex >= 0 && liftIndex < liftCount();
}

int Manager::selectDispatcher(int floor) const {
    int selectedIndex = Constants::invalidLiftIndex;
    if (serviceStrategy)
        selectedIndex = serviceStrategy->selectDispatcher(dispatchers, floor);
    return selectedIndex;
}

int Manager::liftCount() const {
    return static_cast<int>(dispatchers.size());
}

Dispatcher* Manager::dispatcherAt(int liftIndex) const {
    Dispatcher* dispatcher = nullptr;
    if (isLiftIndexValid(liftIndex))
        dispatcher = dispatchers[liftIndex];
    return dispatcher;
}

void Manager::requestAvailableLift(int floor) {
    if (!Constants::isFloorValid(floor))
        emit eventReported("Некорректный этаж вызова с этажа: " + QString::number(floor));
    else {
        int liftIndex = selectDispatcher(floor);
        if (liftIndex == Constants::invalidLiftIndex)
            emit eventReported("Нет доступного лифта для этажа " + QString::number(floor));
        else {
            emit hallRequestAssigned(liftIndex, floor);
            bool isAccepted = serviceStrategy->handleHallRequest(*dispatchers[liftIndex], liftIndex, floor);
            if (!isAccepted)
                emit hallRequestCanceled(floor);
        }
    }
}

void Manager::requestSpecificLift(int liftIndex, int floor) {
    if (!isLiftIndexValid(liftIndex))
        emit eventReported("Некорректный индекс лифта: " + QString::number(liftIndex));
    else if (!Constants::isFloorValid(floor))
        emit eventReported("Некорректный этаж вызова из кабины: " + QString::number(floor));
    else {
        bool isAccepted = serviceStrategy->handleCabinRequest(*dispatchers[liftIndex], liftIndex, floor);
        if (!isAccepted)
            emit cabinRequestCanceled(liftIndex, floor);
    }
}
