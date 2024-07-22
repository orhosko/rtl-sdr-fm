#ifndef WORKER_H
#define WORKER_H

#include "qobject.h"
#include "qthread.h"
#include "qtmetamacros.h"
#include <iostream>

class Worker : public QObject
{
    Q_OBJECT

public slots:
    void doWork(const QString &parameter) {
        QString result = parameter;
        /* ... here is the expensive or blocking operation ... */
        QThread::sleep(parameter.toInt());
        emit resultReady(result);
    }

    // template<typename Function, typename... Arguments>
    // auto doWork(Function func, Arguments... args) {
    //     return [=](auto... rest) {
    //         return func(args..., rest...);
    //     };
    // }

    // template<typename Function>
    // auto doWork(Function func) {
    //     return [=](auto... rest) {
    //         return func(rest...);
    //     };
    // }

signals:
    void resultReady(const QString &result);
};

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Worker *worker = new Worker;
    Controller() {
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        // connect(this, &Controller::operate, worker, &Worker::doWork);
        connect(worker, &Worker::resultReady, this, &Controller::handleResults);
        workerThread.start();
    }
    ~Controller() {
        workerThread.quit();
        workerThread.wait();
    }
    bool running = 1;
public slots:
    void handleResults(const QString &res){
        std::cout << "------------------------" << res.toStdString() << std::endl;
    };
signals:
    void operate(const QString &);
};

#endif
