#include "cabin.h"

// ======== public ========

Cabin::Cabin(QObject* parent)
    : QObject(parent),
    activeFloor(startFloor),
    currentDirection(Direction::None),
    plannedDirection(Direction::None),
    state(CabinState::Parked) {

    setupStateNames();
    travelTimer.setSingleShot(false);
    connect(&travelTimer, &QTimer::timeout, this, &Cabin::completeFloorStep);
}

int Cabin::currentFloor() const {
    return activeFloor;
}

Direction Cabin::direction() const {
    return currentDirection;
}

void Cabin::prepareForMovement(Direction direction) {
    if (state == CabinState::Parked || state == CabinState::Ready) {
        plannedDirection = direction;
        changeState(CabinState::Preparing);
    }
}

void Cabin::beginMovement() {
    if (state == CabinState::Preparing) {
        currentDirection = plannedDirection;
        changeState(CabinState::Moving);
        travelTimer.start(travelIntervalMs);
    }
}

void Cabin::stopAtCurrentFloor() {
    if (state == CabinState::Moving) {
        travelTimer.stop();
        currentDirection = Direction::None;
        changeState(CabinState::Parked);
        emit movementStopped(activeFloor);
    }
}

void Cabin::lockCabin() {
    if (state != CabinState::Locked) {
        travelTimer.stop();
        currentDirection = Direction::None;
        plannedDirection = Direction::None;
        changeState(CabinState::Locked);
    }
}

void Cabin::releaseCabin() {
    if (state == CabinState::Locked)
        changeState(CabinState::Ready);
}

// ======== private ========

void Cabin::setupStateNames() {
    stateNames[CabinState::Parked] = "PARKED";
    stateNames[CabinState::Preparing] = "PREPARING";
    stateNames[CabinState::Moving] = "MOVING";
    stateNames[CabinState::Locked] = "LOCKED";
    stateNames[CabinState::Ready] = "READY";
}

QString Cabin::stateText() const {
    QString text;
    auto stateName = stateNames.find(state);
    if (stateName != stateNames.end())
        text = stateName->second;
    return text;
}

void Cabin::changeState(CabinState nextState) {
    state = nextState;
    emit stateChanged(stateText());
}

// ======== private slots ========

void Cabin::completeFloorStep() {
    activeFloor += static_cast<int>(currentDirection);
    emit floorReached(activeFloor);
}
