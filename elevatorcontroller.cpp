#include "elevatorcontroller.h"

#include <QDebug>

ElevatorController::ElevatorController(QObject* parent) : QObject(parent), cabin(new Cabin(this)), doors(new Doors(this))
    , targetSelection(std::make_unique<DirectionalTargetSelection>()), targetFloor(ElevatorConfig::FIRST_FLOOR), state(ControllerState::FREE) {
    setupConnections();
    setupStateNames();
    setupActions();
}

void ElevatorController::setupConnections() {
    connect(cabin, &Cabin::floorChanged, this, &ElevatorController::handleFloorChanged);
    connect(cabin, &Cabin::cabinStopped, this, &ElevatorController::handleCabinStopped);
    connect(cabin, &Cabin::cabinStateChanged, this, &ElevatorController::cabinStateChanged);

    connect(doors, &Doors::doorsOpened, this, &ElevatorController::handleDoorsOpened);
    connect(doors, &Doors::doorsClosed, this, &ElevatorController::handleDoorsClosed);
    connect(doors, &Doors::doorStateChanged, this, &ElevatorController::doorStateChanged);
}

void ElevatorController::setupStateNames() {
    stateNames.add(ControllerState::FREE, "FREE");
    stateNames.add(ControllerState::MOVING, "MOVING");
    stateNames.add(ControllerState::TARGET_UPDATING, "TARGET_UPDATING");
    stateNames.add(ControllerState::TARGET_REACHED, "TARGET_REACHED");
}

void ElevatorController::setupActions() {
    actions.add(ControllerCommand::START_PROCESSING, ControllerState::FREE, &ElevatorController::startProcessing);
    actions.add(ControllerCommand::START_PROCESSING, ControllerState::MOVING, &ElevatorController::updateTarget);
    actions.add(ControllerCommand::START_MOVING, ControllerState::TARGET_UPDATING, &ElevatorController::startMoving);
    actions.add(ControllerCommand::UPDATE_TARGET, ControllerState::MOVING, &ElevatorController::updateTarget);
    actions.add(ControllerCommand::REACH_TARGET, ControllerState::MOVING, &ElevatorController::reachTarget);
    actions.add(ControllerCommand::REACH_TARGET, ControllerState::TARGET_UPDATING, &ElevatorController::reachTarget);
    actions.add(ControllerCommand::FINISH_PROCESSING, ControllerState::TARGET_REACHED, &ElevatorController::finishProcessing);
}

void ElevatorController::setState(ControllerState newState) {
    state = newState;
    QString stateName = QString::fromStdString(stateNames.getName(state));
    emit controllerStateChanged(stateName);
    qDebug() << "Controller state:" << stateName;
}

void ElevatorController::callFromFloor(int floor) {
    if (ElevatorConfig::isValidFloor(floor)) {
        qDebug() << "Elevator called from floor:" << floor;
        requestQueue.addRequest(floor, RequestSource::FLOOR);
        actions.execute(this, ControllerCommand::START_PROCESSING, state);
    }
}

void ElevatorController::selectCabinFloor(int floor) {
    if (ElevatorConfig::isValidFloor(floor)) {
        qDebug() << "Cabin floor selected:" << floor;
        requestQueue.addRequest(floor, RequestSource::CABIN);
        actions.execute(this, ControllerCommand::START_PROCESSING, state);
    }
}

void ElevatorController::handleFloorChanged(int floor) {
    bool needStop = targetSelection->shouldStop(floor, requestQueue.getRequests());
    emit currentFloorChanged(floor);

    if (needStop)
        actions.execute(this, ControllerCommand::REACH_TARGET, state);
    else
        actions.execute(this, ControllerCommand::UPDATE_TARGET, state);
}

void ElevatorController::handleCabinStopped(int floor) {
    qDebug() << "Controller detected cabin stop on floor:" << floor;
}

void ElevatorController::handleDoorsOpened() {
    qDebug() << "Controller detected opened doors";
}

void ElevatorController::handleDoorsClosed() {
    qDebug() << "Controller detected closed doors";
    cabin->unlock();
    requestQueue.removeRequestsForFloor(cabin->getCurrentFloor());
    actions.execute(this, ControllerCommand::FINISH_PROCESSING, state);
}

int ElevatorController::getDirectionToTarget() const {
    int result = 0;
    if (targetFloor > cabin->getCurrentFloor())
        result = 1;
    if (targetFloor < cabin->getCurrentFloor())
        result = -1;
    return result;
}

void ElevatorController::selectNextTarget() {
    targetFloor = targetSelection->selectTarget(cabin->getCurrentFloor(), cabin->getDirection(), requestQueue.getRequests());
    emit targetFloorChanged(targetFloor);
    qDebug() << "New target floor:" << targetFloor;
}

void ElevatorController::startProcessing() {
    if (requestQueue.hasRequests()) {
        setState(ControllerState::TARGET_UPDATING);
        selectNextTarget();
        if (targetFloor == cabin->getCurrentFloor())
            actions.execute(this, ControllerCommand::REACH_TARGET, state);
        else
            actions.execute(this, ControllerCommand::START_MOVING, state);
    }
}

void ElevatorController::startMoving() {
    setState(ControllerState::MOVING);
    qDebug() << "Controller starts moving";
    cabin->prepareMoving(getDirectionToTarget());
    cabin->startMoving();
}

void ElevatorController::updateTarget() {
    setState(ControllerState::TARGET_UPDATING);
    selectNextTarget();
    actions.execute(this, ControllerCommand::START_MOVING, state);
}

void ElevatorController::reachTarget() {
    setState(ControllerState::TARGET_REACHED);
    cabin->stopMoving();
    cabin->lock();
    doors->open();
}

void ElevatorController::finishProcessing() {
    setState(ControllerState::FREE);
    if (requestQueue.hasRequests()) {
        actions.execute(this, ControllerCommand::START_PROCESSING, state);
    }
}
