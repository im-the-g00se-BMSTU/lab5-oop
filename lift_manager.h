#ifndef LIFT_MANAGER_H
#define LIFT_MANAGER_H

#include "lift_constants.h"
#include "lift_dispatcher.h"

#include <new>
#include <QObject>
#include <QString>
#include <vector>

class LiftDispatcher;

class LiftManager : public QObject {
    Q_OBJECT

private:
    std::vector<LiftDispatcher*> dispatchers;
    int dispatcherCount;
    QString dispatcherTypeName;

    void createDispatchers();
    void connectDispatcherReports();
    bool isLiftIndexValid(int liftIndex) const;
    int selectDispatcher(int floor) const;
    int distanceToRequest(const LiftDispatcher* dispatcher, int floor) const;

public:
    explicit LiftManager(int liftCount, const QString& liftType, QObject* parent = nullptr);

    int liftCount() const;
    LiftDispatcher* dispatcherAt(int liftIndex) const;

public slots:
    void requestAvailableLift(int floor);
    void requestSpecificLift(int liftIndex, int floor);

signals:
    void eventReported(QString message);
    void liftEventReported(int liftIndex, QString message);
    void hallRequestAssigned(int liftIndex, int floor);
};

#endif // LIFT_MANAGER_H
