#include "facade.h"

// ======== private ========

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

// ======== public ========

Facade::Facade(
    int liftCount,
    const QString& audience,
    ManagerStrategy* serviceStrategy,
    QWidget* parent
)
    : QObject(parent),
    manager(new Manager(liftCount, "Лифт " + audience, serviceStrategy, this)),
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
    connect(manager, &Manager::eventReported, this, &Facade::eventReported);
    connect(manager, &Manager::messageBoxRequested, this, &Facade::messageBoxRequested);
}

// ======== private ========

void Facade::setupCallLayout() {
    QFormLayout* callLayout = new QFormLayout(callGroupBox);
    callLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    createHallButtons(callLayout);
}

void Facade::setupLiftLayout() {
    liftGridLayout = new QGridLayout(liftGroupBox);
}

void Facade::createHallButtons(QFormLayout* callLayout) {
    for (int floor = Constants::floorCount; floor >= 1; --floor) {
        QPushButton* button = createCallButton(floor);
        callLayout->addRow(createFloorLabel(floor), button);
        hallButtons.insert(hallButtons.begin(), button);
        blockedHallRequests.insert(blockedHallRequests.begin(), false);
    }
}

void Facade::createLiftPanels() {
    int columnCount = liftGridColumnCount();
    for (int index = 0; index < manager->liftCount(); ++index) {
        UiPanel* panel = new UiPanel("Лифт " + QString::number(index + 1), liftGroupBox);
        liftGridLayout->addWidget(panel, index / columnCount, index % columnCount, Qt::AlignCenter);
        liftPanels.push_back(panel);
        connectLiftPanel(index);
    }
}

void Facade::connectHallButtons() {
    int floor = 1;
    for (QPushButton* button : hallButtons) {
        connect(button, &QPushButton::clicked, this, [this, floor]() {
            if (!isHallRequestBlocked(floor))
                manager->requestAvailableLift(floor);
        });
        ++floor;
    }
}

void Facade::connectManagerSignals() {
    connect(manager, &Manager::hallRequestAssigned, this, [this](int, int floor) {
        setHallButtonActive(floor, true);
        setHallRequestBlocked(floor, true);
    });
    connect(manager, &Manager::hallRequestCanceled, this, &Facade::completeHallRequest);
}

void Facade::connectLiftPanel(int liftIndex) {
    Dispatcher* dispatcher = manager->dispatcherAt(liftIndex);
    UiPanel* panel = liftPanels[liftIndex];
    connect(panel, &UiPanel::cabinFloorRequested, this, [this, liftIndex](int floor) {
        manager->requestSpecificLift(liftIndex, floor);
    });
    connect(dispatcher, &Dispatcher::currentFloorChanged, panel, &UiPanel::setCurrentFloor);
    connect(dispatcher, &Dispatcher::targetFloorChanged, panel, &UiPanel::setTargetFloor);
    connect(dispatcher, &Dispatcher::dispatcherStateChanged, panel, &UiPanel::setDispatcherState);
    connect(dispatcher, &Dispatcher::carStateChanged, panel, &UiPanel::setCarState);
    connect(dispatcher, &Dispatcher::doorStateChanged, panel, &UiPanel::setDoorState);
    connect(dispatcher, &Dispatcher::requestServed, this, [this](int floor) {
        setHallButtonActive(floor, false);
    });
    connect(dispatcher, &Dispatcher::requestCompleted, this, &Facade::completeHallRequest);
    connect(dispatcher, &Dispatcher::requestServed, panel, &UiPanel::clearCabinRequest);
    connect(manager, &Manager::cabinRequestCanceled, panel, [panel, liftIndex](int requestLiftIndex, int floor) {
        if (requestLiftIndex == liftIndex)
            panel->clearCabinRequest(floor);
    });
    connectLiftAnimation(liftIndex, panel);
}

void Facade::connectLiftAnimation(int liftIndex, UiPanel* panel) {
    connect(manager, &Manager::liftAnimationStarted, panel, [panel, liftIndex](int animationLiftIndex, int floor, const QString& path) {
        if (animationLiftIndex == liftIndex)
            panel->startAnimation(path, floor);
    });
    connect(manager, &Manager::liftAnimationStopped, panel, [panel, liftIndex](int animationLiftIndex, int floor) {
        if (animationLiftIndex == liftIndex)
            panel->stopAnimation(floor);
    });
}

bool Facade::isHallRequestBlocked(int floor) const {
    bool isBlocked = false;
    int index = floor - 1;
    if (index >= 0 && index < static_cast<int>(blockedHallRequests.size()))
        isBlocked = blockedHallRequests[index];
    return isBlocked;
}

void Facade::setHallButtonActive(int floor, bool isActive) {
    int index = floor - 1;
    if (index >= 0 && index < static_cast<int>(hallButtons.size()))
        setButtonActive(hallButtons[index], isActive);
}

void Facade::setHallRequestBlocked(int floor, bool isBlocked) {
    int index = floor - 1;
    if (index >= 0 && index < static_cast<int>(blockedHallRequests.size()))
        blockedHallRequests[index] = isBlocked;
}

void Facade::completeHallRequest(int floor) {
    setHallButtonActive(floor, false);
    setHallRequestBlocked(floor, false);
}

int Facade::liftGridColumnCount() const {
    return (manager->liftCount() + liftGridRowCount - 1) / liftGridRowCount;
}

// ======== public ========

int Facade::stretchFactor() const {
    return liftGridColumnCount();
}

QWidget* Facade::liftWidget() const {
    return liftGroupBox;
}

QWidget* Facade::callWidget() const {
    return callGroupBox;
}
