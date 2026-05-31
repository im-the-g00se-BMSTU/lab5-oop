#ifndef LIFT_CAR_H
#define LIFT_CAR_H

#include "lift_constants.h"

#include <QObject>
#include <QTimer>

class LiftCar : public QObject {
    Q_OBJECT

private:
    enum class CarState {
        Parked,
        Preparing,
        Moving,
        Locked,
        Ready
    };

    QTimer travelTimer;
    int activeFloor;
    int currentDirection;
    int plannedDirection;
    CarState state;

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

signals:
    void floorReached(int floor);
    void movementStopped(int floor);
    void stateChanged(QString stateName);
};

#endif // LIFT_CAR_H
