#ifndef CAR_H
#define CAR_H

#include "constants.h"

#include <QObject>
#include <QTimer>
#include <map>

class Car : public QObject {
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
    std::map<CarState, QString> stateNames;

    void setupStateNames();
    QString stateText() const;
    void changeState(CarState nextState);

private slots:
    void completeFloorStep();

public:
    explicit Car(QObject* parent = nullptr);

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

#endif // CAR_H
