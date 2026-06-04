#ifndef DOOR_H
#define DOOR_H

#include "constants.h"

#include <QObject>
#include <QTimer>
#include <map>

class Door : public QObject {
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
    std::map<DoorState, QString> stateNames;

    void setupStateNames();
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
    explicit Door(QObject* parent = nullptr);

    void openDoors();
    void closeDoors();

signals:
    void opened();
    void closed();
    void stateChanged(QString stateName);
};

#endif // DOOR_H
