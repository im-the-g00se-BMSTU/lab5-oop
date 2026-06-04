#include "dispatcher.h"

Dispatcher::Dispatcher(QObject* parent)
    : QObject(parent),
    car(new Car(this)),
    doors(new Door(this)),
    state(DispatcherState::Idle),
    destinationFloor(1) {
    setupStateNames();
    connectParts();
}

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

void Dispatcher::addRequest(int floor) {
    if (!Constants::isFloorValid(floor))
        emit eventReported("Некорректный этаж в заявке: " + QString::number(floor));
    else if (isStuck())
        emit eventReported("Лифт застрял и не можен обработать запрос");
    else if (isServingFloor(floor))
        emit requestServed(floor);
    else {
        storage.add(floor);
        emit eventReported("Получен вызов на этаж " + QString::number(floor));
        processNextRequest();
    }
}

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
    changeState(DispatcherState::Moving);
    emit eventReported("Движение началось");
    car->prepareForMovement(directionToDestination());
    car->beginMovement();
}

void Dispatcher::serveCurrentFloor() {
    changeState(DispatcherState::ServingFloor);
    car->lockCabin();
    reportStartedService();
    doors->openDoors();
}

void Dispatcher::reportStartedService() {
    int floor = car->currentFloor();
    if (storage.containsFloor(floor))
        emit requestServed(floor);
}

int Dispatcher::directionToDestination() const {
    int direction = Constants::noDirection;
    if (destinationFloor > car->currentFloor())
        direction = Constants::upDirection;
    if (destinationFloor < car->currentFloor())
        direction = Constants::downDirection;
    return direction;
}

void Dispatcher::handleFloorReached(int floor) {
    emit currentFloorChanged(floor);
    emit eventReported("Лифт прибыл на этаж " + QString::number(floor));
    if (planner.shouldServeFloor(floor, storage.requests()))
        car->stopAtCurrentFloor();
}

void Dispatcher::handleMovementStopped(int floor) {
    emit eventReported("Кабина остановилась на этаже " + QString::number(floor));
    serveCurrentFloor();
}

void Dispatcher::handleDoorsOpened() {
    emit eventReported("Двери открылись");
}

void Dispatcher::handleDoorsClosed() {
    emit eventReported("Двери закрылись");
    int floor = car->currentFloor();
    if (storage.containsFloor(floor))
        emit requestCompleted(floor);
    storage.eraseFloor(floor);
    car->releaseCabin();
    changeState(DispatcherState::Idle);
    processNextRequest();
}

int Dispatcher::currentFloor() const {
    return car->currentFloor();
}

int Dispatcher::direction() const {
    return car->direction();
}

bool Dispatcher::isFree() const {
    return !isStuck() && state == DispatcherState::Idle && storage.isEmpty();
}

bool Dispatcher::isStuck() const {
    return state == DispatcherState::Stuck;
}

bool Dispatcher::canServeHallRequest(int floor) const {
    bool canServe = false;
    if (!isStuck())
        canServe = isFree() || isServingFloor(floor);
    if (!isStuck() && !canServe)
        canServe = planner.isAhead(floor, car->currentFloor(), car->direction());
    return canServe;
}

bool Dispatcher::isServingFloor(int floor) const {
    return state == DispatcherState::ServingFloor && floor == car->currentFloor();
}

void Dispatcher::makeStuck() {
    if (!isStuck()) {
        car->lockCabin();
        changeState(DispatcherState::Stuck);
        emit eventReported("Lift is stuck");
    }
}
