#include "dispatcher.h"

// ======== public ========

Dispatcher::Dispatcher(const QString& reportPrefix, QObject* parent)
    : QObject(parent),
    cabin(new Cabin(this)),
    doors(new Door(this)),
    state(DispatcherState::Idle),
    destinationFloor(1),
    reportPrefix(reportPrefix) {
    setupStateNames();
    connectParts();
}

// ======== private ========

void Dispatcher::setupStateNames() {
    stateNames[DispatcherState::Idle] = "IDLE";
    stateNames[DispatcherState::SelectingTarget] = "SELECTING_TARGET";
    stateNames[DispatcherState::Moving] = "MOVING";
    stateNames[DispatcherState::ServingFloor] = "SERVING_FLOOR";
    stateNames[DispatcherState::Stuck] = "STUCK";
}

QString Dispatcher::stateText() const {
    QString text;
    auto stateName = stateNames.find(state);
    if (stateName != stateNames.end())
        text = stateName->second;
    return text;
}

void Dispatcher::reportEvent(const QString& message) const {
    Logger::write(reportPrefix + message);
}

void Dispatcher::reportMessageBox(const QString& message) const {
    MessageBoxReporter::show(reportPrefix + message);
}

void Dispatcher::changeState(DispatcherState nextState) {
    state = nextState;
    emit dispatcherStateChanged(stateText());
    reportEvent("Состояние диспетчера: " + stateText());
}

void Dispatcher::connectParts() {
    connect(cabin, &Cabin::floorReached, this, &Dispatcher::handleFloorReached);
    connect(cabin, &Cabin::movementStopped, this, &Dispatcher::handleMovementStopped);
    connect(cabin, &Cabin::stateChanged, this, &Dispatcher::cabinStateChanged);
    connect(doors, &Door::opened, this, &Dispatcher::handleDoorsOpened);
    connect(doors, &Door::closed, this, &Dispatcher::handleDoorsClosed);
    connect(doors, &Door::stateChanged, this, &Dispatcher::doorStateChanged);
}

// ======== public slots ========

void Dispatcher::addRequest(int floor) {
    if (!isFloorValid(floor))
        reportEvent("Некорректный этаж в заявке: " + QString::number(floor));
    else if (isStuck())
        reportEvent("Лифт застрял и не можен обработать запрос");
    else if (state == DispatcherState::ServingFloor && floor == cabin->currentFloor())
        emit requestServed(floor);
    else {
        storage.add(floor);
        reportEvent("Получен вызов на этаж " + QString::number(floor));
        processNextRequest();
    }
}

// ======== private ========

void Dispatcher::processNextRequest() {
    if (state == DispatcherState::Idle && !storage.isEmpty()) {
        selectDestination();
        if (destinationFloor == cabin->currentFloor())
            serveCurrentFloor();
        else
            startTrip();
    }
}

void Dispatcher::selectDestination() {
    changeState(DispatcherState::SelectingTarget);
    destinationFloor = planner.nextDestination(cabin->currentFloor(), cabin->direction(), storage.requests());
    emit targetFloorChanged(destinationFloor);
    reportEvent("Новый целевой этаж: " + QString::number(destinationFloor));
}

void Dispatcher::startTrip() {
    Direction direction = Direction::None;
    if (destinationFloor > cabin->currentFloor())
        direction = Direction::Up;
    if (destinationFloor < cabin->currentFloor())
        direction = Direction::Down;
    changeState(DispatcherState::Moving);
    reportEvent("Движение началось");
    cabin->prepareForMovement(direction);
    cabin->beginMovement();
}

void Dispatcher::serveCurrentFloor() {
    changeState(DispatcherState::ServingFloor);
    cabin->lockCabin();
    int floor = cabin->currentFloor();
    if (storage.containsFloor(floor))
        emit requestServed(floor);
    doors->openDoors();
}

// ======== private slots ========

void Dispatcher::handleFloorReached(int floor) {
    if (!isStuck()) {
    emit currentFloorChanged(floor);
    reportEvent("Лифт прибыл на этаж " + QString::number(floor));
    if (storage.containsFloor(floor))
        cabin->stopAtCurrentFloor();
    }
}

void Dispatcher::handleMovementStopped(int floor) {
    if (!isStuck()) {
    reportEvent("Кабина остановилась на этаже " + QString::number(floor));
    serveCurrentFloor();
    }
}

void Dispatcher::handleDoorsOpened() {
    if (!isStuck())
    reportEvent("Двери открылись");
}

void Dispatcher::handleDoorsClosed() {
    if (!isStuck()) {
    reportEvent("Двери закрылись");
    int floor = cabin->currentFloor();
    if (storage.containsFloor(floor))
        emit requestCompleted(floor);
    storage.eraseFloor(floor);
    cabin->releaseCabin();
    changeState(DispatcherState::Idle);
    processNextRequest();
    }
}

// ======== public ========

int Dispatcher::currentFloor() const {
    return cabin->currentFloor();
}

Direction Dispatcher::direction() const {
    return cabin->direction();
}

bool Dispatcher::isStuck() const {
    return state == DispatcherState::Stuck;
}

bool Dispatcher::isFloorValid(int floor) {
    return floor >= 1 && floor <= floorCount;
}

bool Dispatcher::canServeHallRequest(int floor) const {
    bool canServe = false;
    if (!isStuck())
        canServe = state == DispatcherState::Idle && storage.isEmpty()
                   || (state == DispatcherState::ServingFloor && floor == cabin->currentFloor());
    if (!isStuck() && !canServe)
        canServe = planner.isAhead(floor, cabin->currentFloor(), cabin->direction());
    return canServe;
}

void Dispatcher::makeStuck() {
    if (!isStuck()) {
        doors->stopDoors();
        cabin->lockCabin();
        destinationFloor = cabin->currentFloor();
        emit targetFloorChanged(destinationFloor);
        changeState(DispatcherState::Stuck);
        reportMessageBox("Лифт застрял");
    }
}
