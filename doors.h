#ifndef DOORS_H
#define DOORS_H

#include <QObject>
#include <QTimer>
#include <QString>
#include "actiontable.h"
#include "statenametable.h"

class Doors : public QObject {
    Q_OBJECT

private:
    static constexpr int OPENING_TIME = 1000;
    static constexpr int CLOSING_TIME = 1000;
    static constexpr int WAITING_TIME = 1500;

    enum class DoorCommand {
        OPEN,
        CLOSE
    };

    enum class DoorState {
        OPENING,
        OPENED,
        CLOSING,
        CLOSED
    };

    QTimer openingTimer;
    QTimer closingTimer;
    QTimer waitingTimer;

    ActionTable<Doors, DoorCommand, DoorState> actions;
    DoorState state;
    StateNameTable<DoorState> stateNames;

    void setupActions();
    void setupStateNames();
    void setState(DoorState newState);

    void stopTimers();
    void startOpening();
    void startWaiting();
    void startClosing();

public:
    explicit Doors(QObject* parent = nullptr);

    void open();
    void close();

signals:
    void doorsOpened();
    void doorsClosed();
    void doorStateChanged(QString stateName);

private slots:
    void finishOpening();
    void finishWaiting();
    void finishClosing();
};

#endif // DOORS_H
