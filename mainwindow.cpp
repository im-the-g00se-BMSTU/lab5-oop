#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    logger(new Logger(this)) {

    ui->setupUi(this);
    ui->callLayout->addStretch(1);

    if (Constants::studentLiftCount > 0)
        addLiftGroup(new Facade(
        Constants::studentLiftCount,
        "для студентов",
        new StudentLiftStrategy(),
        this
        ));
    if (Constants::teacherLiftCount > 0)
        addLiftGroup(new Facade(
        Constants::teacherLiftCount,
        "для преподавателей",
        new TeacherLiftStrategy(),
        this
        ));

    ui->callLayout->addStretch(1);
    setMinimumWidth(sizeHint().width());
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::addLiftGroup(Facade* group) {
    ui->callLayout->addWidget(group->callWidget());
    ui->rootLayout->addWidget(group->liftWidget(), group->stretchFactor());
    liftGroups.push_back(group);
    connect(group, &Facade::eventReported, logger, &Logger::write);
}
