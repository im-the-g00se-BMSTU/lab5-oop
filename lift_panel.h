#ifndef LIFT_PANEL_H
#define LIFT_PANEL_H

#include "lift_constants.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QStyle>
#include <QGroupBox>
#include <QVBoxLayout>
#include <vector>

class QLabel;
class QGridLayout;
class QHBoxLayout;

struct LiftStatusLabels {
    QLabel* currentFloor;
    QLabel* targetFloor;
    QLabel* dispatcherState;
    QLabel* carState;
    QLabel* doorState;
};

class LiftPanel : public QGroupBox {
    Q_OBJECT

private:
    std::vector<QLabel*> shaftLabels;
    std::vector<QPushButton*> cabinButtons;
    LiftStatusLabels statusLabels;

    void setupLayout();
    void addShaftRows(QGridLayout* layout);
    void addCabinButtons(QHBoxLayout* layout);
    void addStatusRows(QGridLayout* layout);
    void drawCarAtFloor(int floor);
    QPushButton* cabinButtonAt(int floor) const;
    void setCabinButtonActive(int floor, bool isActive);

public:
    explicit LiftPanel(const QString& title, QWidget* parent = nullptr);

public slots:
    void setCurrentFloor(int floor);
    void setTargetFloor(int floor);
    void setDispatcherState(const QString& state);
    void setCarState(const QString& state);
    void setDoorState(const QString& state);
    void clearCabinRequest(int floor);

signals:
    void cabinFloorRequested(int floor);
};

#endif // LIFT_PANEL_H
