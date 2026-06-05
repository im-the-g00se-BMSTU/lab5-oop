#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "facade.h"

#include <QMainWindow>
#include <vector>

class Facade;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    static constexpr int studentLiftCount = 8;
    static constexpr int teacherLiftCount = 2;
    static constexpr int maxLiftCount = 12;

    Ui::MainWindow* ui;
    std::vector<Facade*> liftGroups;

    void addLiftGroup(Facade* group);

    static_assert(studentLiftCount + teacherLiftCount <= maxLiftCount,
                  "Total lift count exceeds maximum allowed");

    static_assert(!(studentLiftCount + teacherLiftCount == maxLiftCount
                    && (studentLiftCount % 2 != 0 || teacherLiftCount % 2 != 0)),
                  "When using maxLiftCount lifts, both studentLiftCount and teacherLiftCount must be even numbers");

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
};

#endif // MAINWINDOW_H
