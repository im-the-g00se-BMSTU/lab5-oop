#include "dispatcher.h"

// ======== public ========

Dispatcher::Dispatcher(QObject* parent)
    : QObject(parent),
    car(new Car(this)),
    doors(new Door(this)),
    state(DispatcherState::Idle),
    destinationFloor(1) {
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

void Dispatcher::changeState(DispatcherState nextState) {
    state = nextState;
    emit dispatcherStateChanged(stateText());
    emit eventReported("Состояние диспетчера: " + stateText());
}

void Dispatcher::connectParts() {
    connect(car, &Car::floorReached, this, &Dispatcher::handleFloorReached);
    connect(car, &Car::movementStopped, this, &Dispatcher::handleMovementStopped);
    connect(car, &Car::stateChanged, this, &Dispatcher::carStateChanged);
    connect(doors, &Door::opened, this, &Dispatcher::handleDoorsOpened);
    connect(doors, &Door::closed, this, &Dispatcher::handleDoorsClosed);
    connect(doors, &Door::stateChanged, this, &Dispatcher::doorStateChanged);
}

// ======== public slots ========

void Dispatcher::addRequest(int floor) {
    if (!Constants::isFloorValid(floor))
        emit eventReported("Некорректный этаж в заявке: " + QString::number(floor));
    else if (isStuck())
        emit eventReported("Лифт застрял и не можен обработать запрос");
    else if (state == DispatcherState::ServingFloor && floor == car->currentFloor())
        emit requestServed(floor);
    else {
        storage.add(floor);
        emit eventReported("Получен вызов на этаж " + QString::number(floor));
        processNextRequest();
    }
}

// ======== private ========

void Dispatcher::processNextRequest() {
    if (state == DispatcherState::Idle && !storage.isEmpty()) {
        selectDestination();
        if (destinationFloor == car->currentFloor())
            serveCurrentFloor();
        else
            startTrip();
    }
}

void Dispatcher::selectDestination() {
    changeState(DispatcherState::SelectingTarget);
    destinationFloor = planner.nextDestination(car->currentFloor(), car->direction(), storage.requests());
    emit targetFloorChanged(destinationFloor);
    emit eventReported("Новый целевой этаж: " + QString::number(destinationFloor));
}

void Dispatcher::startTrip() {
    int direction = Constants::noDirection;
    if (destinationFloor > car->currentFloor())
        direction = Constants::upDirection;
    if (destinationFloor < car->currentFloor())
        direction = Constants::downDirection;
    changeState(DispatcherState::Moving);
    emit eventReported("Движение началось");
    car->prepareForMovement(direction);
    car->beginMovement();
}

void Dispatcher::serveCurrentFloor() {
    changeState(DispatcherState::ServingFloor);
    car->lockCabin();
    int floor = car->currentFloor();
    if (storage.containsFloor(floor))
        emit requestServed(floor);
    doors->openDoors();
}

// ======== private slots ========

void Dispatcher::handleFloorReached(int floor) {
    if (!isStuck()) {
    emit currentFloorChanged(floor);
    emit eventReported("Лифт прибыл на этаж " + QString::number(floor));
    if (storage.containsFloor(floor))
        car->stopAtCurrentFloor();
    }
}

void Dispatcher::handleMovementStopped(int floor) {
    if (!isStuck()) {
    emit eventReported("Кабина остановилась на этаже " + QString::number(floor));
    serveCurrentFloor();
    }
}

void Dispatcher::handleDoorsOpened() {
    if (!isStuck())
    emit eventReported("Двери открылись");
}

void Dispatcher::handleDoorsClosed() {
    if (!isStuck()) {
    emit eventReported("Двери закрылись");
    int floor = car->currentFloor();
    if (storage.containsFloor(floor))
        emit requestCompleted(floor);
    storage.eraseFloor(floor);
    car->releaseCabin();
    changeState(DispatcherState::Idle);
    processNextRequest();
    }
}

// ======== public ========

int Dispatcher::currentFloor() const {
    return car->currentFloor();
}

int Dispatcher::direction() const {
    return car->direction();
}

bool Dispatcher::isStuck() const {
    return state == DispatcherState::Stuck;
}

bool Dispatcher::canServeHallRequest(int floor) const {
    bool canServe = false;
    if (!isStuck())
        canServe = state == DispatcherState::Idle && storage.isEmpty()
                   || (state == DispatcherState::ServingFloor && floor == car->currentFloor());
    if (!isStuck() && !canServe)
        canServe = planner.isAhead(floor, car->currentFloor(), car->direction());
    return canServe;
}

void Dispatcher::makeStuck() {
    if (!isStuck()) {
        doors->stopDoors();
        car->lockCabin();
        destinationFloor = car->currentFloor();
        emit targetFloorChanged(destinationFloor);
        changeState(DispatcherState::Stuck);
        emit messageBoxRequested("Лифт застрял");
    }
}
