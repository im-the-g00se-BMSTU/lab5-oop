#ifndef MANAGER_H
#define MANAGER_H

#include "constants.h"
#include "dispatcher.h"
#include "manager_strategy.h"

#include <new>
#include <QObject>
#include <QString>
#include <vector>

class Dispatcher;

class Manager : public QObject {
    Q_OBJECT

private:
    std::vector<Dispatcher*> dispatchers;
    ManagerStrategy* serviceStrategy;
    int dispatcherCount;
    QString dispatcherTypeName;

    void createDispatchers();
    void connectDispatcherReports();
    void connectStrategyReports();
    bool isLiftIndexValid(int liftIndex) const;
    int selectDispatcher(int floor) const;

public:
    explicit Manager(
        int liftCount,
        const QString& liftType,
        ManagerStrategy* serviceStrategy,
        QObject* parent = nullptr
    );
    ~Manager() override;

    int liftCount() const;
    Dispatcher* dispatcherAt(int liftIndex) const;

public slots:
    void requestAvailableLift(int floor);
    void requestSpecificLift(int liftIndex, int floor);

signals:
    void eventReported(QString message);
    void cabinRequestCanceled(int liftIndex, int floor);
    void liftEventReported(int liftIndex, QString message);
    void hallRequestAssigned(int liftIndex, int floor);
    void hallRequestCanceled(int floor);
    void liftAnimationStarted(int liftIndex, int floor, QString resourcePath);
    void liftAnimationStopped(int liftIndex, int floor);
};

#endif // MANAGER_H
