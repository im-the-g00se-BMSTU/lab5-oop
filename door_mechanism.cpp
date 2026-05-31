#include "door_mechanism.h"

DoorMechanism::DoorMechanism(QObject* parent)
    : QObject(parent),
    state(DoorState::Closed) {
    setupStateNames();
    openingTimer.setSingleShot(true);
    stayingOpenTimer.setSingleShot(true);
    closingTimer.setSingleShot(true);

    connect(&openingTimer, &QTimer::timeout, this, &DoorMechanism::finishOpening);
    connect(&stayingOpenTimer, &QTimer::timeout, this, &DoorMechanism::finishWaiting);
    connect(&closingTimer, &QTimer::timeout, this, &DoorMechanism::finishClosing);
}

void DoorMechanism::setupStateNames() {
    stateNames[DoorState::Opening] = "OPENING";
    stateNames[DoorState::Open] = "OPEN";
    stateNames[DoorState::Closing] = "CLOSING";
    stateNames[DoorState::Closed] = "CLOSED";
}

QString DoorMechanism::stateText() const {
    QString text;
    auto stateName = stateNames.find(state);
    if (stateName != stateNames.end())
        text = stateName->second;
    return text;
}

void DoorMechanism::changeState(DoorState nextState) {
    state = nextState;
    emit stateChanged(stateText());
}

void DoorMechanism::stopAllTimers() {
    openingTimer.stop();
    stayingOpenTimer.stop();
    closingTimer.stop();
}

void DoorMechanism::beginOpening() {
    stopAllTimers();
    changeState(DoorState::Opening);
    openingTimer.start(LiftConstants::doorOpenIntervalMs);
}

void DoorMechanism::holdOpen() {
    stopAllTimers();
    changeState(DoorState::Open);
    emit opened();
    stayingOpenTimer.start(LiftConstants::floorWaitIntervalMs);
}

void DoorMechanism::beginClosing() {
    stopAllTimers();
    changeState(DoorState::Closing);
    closingTimer.start(LiftConstants::doorCloseIntervalMs);
}

void DoorMechanism::openDoors() {
    if (state == DoorState::Closed || state == DoorState::Closing)
        beginOpening();
}

void DoorMechanism::closeDoors() {
    if (state == DoorState::Open)
        beginClosing();
}

void DoorMechanism::finishOpening() {
    holdOpen();
}

void DoorMechanism::finishWaiting() {
    closeDoors();
}

void DoorMechanism::finishClosing() {
    changeState(DoorState::Closed);
    emit closed();
}
