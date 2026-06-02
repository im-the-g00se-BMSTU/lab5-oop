#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    dispatcher(new LiftDispatcher(this)),
    logger(new SimulationLogger(this)) {
    ui->setupUi(this);
    setupInterface();
    setupConnections();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupInterface() {
    shaftLabels = {ui->shaft1Label, ui->shaft2Label, ui->shaft3Label, ui->shaft4Label, ui->shaft5Label};
    floorButtons = {ui->floor1Button, ui->floor2Button, ui->floor3Button, ui->floor4Button, ui->floor5Button};
    cabinButtons = {ui->cabin1Button, ui->cabin2Button, ui->cabin3Button, ui->cabin4Button, ui->cabin5Button};
    drawCarAtFloor(LiftConstants::firstFloor);
}

void MainWindow::setupConnections() {
    connectHallButtons();
    connectCabinButtons();
    connectStateLabels();
    connect(dispatcher, &LiftDispatcher::eventReported, logger, &SimulationLogger::write);
}

void MainWindow::connectHallButtons() {
    int floor = 1;
    for (QPushButton* button : floorButtons) {
        connect(button, &QPushButton::clicked, this, [this, floor]() {
            dispatcher->requestFromHall(floor);
        });
        ++floor;
    }
}

void MainWindow::connectCabinButtons() {
    int floor = 1;
    for (QPushButton* button : cabinButtons) {
        connect(button, &QPushButton::clicked, this, [this, floor]() {
            dispatcher->requestFromCabin(floor);
        });
        ++floor;
    }
}

void MainWindow::connectStateLabels() {
    connect(dispatcher, &LiftDispatcher::currentFloorChanged, this, [this](int floor) {
        ui->currentFloorValueLabel->setText(QString::number(floor));
        drawCarAtFloor(floor);
    });
    connect(dispatcher, &LiftDispatcher::targetFloorChanged, ui->targetFloorValueLabel, qOverload<int>(&QLabel::setNum));
    connect(dispatcher, &LiftDispatcher::dispatcherStateChanged, ui->controllerStateValueLabel, &QLabel::setText);
    connect(dispatcher, &LiftDispatcher::carStateChanged, ui->cabinStateValueLabel, &QLabel::setText);
    connect(dispatcher, &LiftDispatcher::doorStateChanged, ui->doorStateValueLabel, &QLabel::setText);
}

void MainWindow::drawCarAtFloor(int floor) {
    int index = floor - 1;
    for (QLabel* label : shaftLabels) {
        label->setText("");
        label->setProperty("activeLift", false);
        label->style()->unpolish(label);
        label->style()->polish(label);
    }
    if (index >= 0 && index < static_cast<int>(shaftLabels.size())) {
        shaftLabels[index]->setText("LIFT");
        shaftLabels[index]->setProperty("activeLift", true);
        shaftLabels[index]->style()->unpolish(shaftLabels[index]);
        shaftLabels[index]->style()->polish(shaftLabels[index]);
    }
}
