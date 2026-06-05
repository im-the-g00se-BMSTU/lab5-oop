#include "door.h"

// ======== public ========

Door::Door(QObject* parent)
    : QObject(parent),
    state(DoorState::Closed) {
    setupStateNames();
    openingTimer.setSingleShot(true);
    stayingOpenTimer.setSingleShot(true);
    closingTimer.setSingleShot(true);

    connect(&openingTimer, &QTimer::timeout, this, &Door::finishOpening);
    connect(&stayingOpenTimer, &QTimer::timeout, this, &Door::finishWaiting);
    connect(&closingTimer, &QTimer::timeout, this, &Door::finishClosing);
}

void Door::openDoors() {
    if (state == DoorState::Closed || state == DoorState::Closing) {
        stopAllTimers();
        changeState(DoorState::Opening);
        openingTimer.start(openingIntervalMs);
    }
}

void Door::closeDoors() {
    if (state == DoorState::Open) {
        stopAllTimers();
        changeState(DoorState::Closing);
        closingTimer.start(closingIntervalMs);
    }
}

void Door::stopDoors() {
    stopAllTimers();
}

// ======== private ========

void Door::setupStateNames() {
    stateNames[DoorState::Opening] = "OPENING";
    stateNames[DoorState::Open] = "OPEN";
    stateNames[DoorState::Closing] = "CLOSING";
    stateNames[DoorState::Closed] = "CLOSED";
}

QString Door::stateText() const {
    QString text;
    auto stateName = stateNames.find(state);
    if (stateName != stateNames.end())
        text = stateName->second;
    return text;
}

void Door::changeState(DoorState nextState) {
    state = nextState;
    emit stateChanged(stateText());
}

void Door::stopAllTimers() {
    openingTimer.stop();
    stayingOpenTimer.stop();
    closingTimer.stop();
}

// ======== private slots ========

void Door::finishOpening() {
    stopAllTimers();
    changeState(DoorState::Open);
    emit opened();
    stayingOpenTimer.start(stayingOpenIntervalMs);
}

void Door::finishWaiting() {
    closeDoors();
}

void Door::finishClosing() {
    changeState(DoorState::Closed);
    emit closed();
}
