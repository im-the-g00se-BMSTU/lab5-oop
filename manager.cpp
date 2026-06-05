#include "manager.h"

// ======== public ========

Manager::Manager(
    int liftCount,
    const QString& liftType,
    ManagerStrategy* serviceStrategy,
    QObject* parent
)
    : QObject(parent),
    dispatcherPool(serviceStrategy->pool()),
    serviceStrategy(serviceStrategy),
    dispatcherCount(liftCount),
    dispatcherTypeName(liftType) {
    createDispatchers();
    connectStrategyReports();
}

Manager::~Manager() {
    delete serviceStrategy;
}

// ======== private ========

void Manager::createDispatchers() {
    try {
        for (int index = 0; index < dispatcherCount; ++index) {
            QString prefix = dispatcherTypeName + " " + QString::number(index + 1) + ": ";
            Dispatcher* dispatcher = new Dispatcher(prefix, this);
            dispatcherPool.add(dispatcher);
        }
    } catch (const std::bad_alloc&) {
        Logger::write("Не удалось создать диспетчер: " + dispatcherTypeName);
    }
}

void Manager::connectStrategyReports() {
    if (serviceStrategy) {
        connect(serviceStrategy, &ManagerStrategy::liftAnimationStarted,
                this, &Manager::liftAnimationStarted);
        connect(serviceStrategy, &ManagerStrategy::liftAnimationStopped,
                this, &Manager::liftAnimationStopped);
    }
}

bool Manager::isLiftIndexValid(int liftIndex) const {
    return dispatcherPool.isIndexValid(liftIndex);
}

// ======== public ========

int Manager::liftCount() const {
    return dispatcherPool.count();
}

Dispatcher* Manager::dispatcherAt(int liftIndex) const {
    return dispatcherPool.at(liftIndex);
}

// ======== public slots ========

void Manager::requestAvailableLift(int floor) {
    if (!Dispatcher::isFloorValid(floor))
        Logger::write("Некорректный этаж вызова с этажа: " + QString::number(floor));
    else {
        int liftIndex = DispatcherPool::invalidIndex;
        if (serviceStrategy)
            liftIndex = serviceStrategy->selectDispatcher(floor);
        if (liftIndex == DispatcherPool::invalidIndex)
            MessageBoxReporter::show("Нет доступного лифта для этажа " + QString::number(floor));
        else {
            emit hallRequestAssigned(liftIndex, floor);
            if (!serviceStrategy->handleHallRequest(*dispatcherPool.at(liftIndex), liftIndex, floor))
                emit hallRequestCanceled(floor);
        }
    }
}

void Manager::requestSpecificLift(int liftIndex, int floor) {
    if (!isLiftIndexValid(liftIndex))
        Logger::write("Некорректный индекс лифта: " + QString::number(liftIndex));
    else if (!Dispatcher::isFloorValid(floor))
        Logger::write("Некорректный этаж вызова из кабины: " + QString::number(floor));
    else if (dispatcherPool.at(liftIndex)->isStuck())
        Logger::write("Лифт застрял и не может принять запрос из кабины");
    else if (!serviceStrategy->handleCabinRequest(*dispatcherPool.at(liftIndex), liftIndex, floor))
        emit cabinRequestCanceled(liftIndex, floor);
}
