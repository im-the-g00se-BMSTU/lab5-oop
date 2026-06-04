#ifndef MANAGER_STRATEGY_H
#define MANAGER_STRATEGY_H

#include "constants.h"
#include "dispatcher.h"

#include <QObject>
#include <QString>
#include <QTimer>
#include <random>
#include <vector>

class Dispatcher;

class ManagerStrategy : public QObject {
    Q_OBJECT

protected:
    int distanceToRequest(const Dispatcher* dispatcher, int floor) const;
    bool randomChanceGenerator(int chanceDenominator) const;
    int selectNearestDispatcher(const std::vector<Dispatcher*>& dispatchers, int floor) const;

public:
    virtual ~ManagerStrategy() = default;

    virtual int selectDispatcher(const std::vector<Dispatcher*>& dispatchers, int floor) const = 0;
    virtual bool handleCabinRequest(Dispatcher& dispatcher, int liftIndex, int floor) = 0;
    virtual bool handleHallRequest(Dispatcher& dispatcher, int liftIndex, int floor) = 0;

signals:
    void eventReported(QString message);
    void liftAnimationStarted(int liftIndex, int floor, QString resourcePath);
    void liftAnimationStopped(int liftIndex, int floor);
};

class StudentLiftStrategy : public ManagerStrategy {
private:
    int selectAvailableDispatcher(const std::vector<Dispatcher*>& dispatchers, int floor) const;
    bool makeDispatcherStuckByChance(Dispatcher& dispatcher) const;
    void reportIgnoredFloor(int floor);

public:
    int selectDispatcher(const std::vector<Dispatcher*>& dispatchers, int floor) const override;
    bool handleCabinRequest(Dispatcher& dispatcher, int liftIndex, int floor) override;
    bool handleHallRequest(Dispatcher& dispatcher, int liftIndex, int floor) override;
};

class TeacherLiftStrategy : public ManagerStrategy {
public:
    int selectDispatcher(const std::vector<Dispatcher*>& dispatchers, int floor) const override;
    bool handleCabinRequest(Dispatcher& dispatcher, int liftIndex, int floor) override;
    bool handleHallRequest(Dispatcher& dispatcher, int liftIndex, int floor) override;
};

#endif // MANAGER_STRATEGY_H
