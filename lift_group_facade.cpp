#include "lift_group_facade.h"

namespace {
constexpr int liftGridRowCount = 2;

QLabel* createFloorLabel(int floor) {
    QLabel* label = new QLabel("Этаж " + QString::number(floor));
    label->setProperty("callFloorLabel", true);
    label->setAlignment(Qt::AlignCenter);
    return label;
}

QPushButton* createCallButton(int floor) {
    QPushButton* button = new QPushButton(QString::number(floor));
    button->setProperty("hallCallButton", true);
    return button;
}

void setButtonActive(QPushButton* button, bool isActive) {
    button->setProperty("activeRequest", isActive);
    button->style()->unpolish(button);
    button->style()->polish(button);
}
}

LiftGroupFacade::LiftGroupFacade(int liftCount, const QString& audience, QWidget* parent)
    : QObject(parent),
    manager(new LiftManager(liftCount, "Лифт " + audience, this)),
    liftGroupBox(new QGroupBox("Лифты " + audience, parent)),
    callGroupBox(new QGroupBox("Вызов " + audience, parent)),
    liftGridLayout(nullptr),
    audienceName(audience) {
    callGroupBox->setProperty("callGroup", true);
    callGroupBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    setupCallLayout();
    setupLiftLayout();
    createLiftPanels();
    connectHallButtons();
    connectManagerSignals();
    connect(manager, &LiftManager::eventReported, this, &LiftGroupFacade::eventReported);
}

void LiftGroupFacade::setupCallLayout() {
    QFormLayout* callLayout = new QFormLayout(callGroupBox);
    callLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    createHallButtons(callLayout);
}

void LiftGroupFacade::setupLiftLayout() {
    liftGridLayout = new QGridLayout(liftGroupBox);
}

void LiftGroupFacade::createHallButtons(QFormLayout* callLayout) {
    for (int floor = LiftConstants::floorCount; floor >= 1; --floor) {
        QPushButton* button = createCallButton(floor);
        callLayout->addRow(createFloorLabel(floor), button);
        hallButtons.insert(hallButtons.begin(), button);
    }
}

void LiftGroupFacade::createLiftPanels() {
    for (int index = 0; index < manager->liftCount(); ++index) {
        LiftPanel* panel = new LiftPanel("Лифт " + QString::number(index + 1), liftGroupBox);
        liftGridLayout->addWidget(panel, liftGridRow(index), liftGridColumn(index), Qt::AlignCenter);
        liftPanels.push_back(panel);
        connectLiftPanel(index);
    }
}

void LiftGroupFacade::connectHallButtons() {
    int floor = 1;
    for (QPushButton* button : hallButtons) {
        connect(button, &QPushButton::clicked, this, [this, floor]() {
            manager->requestAvailableLift(floor);
        });
        ++floor;
    }
}

void LiftGroupFacade::connectManagerSignals() {
    connect(manager, &LiftManager::hallRequestAssigned, this, [this](int, int floor) {
        setHallButtonActive(floor, true);
    });
}

void LiftGroupFacade::connectLiftPanel(int liftIndex) {
    LiftDispatcher* dispatcher = manager->dispatcherAt(liftIndex);
    LiftPanel* panel = liftPanels[liftIndex];
    connect(panel, &LiftPanel::cabinFloorRequested, this, [this, liftIndex](int floor) {
        manager->requestSpecificLift(liftIndex, floor);
    });
    connect(dispatcher, &LiftDispatcher::currentFloorChanged, panel, &LiftPanel::setCurrentFloor);
    connect(dispatcher, &LiftDispatcher::targetFloorChanged, panel, &LiftPanel::setTargetFloor);
    connect(dispatcher, &LiftDispatcher::dispatcherStateChanged, panel, &LiftPanel::setDispatcherState);
    connect(dispatcher, &LiftDispatcher::carStateChanged, panel, &LiftPanel::setCarState);
    connect(dispatcher, &LiftDispatcher::doorStateChanged, panel, &LiftPanel::setDoorState);
    connect(dispatcher, &LiftDispatcher::requestServed, this, &LiftGroupFacade::clearHallRequest);
    connect(dispatcher, &LiftDispatcher::requestServed, panel, &LiftPanel::clearCabinRequest);
}

void LiftGroupFacade::setHallButtonActive(int floor, bool isActive) {
    int index = floor - 1;
    if (index >= 0 && index < static_cast<int>(hallButtons.size()))
        setButtonActive(hallButtons[index], isActive);
}

void LiftGroupFacade::clearHallRequest(int floor) {
    setHallButtonActive(floor, false);
}

int LiftGroupFacade::liftGridColumnCount() const {
    return (manager->liftCount() + liftGridRowCount - 1) / liftGridRowCount;
}

int LiftGroupFacade::liftGridRow(int liftIndex) const {
    return liftIndex / liftGridColumnCount();
}

int LiftGroupFacade::liftGridColumn(int liftIndex) const {
    return liftIndex % liftGridColumnCount();
}

int LiftGroupFacade::stretchFactor() const {
    return liftGridColumnCount();
}

QWidget* LiftGroupFacade::liftWidget() const {
    return liftGroupBox;
}

QWidget* LiftGroupFacade::callWidget() const {
    return callGroupBox;
}
