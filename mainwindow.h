#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lift_constants.h"
#include "lift_group_facade.h"
#include "simulation_logger.h"

#include <QMainWindow>
#include <vector>

class LiftGroupFacade;
class SimulationLogger;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Ui::MainWindow* ui;
    SimulationLogger* logger;
    std::vector<LiftGroupFacade*> liftGroups;

    void addLiftGroup(LiftGroupFacade* group);

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
};

#endif // MAINWINDOW_H
