#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "constants.h"
#include "facade.h"
#include "logger.h"

#include <QMainWindow>
#include <vector>

class Facade;
class Logger;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Ui::MainWindow* ui;
    Logger* logger;
    std::vector<Facade*> liftGroups;

    void addLiftGroup(Facade* group);

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
};

#endif // MAINWINDOW_H
