#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , controller(new ElevatorController(this))
{
    ui->setupUi(this);
    setupInterface();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupInterface() {
    shaftLabels = {ui->shaft1Label, ui->shaft2Label, ui->shaft3Label, ui->shaft4Label, ui->shaft5Label};
    floorButtons = {ui->floor1Button, ui->floor2Button, ui->floor3Button, ui->floor4Button, ui->floor5Button};
    cabinButtons = {ui->cabin1Button, ui->cabin2Button, ui->cabin3Button, ui->cabin4Button, ui->cabin5Button};
    updateElevatorPosition(1);
}

void MainWindow::setupConnections() {
    for (size_t i = 0; i < floorButtons.size(); ++i) {
        int floor = static_cast<int>(i + 1);
        connect(floorButtons[i], &QPushButton::clicked, this, [this, floor]() {
            controller->callFromFloor(floor);
        });
    }
    for (size_t i = 0; i < cabinButtons.size(); ++i) {
        int floor = static_cast<int>(i + 1);
        connect(cabinButtons[i], &QPushButton::clicked, this, [this, floor]() {
            controller->selectCabinFloor(floor);
        });
    }
    connect(controller, &ElevatorController::currentFloorChanged, this, [this](int floor) {
        ui->currentFloorValueLabel->setText(QString::number(floor));
        updateElevatorPosition(floor);
    });
    connect(controller, &ElevatorController::targetFloorChanged, ui->targetFloorValueLabel, qOverload<int>(&QLabel::setNum));
    connect(controller, &ElevatorController::controllerStateChanged, ui->controllerStateValueLabel, &QLabel::setText);
    connect(controller, &ElevatorController::cabinStateChanged, ui->cabinStateValueLabel, &QLabel::setText);
    connect(controller, &ElevatorController::doorStateChanged, ui->doorStateValueLabel, &QLabel::setText);
}

void MainWindow::updateElevatorPosition(int floor) {
    int index = floor - 1;
    for (QLabel* label : shaftLabels) {
        label->setText("");
        label->setStyleSheet("");
    }
    if (index >= 0 && index < static_cast<int>(shaftLabels.size())) {
        shaftLabels[index]->setText("ЛИФТ");
        shaftLabels[index]->setStyleSheet("");
    }
}