#ifndef CABIN_H
#define CABIN_H

#include "direction.h"

#include <QObject>
#include <QTimer>
#include <map>

class Cabin : public QObject {
    Q_OBJECT

private:
    static constexpr int startFloor = 1;
    static constexpr int travelIntervalMs = 1000;

    enum class CabinState {
        Parked,
        Preparing,
        Moving,
        Locked,
        Ready
    };

    QTimer travelTimer;
    int activeFloor;
    Direction currentDirection;
    Direction plannedDirection;
    CabinState state;
    std::map<CabinState, QString> stateNames;

    void setupStateNames();
    QString stateText() const;
    void changeState(CabinState nextState);

private slots:
    void completeFloorStep();

public:
    explicit Cabin(QObject* parent = nullptr);

    int currentFloor() const;
    Direction direction() const;

    void prepareForMovement(Direction direction);
    void beginMovement();
    void stopAtCurrentFloor();
    void lockCabin();
    void releaseCabin();

signals:
    void floorReached(int floor);
    void movementStopped(int floor);
    void stateChanged(QString stateName);
};

#endif // CABIN_H
