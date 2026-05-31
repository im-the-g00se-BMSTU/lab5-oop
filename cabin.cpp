#include "cabin.h"

#include <QDebug>

Cabin::Cabin(QObject* parent) : QObject(parent), currentFloor(ElevatorConfig::FIRST_FLOOR), direction(0)
    , nextDirection(0), state(CabinState::STOPPED) {
    movingTimer.setSingleShot(false);

    setupStateNames();
    setupActions();

    connect(&movingTimer, &QTimer::timeout, this, &Cabin::handleMovingTimer);
}

void Cabin::setupStateNames() {
    stateNames.add(CabinState::MOVING, "MOVING");
    stateNames.add(CabinState::STOPPED, "STOPPED");
    stateNames.add(CabinState::PREPARING, "PREPARING");
    stateNames.add(CabinState::BLOCKED, "BLOCKED");
    stateNames.add(CabinState::UNBLOCKED, "UNBLOCKED");
}

void Cabin::setupActions() {
    actions.add(CabinCommand::PREPARE, CabinState::STOPPED, &Cabin::prepareCabin);
    actions.add(CabinCommand::PREPARE, CabinState::UNBLOCKED, &Cabin::prepareCabin);
    actions.add(CabinCommand::START, CabinState::PREPARING, &Cabin::startMovingTimer);
    actions.add(CabinCommand::STOP, CabinState::MOVING, &Cabin::stopMovingTimer);
    actions.add(CabinCommand::LOCK, CabinState::STOPPED, &Cabin::lockCabin);
    actions.add(CabinCommand::LOCK, CabinState::UNBLOCKED, &Cabin::lockCabin);
    actions.add(CabinCommand::UNLOCK, CabinState::BLOCKED, &Cabin::unlockCabin);
}


void Cabin::setState(CabinState newState) {
    state = newState;
    QString stateName = QString::fromStdString(stateNames.getName(state));
    emit cabinStateChanged(stateName);
    qDebug() << "Cabin state:" << stateName;
}

void Cabin::prepareCabin() {
    setState(CabinState::PREPARING);
}

void Cabin::lockCabin() {
    setState(CabinState::BLOCKED);
}

void Cabin::unlockCabin() {
    setState(CabinState::UNBLOCKED);
}

int Cabin::getCurrentFloor() const {
    return currentFloor;
}

int Cabin::getDirection() const {
    return direction;
}

void Cabin::prepareMoving(int newDirection) {
    nextDirection = newDirection;
    actions.execute(this, CabinCommand::PREPARE, state);
}

void Cabin::startMoving() {
    actions.execute(this, CabinCommand::START, state);
}

void Cabin::stopMoving() {
    actions.execute(this, CabinCommand::STOP, state);
}

void Cabin::lock() {
    actions.execute(this, CabinCommand::LOCK, state);
}

void Cabin::unlock() {
    actions.execute(this, CabinCommand::UNLOCK, state);
}
void Cabin::setDirection(int newDirection) {
    direction = newDirection;
}

void Cabin::startMovingTimer() {
    setDirection(nextDirection);
    setState(CabinState::MOVING);
    qDebug() << "Cabin starts moving";
    movingTimer.start(MOVING_TIME);
}

void Cabin::stopMovingTimer() {
    movingTimer.stop();
    setDirection(0);
    setState(CabinState::STOPPED);
    qDebug() << "Cabin stopped on floor:" << currentFloor;
    emit cabinStopped(currentFloor);
}

void Cabin::moveOneFloor() {
    currentFloor += direction;
    emit floorChanged(currentFloor);
    qDebug() << "Cabin arrived on floor:" << currentFloor;
}

void Cabin::handleMovingTimer() {
    moveOneFloor();
}