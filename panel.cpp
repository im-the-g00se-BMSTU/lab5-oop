#include "panel.h"

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

UiPanel::UiPanel(const QString& title, QWidget* parent)
    : QGroupBox(title, parent),
    dispatcherStateName("IDLE") {
    setProperty("liftPanel", true);
    setupLayout();
    drawCarAtFloor(1);
}

void UiPanel::setupLayout() {
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

void UiPanel::addShaftRows(QGridLayout* layout) {
    for (int floor = Constants::floorCount; floor >= 1; --floor) {
        QLabel* shaftLabel = createShaftLabel();
        int row = Constants::floorCount - floor;
        layout->addWidget(createFloorLabel(QString::number(floor)), row, 0);
        layout->addWidget(shaftLabel, row, 1, 1, Constants::floorCount, Qt::AlignCenter);
        shaftLabels.insert(shaftLabels.begin(), shaftLabel);
    }
}

void UiPanel::addCabinButtons(QHBoxLayout* layout) {
    for (int floor = 1; floor <= Constants::floorCount; ++floor) {
        QPushButton* button = createCabinButton(floor);
        layout->addWidget(button);
        cabinButtons.push_back(button);
        connect(button, &QPushButton::clicked, this, [this, floor]() {
            setCabinButtonActive(floor, true);
            emit cabinFloorRequested(floor);
        });
    }
}

QPushButton* UiPanel::cabinButtonAt(int floor) const {
    QPushButton* button = nullptr;
    int index = floor - 1;
    if (index >= 0 && index < static_cast<int>(cabinButtons.size()))
        button = cabinButtons[index];
    return button;
}

void UiPanel::setCabinButtonActive(int floor, bool isActive) {
    QPushButton* button = cabinButtonAt(floor);
    if (button)
        setButtonActive(button, isActive);
}

void UiPanel::addStatusRows(QGridLayout* layout) {
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

void UiPanel::drawCarAtFloor(int floor) {
    int index = floor - 1;
    for (QLabel* label : shaftLabels) {
        label->setMovie(nullptr);
        label->setText("");
        label->setProperty("activeLift", false);
        label->setProperty("liftState", "");
        label->style()->unpolish(label);
        label->style()->polish(label);
    }
    if (index >= 0 && index < static_cast<int>(shaftLabels.size())) {
        shaftLabels[index]->setText("Лифт");
        shaftLabels[index]->setProperty("activeLift", true);
        shaftLabels[index]->setProperty("liftState", dispatcherStateName);
        shaftLabels[index]->style()->unpolish(shaftLabels[index]);
        shaftLabels[index]->style()->polish(shaftLabels[index]);
    }
    for (const auto& animation : animationMovies) {
        int animationIndex = animation.first - 1;
        QMovie* movie = animation.second;
        if (movie && animationIndex >= 0 && animationIndex < static_cast<int>(shaftLabels.size())) {
            int sideLength = shaftLabels[animationIndex]->height();
            movie->setScaledSize(QSize(sideLength, sideLength));
            shaftLabels[animationIndex]->setText("");
            shaftLabels[animationIndex]->setMovie(movie);
            shaftLabels[animationIndex]->setProperty("activeLift", animation.first == floor);
            shaftLabels[animationIndex]->style()->unpolish(shaftLabels[animationIndex]);
            shaftLabels[animationIndex]->style()->polish(shaftLabels[animationIndex]);
        }
    }
}

void UiPanel::setCurrentFloor(int floor) {
    statusLabels.currentFloor->setText(QString::number(floor));
    drawCarAtFloor(floor);
}

void UiPanel::setTargetFloor(int floor) {
    statusLabels.targetFloor->setText(QString::number(floor));
}

void UiPanel::setDispatcherState(const QString& state) {
    dispatcherStateName = state;
    statusLabels.dispatcherState->setText(state);
    drawCarAtFloor(statusLabels.currentFloor->text().toInt());
}

void UiPanel::setCarState(const QString& state) {
    statusLabels.carState->setText(state);
}

void UiPanel::setDoorState(const QString& state) {
    statusLabels.doorState->setText(state);
}

void UiPanel::clearCabinRequest(int floor) {
    setCabinButtonActive(floor, false);
}

void UiPanel::startAnimation(const QString& resourcePath, int floor) {
    if (Constants::isFloorValid(floor)) {
        stopAnimation(floor);
        QMovie* movie = new QMovie(resourcePath, QByteArray(), this);
        animationMovies[floor] = movie;
        drawCarAtFloor(statusLabels.currentFloor->text().toInt());
        movie->start();
    }
}

void UiPanel::stopAnimation(int floor) {
    auto animation = animationMovies.find(floor);
    if (animation != animationMovies.end()) {
        animation->second->stop();
        delete animation->second;
        animationMovies.erase(animation);
        drawCarAtFloor(statusLabels.currentFloor->text().toInt());
    }
}
