#ifndef ELEVATORCONTROLLER_H
#define ELEVATORCONTROLLER_H

#include "cabin.h"
#include "doors.h"
#include "elevatorrequestqueue.h"
#include "directionaltargetselection.h"
#include "elevatorconfig.h"
#include "actiontable.h"
#include "statenametable.h"

#include <QObject>
#include <QString>
#include <map>
#include <string>

class ElevatorController : public QObject {
    Q_OBJECT

private:
    enum class ControllerCommand {
        START_PROCESSING,
        START_MOVING,
        UPDATE_TARGET,
        REACH_TARGET,
        FINISH_PROCESSING
    };

    enum class ControllerState {
        FREE,
        MOVING,
        TARGET_UPDATING,
        TARGET_REACHED
    };

    Cabin* cabin;
    Doors* doors;

    std::unique_ptr<ITargetSelectionStrategy> targetSelection;
    ElevatorRequestQueue requestQueue;

    int targetFloor;

    ActionTable<ElevatorController, ControllerCommand, ControllerState> actions;
    StateNameTable<ControllerState> stateNames;
    ControllerState state;

    void setupConnections();
    void setupActions();
    void setupStateNames();
    void setState(ControllerState newState);

    int getDirectionToTarget() const;

    void selectNextTarget();
    void startProcessing();
    void startMoving();
    void updateTarget();
    void reachTarget();
    void finishProcessing();

public:
    explicit ElevatorController(QObject* parent = nullptr);

public slots:
    void callFromFloor(int floor);
    void selectCabinFloor(int floor);
    void handleFloorChanged(int floor);
    void handleCabinStopped(int floor);
    void handleDoorsOpened();
    void handleDoorsClosed();

signals:
    void currentFloorChanged(int floor);
    void targetFloorChanged(int floor);
    void controllerStateChanged(QString stateName);
    void cabinStateChanged(QString stateName);
    void doorStateChanged(QString stateName);
};
#endif // ELEVATORCONTROLLER_H
