#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>

#include "elevatorcontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow* ui;
    ElevatorController* controller;

    std::vector<QLabel*> shaftLabels;
    std::vector<QPushButton*> floorButtons;
    std::vector<QPushButton*> cabinButtons;

    void setupInterface();
    void setupConnections();
    void updateElevatorPosition(int floor);
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
};
#endif // MAINWINDOW_H
