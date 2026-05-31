#ifndef LIFT_DISPATCHER_H
#define LIFT_DISPATCHER_H

#include "door_mechanism.h"
#include "lift_car.h"
#include "request_storage.h"
#include "route_planner.h"
#include "lift_constants.h"

#include <QObject>
#include <map>

class LiftDispatcher : public QObject {
    Q_OBJECT

private:
    enum class DispatcherState {
        Idle,
        SelectingTarget,
        Moving,
        ServingFloor
    };

    LiftCar* car;
    DoorMechanism* doors;
    RequestStorage storage;
    RoutePlanner planner;
    DispatcherState state;
    int destinationFloor;
    std::map<DispatcherState, QString> stateNames;

    void setupStateNames();
    QString stateText() const;
    QString originText(LiftRequestOrigin origin) const;
    void changeState(DispatcherState nextState);
    void connectParts();
    void addRequest(int floor, LiftRequestOrigin origin);
    void processNextRequest();
    void selectDestination();
    void startTrip();
    void serveCurrentFloor();
    int directionToDestination() const;

private slots:
    void handleFloorReached(int floor);
    void handleMovementStopped(int floor);
    void handleDoorsOpened();
    void handleDoorsClosed();

public:
    explicit LiftDispatcher(QObject* parent = nullptr);

public slots:
    void requestFromHall(int floor);
    void requestFromCabin(int floor);
    void setMovementPaused(bool paused);

signals:
    void currentFloorChanged(int floor);
    void targetFloorChanged(int floor);
    void dispatcherStateChanged(QString stateName);
    void carStateChanged(QString stateName);
    void doorStateChanged(QString stateName);
    void eventReported(QString message);
};

#endif // LIFT_DISPATCHER_H
