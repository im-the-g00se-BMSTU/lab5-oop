#include "doors.h"
#include <QDebug>

Doors::Doors(QObject* parent) : QObject(parent), state(DoorState::CLOSED) {
    openingTimer.setSingleShot(true);
    closingTimer.setSingleShot(true);
    waitingTimer.setSingleShot(true);

    setupStateNames();
    setupActions();

    connect(&openingTimer, &QTimer::timeout, this, &Doors::finishOpening);
    connect(&waitingTimer, &QTimer::timeout, this, &Doors::finishWaiting);
    connect(&closingTimer, &QTimer::timeout, this, &Doors::finishClosing);
}

void Doors::setupStateNames() {
    stateNames.add(DoorState::OPENING, "OPENING");
    stateNames.add(DoorState::OPENED, "OPENED");
    stateNames.add(DoorState::CLOSING, "CLOSING");
    stateNames.add(DoorState::CLOSED, "CLOSED");
}

void Doors::setupActions() {
    actions.add(DoorCommand::OPEN, DoorState::CLOSED, &Doors::startOpening);
    actions.add(DoorCommand::OPEN, DoorState::CLOSING, &Doors::startOpening);
    actions.add(DoorCommand::CLOSE, DoorState::OPENED, &Doors::startClosing);
}

void Doors::setState(DoorState newState) {
    state = newState;
    QString stateName = QString::fromStdString(stateNames.getName(state));
    emit doorStateChanged(stateName);
    qDebug() << "Door state:" << stateName;
}

void Doors::open() {
    actions.execute(this, DoorCommand::OPEN, state);
}

void Doors::close() {
    actions.execute(this, DoorCommand::CLOSE, state);
}

void Doors::stopTimers() {
    openingTimer.stop();
    closingTimer.stop();
    waitingTimer.stop();
}

void Doors::startOpening() {
    stopTimers();
    setState(DoorState::OPENING);
    qDebug() << "Doors opening";
    openingTimer.start(OPENING_TIME);
}

void Doors::startWaiting() {
    stopTimers();
    setState(DoorState::OPENED);
    qDebug() << "Doors opened";
    emit doorsOpened();
    waitingTimer.start(WAITING_TIME);
}

void Doors::startClosing() {
    stopTimers();
    setState(DoorState::CLOSING);
    qDebug() << "Doors closing";
    closingTimer.start(CLOSING_TIME);
}

void Doors::finishOpening() {
    startWaiting();
}

void Doors::finishWaiting() {
    close();
}

void Doors::finishClosing() {
    setState(DoorState::CLOSED);
    qDebug() << "Doors closed";
    emit doorsClosed();
}
