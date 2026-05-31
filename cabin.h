#ifndef CABIN_H
#define CABIN_H

#include "elevatorconfig.h"
#include "actiontable.h"
#include "statenametable.h"

#include <QObject>
#include <QTimer>
#include <QString>
#include <map>
#include <string>

class Cabin : public QObject {
    Q_OBJECT

private:
    static constexpr int MOVING_TIME = 1000;

    enum class CabinCommand {
        PREPARE,
        START,
        STOP,
        LOCK,
        UNLOCK
    };

    enum class CabinState {
        MOVING,
        STOPPED,
        PREPARING,
        BLOCKED,
        UNBLOCKED
    };

    QTimer movingTimer;

    int currentFloor;
    int direction;
    int nextDirection;

    ActionTable<Cabin, CabinCommand, CabinState> actions;
    CabinState state;
    StateNameTable<CabinState> stateNames;

    void setupActions();
    void setupStateNames();
    void setState(CabinState newState);

    void setDirection(int newDirection);
    void startMovingTimer();
    void stopMovingTimer();
    void moveOneFloor();
    void prepareCabin();
    void lockCabin();
    void unlockCabin();

public:
    explicit Cabin(QObject* parent = nullptr);

    int getCurrentFloor() const;
    int getDirection() const;

    void prepareMoving(int newDirection);
    void startMoving();
    void stopMoving();
    void lock();
    void unlock();

signals:
    void floorChanged(int floor);
    void cabinStopped(int floor);
    void cabinStateChanged(QString stateName);

private slots:
    void handleMovingTimer();
};

#endif // CABIN_H
