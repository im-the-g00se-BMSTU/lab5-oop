#ifndef LIFT_CAR_H
#define LIFT_CAR_H

#include "lift_constants.h"

#include <QObject>
#include <QTimer>
#include <map>

class LiftCar : public QObject {
    Q_OBJECT

private:
    enum class CarState {
        Parked,
        Preparing,
        Moving,
        Locked,
        Ready,
        Stuck
    };

    QTimer travelTimer;
    int activeFloor;
    int currentDirection;
    int plannedDirection;
    bool movementPaused;
    CarState state;
    CarState stateBeforePause;
    std::map<CarState, QString> stateNames;

    void setupStateNames();
    QString stateText() const;
    void changeState(CarState nextState);
    void setDirection(int direction);

private slots:
    void completeFloorStep();

public:
    explicit LiftCar(QObject* parent = nullptr);

    int currentFloor() const;
    int direction() const;

    void prepareForMovement(int direction);
    void beginMovement();
    void stopAtCurrentFloor();
    void lockCabin();
    void releaseCabin();
    void setMovementPaused(bool paused);

signals:
    void floorReached(int floor);
    void movementStopped(int floor);
    void stateChanged(QString stateName);
};

#endif // LIFT_CAR_H
