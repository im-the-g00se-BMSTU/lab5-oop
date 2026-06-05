#include "mainwindow.h"
#include "./ui_mainwindow.h"

// ======== public ========

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow) {

    ui->setupUi(this);
    ui->callLayout->addStretch(1);

    if (studentLiftCount > 0) {
        DispatcherPool* studentDispatcherPool = new DispatcherPool();
        addLiftGroup(new Facade(
            studentLiftCount,
            "для студентов",
            studentDispatcherPool,
            new StudentLiftStrategy(*studentDispatcherPool),
            this
        ));
    }

    if (teacherLiftCount > 0) {
        DispatcherPool* teacherDispatcherPool = new DispatcherPool();
        addLiftGroup(new Facade(
            teacherLiftCount,
            "для преподавателей",
            teacherDispatcherPool,
            new TeacherLiftStrategy(*teacherDispatcherPool),
            this
        ));
    }

    ui->callLayout->addStretch(1);
    setMinimumWidth(sizeHint().width());
}

MainWindow::~MainWindow() {
    delete ui;
}

// ======== private ========

void MainWindow::addLiftGroup(Facade* group) {
    ui->callLayout->addWidget(group->callWidget());
    ui->rootLayout->addWidget(group->liftWidget(), group->stretchFactor());
    liftGroups.push_back(group);
}
