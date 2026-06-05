#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>
#include <QMessageBox>
#include <QString>

class Logger {
public:
    static void write(const QString& message) {
        qDebug().noquote() << message;
    }
};

class MessageBoxReporter {
public:
    static void show(const QString& message) {
        QMessageBox::information(nullptr, "Message", message);
    }
};

#endif // LOGGER_H
