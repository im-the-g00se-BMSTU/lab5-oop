#include "lift_dispatcher.h"

LiftDispatcher::LiftDispatcher(QObject* parent)
    : QObject(parent),
    car(new LiftCar(this)),
    doors(new DoorMechanism(this)),
    state(DispatcherState::Idle),
    destinationFloor(1) {
    setupStateNames();
    connectParts();
}

void LiftDispatcher::setupStateNames() {
    stateNames[DispatcherState::Idle] = "IDLE";
    stateNames[DispatcherState::SelectingTarget] = "SELECTING_TARGET";
    stateNames[DispatcherState::Moving] = "MOVING";
    stateNames[DispatcherState::ServingFloor] = "SERVING_FLOOR";
}

QString LiftDispatcher::stateText() const {
    QString text;
    auto stateName = stateNames.find(state);
    if (stateName != stateNames.end())
        text = stateName->second;
    return text;
}

void LiftDispatcher::changeState(DispatcherState nextState) {
    state = nextState;
    emit dispatcherStateChanged(stateText());
    emit eventReported("Состояние диспетчера: " + stateText());
}

void LiftDispatcher::connectParts() {
    connect(car, &LiftCar::floorReached, this, &LiftDispatcher::handleFloorReached);
    connect(car, &LiftCar::movementStopped, this, &LiftDispatcher::handleMovementStopped);
    connect(car, &LiftCar::stateChanged, this, &LiftDispatcher::carStateChanged);
    connect(doors, &DoorMechanism::opened, this, &LiftDispatcher::handleDoorsOpened);
    connect(doors, &DoorMechanism::closed, this, &LiftDispatcher::handleDoorsClosed);
    connect(doors, &DoorMechanism::stateChanged, this, &LiftDispatcher::doorStateChanged);
}

void LiftDispatcher::addRequest(int floor) {
    if (!LiftConstants::isFloorValid(floor)) {
        emit eventReported("Некорректный этаж в заявке: " + QString::number(floor));
        return;
    }

    storage.add(floor);
    emit eventReported("Получен вызов на этаж " + QString::number(floor));
    processNextRequest();
}

void LiftDispatcher::processNextRequest() {
    if (state == DispatcherState::Idle && !storage.isEmpty()) {
        selectDestination();
        if (destinationFloor == car->currentFloor())
            serveCurrentFloor();
        else
            startTrip();
    }
}

void LiftDispatcher::selectDestination() {
    changeState(DispatcherState::SelectingTarget);
    destinationFloor = planner.nextDestination(car->currentFloor(), car->direction(), storage.requests());
    emit targetFloorChanged(destinationFloor);
    emit eventReported("Новый целевой этаж: " + QString::number(destinationFloor));
}

void LiftDispatcher::startTrip() {
    changeState(DispatcherState::Moving);
    emit eventReported("Движение началось");
    car->prepareForMovement(directionToDestination());
    car->beginMovement();
}

void LiftDispatcher::serveCurrentFloor() {
    changeState(DispatcherState::ServingFloor);
    car->lockCabin();
    reportStartedService();
    doors->openDoors();
}

void LiftDispatcher::reportStartedService() {
    int floor = car->currentFloor();
    if (storage.containsFloor(floor))
        emit requestServed(floor);
}

int LiftDispatcher::directionToDestination() const {
    int direction = LiftConstants::noDirection;
    if (destinationFloor > car->currentFloor())
        direction = LiftConstants::upDirection;
    if (destinationFloor < car->currentFloor())
        direction = LiftConstants::downDirection;
    return direction;
}

void LiftDispatcher::handleFloorReached(int floor) {
    emit currentFloorChanged(floor);
    emit eventReported("Лифт прибыл на этаж " + QString::number(floor));
    if (planner.shouldServeFloor(floor, storage.requests()))
        car->stopAtCurrentFloor();
}

void LiftDispatcher::handleMovementStopped(int floor) {
    emit eventReported("Кабина остановилась на этаже " + QString::number(floor));
    serveCurrentFloor();
}

void LiftDispatcher::handleDoorsOpened() {
    emit eventReported("Двери открылись");
}

void LiftDispatcher::handleDoorsClosed() {
    emit eventReported("Двери закрылись");
    storage.eraseFloor(car->currentFloor());
    car->releaseCabin();
    changeState(DispatcherState::Idle);
    processNextRequest();
}

int LiftDispatcher::currentFloor() const {
    return car->currentFloor();
}

int LiftDispatcher::direction() const {
    return car->direction();
}

bool LiftDispatcher::isFree() const {
    return state == DispatcherState::Idle && storage.isEmpty();
}

bool LiftDispatcher::canServeHallRequest(int floor) const {
    bool canServe = isFree();
    if (!canServe)
        canServe = planner.canServeOnRoute(car->currentFloor(), car->direction(), floor);
    return canServe;
}
