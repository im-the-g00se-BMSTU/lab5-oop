#include "lift_panel.h"

namespace {
QLabel* createShaftLabel() {
    QLabel* label = new QLabel;
    label->setProperty("shaftCell", true);
    label->setAlignment(Qt::AlignCenter);
    return label;
}

QLabel* createFloorLabel(const QString& text) {
    QLabel* label = new QLabel(text);
    label->setProperty("liftFloorLabel", true);
    label->setAlignment(Qt::AlignCenter);
    return label;
}

QLabel* createStatusNameLabel(const QString& text) {
    QLabel* label = new QLabel(text);
    label->setProperty("statusName", true);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    return label;
}

QLabel* createStatusValueLabel(const QString& text) {
    QLabel* label = new QLabel(text);
    label->setProperty("statusValue", true);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return label;
}

QPushButton* createCabinButton(int floor) {
    QPushButton* button = new QPushButton(QString::number(floor));
    button->setProperty("cabinButton", true);
    return button;
}

void setButtonActive(QPushButton* button, bool isActive) {
    button->setProperty("activeRequest", isActive);
    button->style()->unpolish(button);
    button->style()->polish(button);
}
}

LiftPanel::LiftPanel(const QString& title, QWidget* parent)
    : QGroupBox(title, parent) {
    setProperty("liftPanel", true);
    setupLayout();
    drawCarAtFloor(1);
}

void LiftPanel::setupLayout() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QGridLayout* shaftLayout = new QGridLayout;
    QHBoxLayout* cabinLayout = new QHBoxLayout;
    QGridLayout* statusLayout = new QGridLayout;

    addShaftRows(shaftLayout);
    addCabinButtons(cabinLayout);
    addStatusRows(statusLayout);

    mainLayout->addLayout(shaftLayout);
    mainLayout->addLayout(cabinLayout);
    mainLayout->addLayout(statusLayout);
    mainLayout->setAlignment(shaftLayout, Qt::AlignHCenter);
    mainLayout->setAlignment(cabinLayout, Qt::AlignHCenter);
}

void LiftPanel::addShaftRows(QGridLayout* layout) {
    for (int floor = LiftConstants::floorCount; floor >= 1; --floor) {
        QLabel* shaftLabel = createShaftLabel();
        int row = LiftConstants::floorCount - floor;
        layout->addWidget(createFloorLabel(QString::number(floor)), row, 0);
        layout->addWidget(shaftLabel, row, 1, 1, LiftConstants::floorCount, Qt::AlignCenter);
        shaftLabels.insert(shaftLabels.begin(), shaftLabel);
    }
}

void LiftPanel::addCabinButtons(QHBoxLayout* layout) {
    for (int floor = 1; floor <= LiftConstants::floorCount; ++floor) {
        QPushButton* button = createCabinButton(floor);
        layout->addWidget(button);
        cabinButtons.push_back(button);
        connect(button, &QPushButton::clicked, this, [this, floor]() {
            setCabinButtonActive(floor, true);
            emit cabinFloorRequested(floor);
        });
    }
}

QPushButton* LiftPanel::cabinButtonAt(int floor) const {
    QPushButton* button = nullptr;
    int index = floor - 1;
    if (index >= 0 && index < static_cast<int>(cabinButtons.size()))
        button = cabinButtons[index];
    return button;
}

void LiftPanel::setCabinButtonActive(int floor, bool isActive) {
    QPushButton* button = cabinButtonAt(floor);
    if (button)
        setButtonActive(button, isActive);
}

void LiftPanel::addStatusRows(QGridLayout* layout) {
    statusLabels = {
        createStatusValueLabel(QString::number(1)),
        createStatusValueLabel(QString::number(1)),
        createStatusValueLabel("IDLE"),
        createStatusValueLabel("PARKED"),
        createStatusValueLabel("CLOSED")
    };
    int row = 0;
    QLabel* floorNameLabel = createStatusNameLabel("Этаж");
    layout->addWidget(floorNameLabel, row, 0);
    layout->addWidget(statusLabels.currentFloor, row, 1);
    layout->addWidget(createStatusNameLabel("Цель"), row + 1, 0);
    layout->addWidget(statusLabels.targetFloor, row + 1, 1);
    layout->addWidget(createStatusNameLabel("Диспетчер"), row + 2, 0);
    layout->addWidget(statusLabels.dispatcherState, row + 2, 1);
    layout->addWidget(createStatusNameLabel("Кабина"), row + 3, 0);
    layout->addWidget(statusLabels.carState, row + 3, 1);
    layout->addWidget(createStatusNameLabel("Двери"), row + 4, 0);
    layout->addWidget(statusLabels.doorState, row + 4, 1);
    layout->setColumnStretch(1, 1);
}

void LiftPanel::drawCarAtFloor(int floor) {
    int index = floor - 1;
    for (QLabel* label : shaftLabels) {
        label->setText("");
        label->setProperty("activeLift", false);
        label->style()->unpolish(label);
        label->style()->polish(label);
    }
    if (index >= 0 && index < static_cast<int>(shaftLabels.size())) {
        shaftLabels[index]->setText("L");
        shaftLabels[index]->setProperty("activeLift", true);
        shaftLabels[index]->style()->unpolish(shaftLabels[index]);
        shaftLabels[index]->style()->polish(shaftLabels[index]);
    }
}

void LiftPanel::setCurrentFloor(int floor) {
    statusLabels.currentFloor->setText(QString::number(floor));
    drawCarAtFloor(floor);
}

void LiftPanel::setTargetFloor(int floor) {
    statusLabels.targetFloor->setText(QString::number(floor));
}

void LiftPanel::setDispatcherState(const QString& state) {
    statusLabels.dispatcherState->setText(state);
}

void LiftPanel::setCarState(const QString& state) {
    statusLabels.carState->setText(state);
}

void LiftPanel::setDoorState(const QString& state) {
    statusLabels.doorState->setText(state);
}

void LiftPanel::clearCabinRequest(int floor) {
    setCabinButtonActive(floor, false);
}
