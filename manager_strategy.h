#ifndef MANAGER_STRATEGY_H
#define MANAGER_STRATEGY_H

#include "dispatcher.h"
#include "dispatcher_pool.h"
#include "logger.h"

#include <QObject>
#include <QString>
#include <QTimer>
#include <cstdlib>

class Dispatcher;

class ManagerStrategy : public QObject {
    Q_OBJECT

protected:
    DispatcherPool& dispatcherPool;

    bool randomChanceGenerator(int chanceDenominator) const {
        bool isGenerated = false;
        if (chanceDenominator > 0)
            isGenerated = std::rand() % chanceDenominator == 0;
        return isGenerated;
    }

public:
    explicit ManagerStrategy(DispatcherPool& dispatcherPool)
        : dispatcherPool(dispatcherPool) {}

    virtual ~ManagerStrategy() = default;

    DispatcherPool& pool() const {
        return dispatcherPool;
    }

    virtual int selectDispatcher(int floor) const = 0;
    virtual bool handleCabinRequest(Dispatcher& dispatcher, int liftIndex, int floor) = 0;
    virtual bool handleHallRequest(Dispatcher& dispatcher, int liftIndex, int floor) = 0;

signals:
    void liftAnimationStarted(int liftIndex, int floor, QString resourcePath);
    void liftAnimationStopped(int liftIndex, int floor);
};

class StudentLiftStrategy : public ManagerStrategy {
private:
    static constexpr int ignoredFloor = 3;
    static constexpr int stuckChanceDenominator = 7;

    int selectAvailableDispatcher(int floor) const;
    bool makeDispatcherStuckByChance(Dispatcher& dispatcher) const;
    void reportIgnoredFloor(int floor);

public:
    explicit StudentLiftStrategy(DispatcherPool& dispatcherPool);

    int selectDispatcher(int floor) const override;
    bool handleCabinRequest(Dispatcher& dispatcher, int liftIndex, int floor) override;
    bool handleHallRequest(Dispatcher& dispatcher, int liftIndex, int floor) override;
};

class TeacherLiftStrategy : public ManagerStrategy {
private:
    static constexpr int securityChanceDenominator = 5;
    static constexpr int securityAnimationIntervalMs = 2000;

public:
    explicit TeacherLiftStrategy(DispatcherPool& dispatcherPool);

    int selectDispatcher(int floor) const override;
    bool handleCabinRequest(Dispatcher& dispatcher, int liftIndex, int floor) override;
    bool handleHallRequest(Dispatcher& dispatcher, int liftIndex, int floor) override;
};

#endif // MANAGER_STRATEGY_H
