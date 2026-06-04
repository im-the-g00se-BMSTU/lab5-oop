#include "car.h"

// ======== public ========

Car::Car(QObject* parent)
    : QObject(parent),
    activeFloor(1),
    currentDirection(Constants::noDirection),
    plannedDirection(Constants::noDirection),
    state(CarState::Parked) {

    setupStateNames();
    travelTimer.setSingleShot(false);
    connect(&travelTimer, &QTimer::timeout, this, &Car::completeFloorStep);
}

int Car::currentFloor() const {
    return activeFloor;
}

int Car::direction() const {
    return currentDirection;
}

void Car::prepareForMovement(int direction) {
    if (state == CarState::Parked || state == CarState::Ready) {
        plannedDirection = direction;
        changeState(CarState::Preparing);
    }
}

void Car::beginMovement() {
    if (state == CarState::Preparing) {
        currentDirection = plannedDirection;
        changeState(CarState::Moving);
        travelTimer.start(Constants::travelIntervalMs);
    }
}

void Car::stopAtCurrentFloor() {
    if (state == CarState::Moving) {
        travelTimer.stop();
        currentDirection = Constants::noDirection;
        changeState(CarState::Parked);
        emit movementStopped(activeFloor);
    }
}

void Car::lockCabin() {
    if (state != CarState::Locked) {
        travelTimer.stop();
        currentDirection = Constants::noDirection;
        plannedDirection = Constants::noDirection;
        changeState(CarState::Locked);
    }
}

void Car::releaseCabin() {
    if (state == CarState::Locked)
        changeState(CarState::Ready);
}

// ======== private ========

void Car::setupStateNames() {
    stateNames[CarState::Parked] = "PARKED";
    stateNames[CarState::Preparing] = "PREPARING";
    stateNames[CarState::Moving] = "MOVING";
    stateNames[CarState::Locked] = "LOCKED";
    stateNames[CarState::Ready] = "READY";
}

QString Car::stateText() const {
    QString text;
    auto stateName = stateNames.find(state);
    if (stateName != stateNames.end())
        text = stateName->second;
    return text;
}

void Car::changeState(CarState nextState) {
    state = nextState;
    emit stateChanged(stateText());
}

// ======== private slots ========

void Car::completeFloorStep() {
    activeFloor += currentDirection;
    emit floorReached(activeFloor);
}
