#include "door_mechanism.h"

DoorMechanism::DoorMechanism(QObject* parent)
    : QObject(parent),
    state(DoorState::Closed) {
    openingTimer.setSingleShot(true);
    stayingOpenTimer.setSingleShot(true);
    closingTimer.setSingleShot(true);

    connect(&openingTimer, &QTimer::timeout, this, &DoorMechanism::finishOpening);
    connect(&stayingOpenTimer, &QTimer::timeout, this, &DoorMechanism::finishWaiting);
    connect(&closingTimer, &QTimer::timeout, this, &DoorMechanism::finishClosing);
}

QString DoorMechanism::stateText() const {
    QString text = "CLOSED";
    if (state == DoorState::Opening)
        text = "OPENING";
    if (state == DoorState::Open)
        text = "OPEN";
    if (state == DoorState::Closing)
        text = "CLOSING";
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
    bool canOpen = state == DoorState::Closed || state == DoorState::Closing;
    if (canOpen)
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
