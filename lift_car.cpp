#include "lift_car.h"

LiftCar::LiftCar(QObject* parent)
    : QObject(parent),
    activeFloor(LiftConstants::firstFloor),
    currentDirection(LiftConstants::noDirection),
    plannedDirection(LiftConstants::noDirection),
    state(CarState::Parked) {

    setupStateNames();
    travelTimer.setSingleShot(false);
    connect(&travelTimer, &QTimer::timeout, this, &LiftCar::completeFloorStep);
}

void LiftCar::setupStateNames() {
    stateNames[CarState::Parked] = "PARKED";
    stateNames[CarState::Preparing] = "PREPARING";
    stateNames[CarState::Moving] = "MOVING";
    stateNames[CarState::Locked] = "LOCKED";
    stateNames[CarState::Ready] = "READY";
}

QString LiftCar::stateText() const {
    QString text;
    auto stateName = stateNames.find(state);
    if (stateName != stateNames.end())
        text = stateName->second;
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
    if (state == CarState::Parked || state == CarState::Ready) {
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
    if (state == CarState::Parked || state == CarState::Ready)
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
