#include "lift_dispatcher.h"

LiftDispatcher::LiftDispatcher(QObject* parent)
    : QObject(parent),
    car(new LiftCar(this)),
    doors(new DoorMechanism(this)),
    state(DispatcherState::Idle),
    destinationFloor(LiftConstants::firstFloor) {
    connectParts();
}

QString LiftDispatcher::stateText() const {
    QString text = "IDLE";
    if (state == DispatcherState::SelectingTarget)
        text = "SELECTING_TARGET";
    if (state == DispatcherState::Moving)
        text = "MOVING";
    if (state == DispatcherState::ServingFloor)
        text = "SERVING_FLOOR";
    return text;
}

QString LiftDispatcher::originText(LiftRequestOrigin origin) const {
    return (origin == LiftRequestOrigin::Cabin) ? "cabin" : "hall";
}

void LiftDispatcher::changeState(DispatcherState nextState) {
    state = nextState;
    emit dispatcherStateChanged(stateText());
    emit eventReported("Dispatcher state: " + stateText());
}

void LiftDispatcher::connectParts() {
    connect(car, &LiftCar::floorReached, this, &LiftDispatcher::handleFloorReached);
    connect(car, &LiftCar::movementStopped, this, &LiftDispatcher::handleMovementStopped);
    connect(car, &LiftCar::stateChanged, this, &LiftDispatcher::carStateChanged);
    connect(doors, &DoorMechanism::opened, this, &LiftDispatcher::handleDoorsOpened);
    connect(doors, &DoorMechanism::closed, this, &LiftDispatcher::handleDoorsClosed);
    connect(doors, &DoorMechanism::stateChanged, this, &LiftDispatcher::doorStateChanged);
}

void LiftDispatcher::addRequest(int floor, LiftRequestOrigin origin) {
    if (!LiftConstants::isFloorValid(floor)) {
        emit eventReported("Invalid floor request: " + QString::number(floor));
        return;
    }

    storage.add(floor, origin);
    emit eventReported("Lift requested from " + originText(origin) + " for floor " + QString::number(floor));
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
    emit eventReported("New target floor: " + QString::number(destinationFloor));
}

void LiftDispatcher::startTrip() {
    changeState(DispatcherState::Moving);
    emit eventReported("Movement started");
    car->prepareForMovement(directionToDestination());
    car->beginMovement();
}

void LiftDispatcher::serveCurrentFloor() {
    changeState(DispatcherState::ServingFloor);
    car->lockCabin();
    doors->openDoors();
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
    emit eventReported("Arrived at floor " + QString::number(floor));
    if (planner.shouldServeFloor(floor, storage.requests()))
        car->stopAtCurrentFloor();
}

void LiftDispatcher::handleMovementStopped(int floor) {
    emit eventReported("Cabin stopped at floor " + QString::number(floor));
    serveCurrentFloor();
}

void LiftDispatcher::handleDoorsOpened() {
    emit eventReported("Doors opened");
}

void LiftDispatcher::handleDoorsClosed() {
    emit eventReported("Doors closed");
    storage.eraseFloor(car->currentFloor());
    car->releaseCabin();
    changeState(DispatcherState::Idle);
    processNextRequest();
}

void LiftDispatcher::requestFromHall(int floor) {
    addRequest(floor, LiftRequestOrigin::Hall);
}

void LiftDispatcher::requestFromCabin(int floor) {
    addRequest(floor, LiftRequestOrigin::Cabin);
}
