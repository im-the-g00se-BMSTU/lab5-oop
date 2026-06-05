#ifndef FACADE_H
#define FACADE_H

#include "dispatcher.h"
#include "dispatcher_pool.h"
#include "manager.h"
#include "panel.h"
#include "manager_strategy.h"

#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QObject>
#include <QSizePolicy>
#include <QString>
#include <QStyle>
#include <vector>

class Manager;
class UiPanel;
class QPushButton;
class QFormLayout;
class QGridLayout;
class QGroupBox;
class QWidget;

class Facade : public QObject {
    Q_OBJECT

private:
    DispatcherPool* dispatcherPool;
    Manager* manager;
    QGroupBox* liftGroupBox;
    QGroupBox* callGroupBox;
    QGridLayout* liftGridLayout;
    std::vector<UiPanel*> liftPanels;
    std::vector<QPushButton*> hallButtons;
    std::vector<bool> blockedHallRequests;
    QString audienceName;

    void setupCallLayout();
    void setupLiftLayout();
    void createHallButtons(QFormLayout* callLayout);
    void createLiftPanels();
    void connectHallButtons();
    void connectManagerSignals();
    void connectLiftPanel(int liftIndex);
    void connectLiftAnimation(int liftIndex, UiPanel* panel);
    bool isHallRequestBlocked(int floor) const;
    void setHallButtonActive(int floor, bool isActive);
    void setHallRequestBlocked(int floor, bool isBlocked);
    void completeHallRequest(int floor);
    int liftGridColumnCount() const;

public:
    explicit Facade(
        int liftCount,
        const QString& audience,
        DispatcherPool* dispatcherPool,
        ManagerStrategy* serviceStrategy,
        QWidget* parent = nullptr
    );
    ~Facade() override;

    int stretchFactor() const;
    QWidget* liftWidget() const;
    QWidget* callWidget() const;

};

#endif // FACADE_H
