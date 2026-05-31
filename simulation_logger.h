#ifndef SIMULATION_LOGGER_H
#define SIMULATION_LOGGER_H

#include <QDebug>
#include <QObject>

class SimulationLogger : public QObject {
    Q_OBJECT

public:
    explicit SimulationLogger(QObject* parent = nullptr)
        : QObject(parent) {}

public slots:
    void write(QString message) const {
        qDebug().noquote() << message;
    }
};

#endif // SIMULATION_LOGGER_H
