#ifndef PANEL_H
#define PANEL_H

#include "dispatcher.h"

#include <QByteArray>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>
#include <QPushButton>
#include <QSizePolicy>
#include <QStyle>
#include <QGroupBox>
#include <QVBoxLayout>
#include <map>
#include <vector>

class QLabel;
class QGridLayout;
class QHBoxLayout;

struct StatusLabels {
    QLabel* currentFloor;
    QLabel* targetFloor;
    QLabel* dispatcherState;
    QLabel* cabinState;
    QLabel* doorState;
};

class UiPanel : public QGroupBox {
    Q_OBJECT

private:
    std::vector<QLabel*> shaftLabels;
    std::vector<QPushButton*> cabinButtons;
    StatusLabels statusLabels;
    std::map<int, QMovie*> animationMovies;
    QString dispatcherStateName;

    void setupLayout();
    void addShaftRows(QGridLayout* layout);
    void addCabinButtons(QHBoxLayout* layout);
    void addStatusRows(QGridLayout* layout);
    void drawCabinAtFloor(int floor);
    void setCabinButtonActive(int floor, bool isActive);
    void clearCabinButtons();
    void setCabinButtonsEnabled(bool isEnabled);

public:
    explicit UiPanel(const QString& title, QWidget* parent = nullptr);

public slots:
    void setCurrentFloor(int floor);
    void setTargetFloor(int floor);
    void setDispatcherState(const QString& state);
    void setCabinState(const QString& state);
    void setDoorState(const QString& state);
    void clearCabinRequest(int floor);
    void startAnimation(const QString& resourcePath, int floor);
    void stopAnimation(int floor);

signals:
    void cabinFloorRequested(int floor);
};

#endif // PANEL_H
