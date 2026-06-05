#ifndef DOOR_H
#define DOOR_H

#include <QObject>
#include <QTimer>
#include <map>

class Door : public QObject {
    Q_OBJECT

private:
    static constexpr int openingIntervalMs = 1000;
    static constexpr int closingIntervalMs = 1000;
    static constexpr int stayingOpenIntervalMs = 1500;

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

private slots:
    void finishOpening();
    void finishWaiting();
    void finishClosing();

public:
    explicit Door(QObject* parent = nullptr);

    void openDoors();
    void closeDoors();
    void stopDoors();

signals:
    void opened();
    void closed();
    void stateChanged(QString stateName);
};

#endif // DOOR_H
