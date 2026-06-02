#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lift_dispatcher.h"
#include "simulation_logger.h"

#include <QStyle>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Ui::MainWindow* ui;
    LiftDispatcher* dispatcher;
    SimulationLogger* logger;

    std::vector<QLabel*> shaftLabels;
    std::vector<QPushButton*> floorButtons;
    std::vector<QPushButton*> cabinButtons;

    void setupInterface();
    void setupConnections();
    void connectHallButtons();
    void connectCabinButtons();
    void connectStateLabels();
    void drawCarAtFloor(int floor);

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
};

#endif // MAINWINDOW_H
