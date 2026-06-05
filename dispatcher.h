#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "door.h"
#include "cabin.h"
#include "direction.h"
#include "request_storage.h"
#include "route_planner.h"
#include "logger.h"

#include <QObject>
#include <QString>
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

    Cabin* cabin;
    Door* doors;
    RequestStorage storage;
    RoutePlanner planner;
    DispatcherState state;
    int destinationFloor;
    QString reportPrefix;
    std::map<DispatcherState, QString> stateNames;

    void setupStateNames();
    QString stateText() const;
    void reportEvent(const QString& message) const;
    void reportMessageBox(const QString& message) const;
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
    static constexpr int floorCount = 5;

    explicit Dispatcher(const QString& reportPrefix = "", QObject* parent = nullptr);
    int currentFloor() const;
    Direction direction() const;
    bool isStuck() const;
    static bool isFloorValid(int floor);
    bool canServeHallRequest(int floor) const;
    void makeStuck();

public slots:
    void addRequest(int floor);

signals:
    void currentFloorChanged(int floor);
    void targetFloorChanged(int floor);
    void dispatcherStateChanged(QString stateName);
    void cabinStateChanged(QString stateName);
    void doorStateChanged(QString stateName);
    void requestServed(int floor);
    void requestCompleted(int floor);
};

#endif // DISPATCHER_H
