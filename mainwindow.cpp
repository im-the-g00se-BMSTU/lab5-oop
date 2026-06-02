#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    logger(new SimulationLogger(this)) {

    ui->setupUi(this);
    ui->callLayout->addStretch(1);

    if (LiftConstants::studentLiftCount > 0)
        addLiftGroup(new LiftGroupFacade(
        LiftConstants::studentLiftCount,
        "для студентов",
        this
        ));
    if (LiftConstants::teacherLiftCount > 0)
        addLiftGroup(new LiftGroupFacade(
        LiftConstants::teacherLiftCount,
        "для преподавателей",
        this
        ));

    ui->callLayout->addStretch(1);
    setMinimumWidth(sizeHint().width());
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::addLiftGroup(LiftGroupFacade* group) {
    ui->callLayout->addWidget(group->callWidget());
    ui->rootLayout->addWidget(group->liftWidget(), group->stretchFactor());
    liftGroups.push_back(group);
    connect(group, &LiftGroupFacade::eventReported, logger, &SimulationLogger::write);
}
