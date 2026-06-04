#include "manager.h"

// ======== public ========

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

// ======== private ========

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
        connect(dispatcher, &Dispatcher::messageBoxRequested, this, [this, liftIndex](const QString& message) {
            emit messageBoxRequested(dispatcherTypeName + " " + QString::number(liftIndex + 1) + ": " + message);
        });
        ++liftIndex;
    }
}

void Manager::connectStrategyReports() {
    if (serviceStrategy) {
        connect(serviceStrategy, &ManagerStrategy::eventReported,
                this, &Manager::eventReported);
        connect(serviceStrategy, &ManagerStrategy::messageBoxRequested,
                this, &Manager::messageBoxRequested);
        connect(serviceStrategy, &ManagerStrategy::liftAnimationStarted,
                this, &Manager::liftAnimationStarted);
        connect(serviceStrategy, &ManagerStrategy::liftAnimationStopped,
                this, &Manager::liftAnimationStopped);
    }
}

bool Manager::isLiftIndexValid(int liftIndex) const {
    return liftIndex >= 0 && liftIndex < liftCount();
}

// ======== public ========

int Manager::liftCount() const {
    return static_cast<int>(dispatchers.size());
}

Dispatcher* Manager::dispatcherAt(int liftIndex) const {
    Dispatcher* dispatcher = nullptr;
    if (isLiftIndexValid(liftIndex))
        dispatcher = dispatchers[liftIndex];
    return dispatcher;
}

// ======== public slots ========

void Manager::requestAvailableLift(int floor) {
    if (!Constants::isFloorValid(floor))
        emit eventReported("Некорректный этаж вызова с этажа: " + QString::number(floor));
    else {
        int liftIndex = Constants::invalidLiftIndex;
        if (serviceStrategy)
            liftIndex = serviceStrategy->selectDispatcher(dispatchers, floor);
        if (liftIndex == Constants::invalidLiftIndex)
            emit messageBoxRequested("Нет доступного лифта для этажа " + QString::number(floor));
        else {
            emit hallRequestAssigned(liftIndex, floor);
            if (!serviceStrategy->handleHallRequest(*dispatchers[liftIndex], liftIndex, floor))
                emit hallRequestCanceled(floor);
        }
    }
}

void Manager::requestSpecificLift(int liftIndex, int floor) {
    if (!isLiftIndexValid(liftIndex))
        emit eventReported("Некорректный индекс лифта: " + QString::number(liftIndex));
    else if (!Constants::isFloorValid(floor))
        emit eventReported("Некорректный этаж вызова из кабины: " + QString::number(floor));
    else if (dispatchers[liftIndex]->isStuck())
        emit eventReported("Лифт застрял и не может принять запрос из кабины");
    else if (!serviceStrategy->handleCabinRequest(*dispatchers[liftIndex], liftIndex, floor))
        emit cabinRequestCanceled(liftIndex, floor);
}
