#include "lift_car.h"

LiftCar::LiftCar(QObject* parent)
    : QObject(parent),
    activeFloor(LiftConstants::firstFloor),
    currentDirection(LiftConstants::noDirection),
    plannedDirection(LiftConstants::noDirection),
    state(CarState::Parked) {
    travelTimer.setSingleShot(false);
    connect(&travelTimer, &QTimer::timeout, this, &LiftCar::completeFloorStep);
}

QString LiftCar::stateText() const {
    QString text = "PARKED";
    if (state == CarState::Preparing)
        text = "PREPARING";
    if (state == CarState::Moving)
        text = "MOVING";
    if (state == CarState::Locked)
        text = "LOCKED";
    if (state == CarState::Ready)
        text = "READY";
    return text;
}

void LiftCar::changeState(CarState nextState) {
    state = nextState;
    emit stateChanged(stateText());
}

void LiftCar::setDirection(int direction) {
    currentDirection = direction;
}

int LiftCar::currentFloor() const {
    return activeFloor;
}

int LiftCar::direction() const {
    return currentDirection;
}

void LiftCar::prepareForMovement(int direction) {
    bool canPrepare = state == CarState::Parked || state == CarState::Ready;
    if (canPrepare) {
        plannedDirection = direction;
        changeState(CarState::Preparing);
    }
}

void LiftCar::beginMovement() {
    if (state == CarState::Preparing) {
        setDirection(plannedDirection);
        changeState(CarState::Moving);
        travelTimer.start(LiftConstants::travelIntervalMs);
    }
}

void LiftCar::stopAtCurrentFloor() {
    if (state == CarState::Moving) {
        travelTimer.stop();
        setDirection(LiftConstants::noDirection);
        changeState(CarState::Parked);
        emit movementStopped(activeFloor);
    }
}

void LiftCar::lockCabin() {
    bool canLock = state == CarState::Parked || state == CarState::Ready;
    if (canLock)
        changeState(CarState::Locked);
}

void LiftCar::releaseCabin() {
    if (state == CarState::Locked)
        changeState(CarState::Ready);
}

void LiftCar::completeFloorStep() {
    activeFloor += currentDirection;
    emit floorReached(activeFloor);
}
