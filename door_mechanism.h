#ifndef DOOR_MECHANISM_H
#define DOOR_MECHANISM_H

#include "lift_constants.h"

#include <QObject>
#include <QTimer>

class DoorMechanism : public QObject {
    Q_OBJECT

private:
    enum class DoorState {
        Opening,
        Open,
        Closing,
        Closed
    };

    QTimer openingTimer;
    QTimer stayingOpenTimer;
    QTimer closingTimer;
    DoorState state;

    QString stateText() const;
    void changeState(DoorState nextState);
    void stopAllTimers();
    void beginOpening();
    void holdOpen();
    void beginClosing();

private slots:
    void finishOpening();
    void finishWaiting();
    void finishClosing();

public:
    explicit DoorMechanism(QObject* parent = nullptr);

    void openDoors();
    void closeDoors();

signals:
    void opened();
    void closed();
    void stateChanged(QString stateName);
};

#endif // DOOR_MECHANISM_H
