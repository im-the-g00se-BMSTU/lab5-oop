#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>
#include <QObject>

class Logger : public QObject {
    Q_OBJECT

public:
    explicit Logger(QObject* parent = nullptr)
        : QObject(parent) {}

public slots:
    void write(QString message) const {
        qDebug().noquote() << message;
    }
};

#endif // LOGGER_H
