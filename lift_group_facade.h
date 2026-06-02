#ifndef LIFT_GROUP_FACADE_H
#define LIFT_GROUP_FACADE_H

#include "lift_constants.h"
#include "lift_dispatcher.h"
#include "lift_manager.h"
#include "lift_panel.h"

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

class LiftManager;
class LiftPanel;
class QPushButton;
class QFormLayout;
class QGridLayout;
class QGroupBox;
class QWidget;

class LiftGroupFacade : public QObject {
    Q_OBJECT

private:
    LiftManager* manager;
    QGroupBox* liftGroupBox;
    QGroupBox* callGroupBox;
    QGridLayout* liftGridLayout;
    std::vector<LiftPanel*> liftPanels;
    std::vector<QPushButton*> hallButtons;
    QString audienceName;

    void setupCallLayout();
    void setupLiftLayout();
    void createHallButtons(QFormLayout* callLayout);
    void createLiftPanels();
    void connectHallButtons();
    void connectManagerSignals();
    void connectLiftPanel(int liftIndex);
    void setHallButtonActive(int floor, bool isActive);
    void clearHallRequest(int floor);
    int liftGridColumnCount() const;
    int liftGridRow(int liftIndex) const;
    int liftGridColumn(int liftIndex) const;

public:
    explicit LiftGroupFacade(int liftCount, const QString& audience, QWidget* parent = nullptr);

    int stretchFactor() const;
    QWidget* liftWidget() const;
    QWidget* callWidget() const;

signals:
    void eventReported(QString message);
};

#endif // LIFT_GROUP_FACADE_H
