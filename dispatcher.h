#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "door.h"
#include "car.h"
#include "request_storage.h"
#include "route_planner.h"
#include "constants.h"

#include <QObject>
#include <map>

class Dispatcher : public QObject {
    Q_OBJECT

private:
    enum class DispatcherState {
        Idle,
        SelectingTarget,
        Moving,
        ServingFloor,
        Stuck
    };

    Car* car;
    Door* doors;
    RequestStorage storage;
    RoutePlanner planner;
    DispatcherState state;
    int destinationFloor;
    std::map<DispatcherState, QString> stateNames;

    void setupStateNames();
    QString stateText() const;
    void changeState(DispatcherState nextState);
    void connectParts();
    void processNextRequest();
    void selectDestination();
    void startTrip();
    void serveCurrentFloor();

private slots:
    void handleFloorReached(int floor);
    void handleMovementStopped(int floor);
    void handleDoorsOpened();
    void handleDoorsClosed();

public:
    explicit Dispatcher(QObject* parent = nullptr);
    int currentFloor() const;
    int direction() const;
    bool isStuck() const;
    bool canServeHallRequest(int floor) const;
    void makeStuck();

public slots:
    void addRequest(int floor);

signals:
    void currentFloorChanged(int floor);
    void targetFloorChanged(int floor);
    void dispatcherStateChanged(QString stateName);
    void carStateChanged(QString stateName);
    void doorStateChanged(QString stateName);
    void eventReported(QString message);
    void messageBoxRequested(QString message);
    void requestServed(int floor);
    void requestCompleted(int floor);
};

#endif // DISPATCHER_H
