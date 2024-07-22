#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "worker.h"
#include "fm_demod.h"
#include "qaudio.h"
#include "qcombobox.h"
#include "qlayoutitem.h"
#include "qmainwindow.h"
#include "qpushbutton.h"
#include "qspinbox.h"
#include "qtimer.h"
#include "qthread.h"
#include "qtmetamacros.h"
#include "rtlsdr.h"
#include "visualizer.h"

#include <complex>
#include <iostream>
#include <vector>

#ifndef RTLSDR_H
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , rawDataProvider(new RtlSdr())
    , fm (new FM_Demod())
{
    ui->setupUi(this);

    // ui->comboBox->addItem("UHD");
    ui->comboBox->addItem("File Source");

    ui->frame->setLayout(rawDataProvider->view());

    connect(ui->comboBox, &QComboBox::currentIndexChanged, this, [=, this](int newIndex)
        {
            std::cout << "newIndex: " << newIndex << std::endl;
            this->rawDataProvider->close();
            if(newIndex == 0) rawDataProvider = nullptr;
            if(newIndex == 1) rawDataProvider = new FileReader();

            source_mode = newIndex==0 ? SourceMode::RTLSDR : SourceMode::File;
            running=false;
            QLayout* layout = ui->frame->layout();

            // completely delete layout and sublayouts
            QLayoutItem * item = nullptr;
            QLayout * sublayout = nullptr;
            QWidget * widget = nullptr;
            while ((item = layout->takeAt(0))) {
                if ((sublayout = item->layout()) != 0) {/* do the same for sublayout*/ delete sublayout;} //TODO: memleak
                else if ((widget = item->widget()) != 0) {widget->hide(); delete widget;}
                else {delete item;}
            }
            // then finally
            delete layout;

            ui->frame->setLayout(rawDataProvider->view());
            rawDataProvider->open();
            next_data.clear();
            fm->setFs(rawDataProvider->getSampleRate());
            next_data = fm->aa(rawDataProvider->readData());
        });

    emit ui->comboBox->currentIndexChanged(0);

    ui->graphicsView->setViewport(vis1->view());
    ui->graphicsView_2->setViewport(vis2->view());
    ui->graphicsView_3->setViewport(vis3->view());
    ui->graphicsView_4->setViewport(spec->view());

    //------------------------------------
    byteBuffer2->resize(sizeof(float) * 20'000);

    // connect(controller, &Controller::operate, controller->worker, &Worker::doWork);
    connect(controller, &Controller::operate, controller->worker, [=](const QString &s){
        int b_no = s.toInt();

        next_data=buffer_data;

        if(b_no==1) buffer_data = fm->aa(read_data);
        if(b_no==2) buffer_data = fm->aa(read_data2);

        std::cout << "ready to prepare" << std::endl;
        prepare(b_no);
        std::cout << "bytebuff ready" << std::endl;
    });

    connect(audio_sink.get(), &AudioSink::needData, [=]{emit reader->operate("signal forward");});

    connect(reader.get(), &Controller::operate, reader->worker, [=](const QString &s){
        std::cout << "reader:" << s.toStdString() << std::endl;

        while(running && source_mode==SourceMode::RTLSDR){
            read_data = rawDataProvider->readData();
            std::cout << "read to read_data" << std::endl;
            emit controller->operate("1");
            read_data2 = rawDataProvider->readData();
            std::cout << "read to read_data2" << std::endl;
            emit controller->operate("2");
        }

        if(source_mode==SourceMode::File) {
            if(buff_no==1) {
            read_data = rawDataProvider->readData();
            std::cout << "read to read_data" << std::endl;
            emit controller->operate("1");
            buff_no=2;
            }
            else{
            // QThread::msleep(100);
            read_data2 = rawDataProvider->readData();
            std::cout << "read to read_data2" << std::endl;
            emit controller->operate("2");
            buff_no=1;
            }
        }
    });

    connect(vis_timer.get(), &QTimer::timeout, this, [=, this]{
        // TODO: Bunlar da ayrı thread'e, olmuyomuş::(
        vis1->update(next_data);
        vis2->update(next_data);
        vis3->update(read_data, cur_freq);
        vis_timer->start(1900);
    });

}

#else
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , rawDataProvider(new RtlSdr())
    , fm (new FM_Demod())
{
    ui->setupUi(this);

    ui->comboBox->addItem("RTL-SDR");
    // ui->comboBox->setItemData(0, &rtlsdr);
    ui->comboBox->addItem("File Source");

    ui->frame->setLayout(rawDataProvider->view());

    connect(ui->comboBox, &QComboBox::currentIndexChanged, this, [=, this](int newIndex)
        {
            std::cout << "newIndex: " << newIndex << std::endl;
            this->rawDataProvider->close();
            if(newIndex == 0) rawDataProvider = new RtlSdr();
            if(newIndex == 1) rawDataProvider = new FileReader();

            source_mode = newIndex==0 ? SourceMode::RTLSDR : SourceMode::File;
            running=false;
            QLayout* layout = ui->frame->layout();

            // completely delete layout and sublayouts
            QLayoutItem * item = nullptr;
            QLayout * sublayout = nullptr;
            QWidget * widget = nullptr;
            while ((item = layout->takeAt(0))) {
                if ((sublayout = item->layout()) != 0) {/* do the same for sublayout*/ delete sublayout;} //TODO: memleak
                else if ((widget = item->widget()) != 0) {widget->hide(); delete widget;}
                else {delete item;}
            }
            // then finally
            delete layout;

            ui->frame->setLayout(rawDataProvider->view());
            rawDataProvider->open();
            next_data.clear();
            fm->setFs(rawDataProvider->getSampleRate());
            next_data = fm->aa(rawDataProvider->readData());
        });

    emit ui->comboBox->currentIndexChanged(0);

    ui->graphicsView->setViewport(vis1->view());
    ui->graphicsView_2->setViewport(vis2->view());
    ui->graphicsView_3->setViewport(vis3->view());
    ui->graphicsView_4->setViewport(spec->view());

    //------------------------------------
    byteBuffer2->resize(sizeof(float) * 20'000);

    // connect(controller, &Controller::operate, controller->worker, &Worker::doWork);
    connect(controller, &Controller::operate, controller->worker, [=](const QString &s){
        int b_no = s.toInt();

        next_data=buffer_data;

        if(b_no==1) buffer_data = fm->aa(read_data);
        if(b_no==2) buffer_data = fm->aa(read_data2);

        std::cout << "ready to prepare" << std::endl;
        prepare(b_no);
        std::cout << "bytebuff ready" << std::endl;
    });

    connect(audio_sink.get(), &AudioSink::needData, [=]{emit reader->operate("signal forward");});

    connect(reader.get(), &Controller::operate, reader->worker, [=](const QString &s){
        std::cout << "reader:" << s.toStdString() << std::endl;

        while(running && source_mode==SourceMode::RTLSDR){
            read_data = rawDataProvider->readData();
            std::cout << "read to read_data" << std::endl;
            emit controller->operate("1");
            read_data2 = rawDataProvider->readData();
            std::cout << "read to read_data2" << std::endl;
            emit controller->operate("2");
        }

        if(source_mode==SourceMode::File) {
            if(buff_no==1) {
            read_data = rawDataProvider->readData();
            std::cout << "read to read_data" << std::endl;
            emit controller->operate("1");
            buff_no=2;
            }
            else{
            // QThread::msleep(100);
            read_data2 = rawDataProvider->readData();
            std::cout << "read to read_data2" << std::endl;
            emit controller->operate("2");
            buff_no=1;
            }
        }
    });

    QPushButton::connect(ui->frame->findChildren<QPushButton *>()[1], &QPushButton::clicked,this, [=, this]{
        bool is_music = FreqSink::isMusicc(next_data);
        std::cout << ">>>>>>>>>>>> test " << is_music << std::endl;
        if(!running) return;

        auto *temp = ui->frame->findChildren<QDoubleSpinBox *>()[0];
        cur_freq = temp->value();

        temp->setValue(cur_freq+0.2);
        emit temp->valueChanged(cur_freq+0.2);
        QThread::msleep(550);

        is_music = FreqSink::isMusicc(next_data);

        while(!is_music) {
            if(cur_freq+0.2>107.9) break;
            temp->setValue(cur_freq+0.2);
            emit temp->valueChanged(cur_freq+0.2);
            QThread::msleep(550);
            is_music = FreqSink::isMusicc(next_data);
            cur_freq = temp->value();
        }
    });

    QPushButton::connect(ui->frame->findChildren<QPushButton *>()[0], &QPushButton::clicked,this, [=, this]{
        bool is_music = FreqSink::isMusicc(next_data);
        std::cout << "<<<<<<<<<<<< test" << is_music <<  std::endl;
        if(!running) return;

        auto *temp = ui->frame->findChildren<QDoubleSpinBox *>()[0];
        cur_freq = temp->value();

        temp->setValue(cur_freq-0.2);
        emit temp->valueChanged(cur_freq-0.2);
        QThread::msleep(550);

        is_music = FreqSink::isMusicc(next_data);

        while(!is_music) {
            if(cur_freq-0.2<88.1) break;
            temp->setValue(cur_freq-0.2);
            emit temp->valueChanged(cur_freq-0.2);
            QThread::msleep(550);
            is_music = FreqSink::isMusicc(next_data);
            cur_freq = temp->value();
        }
    });

    connect(vis_timer.get(), &QTimer::timeout, this, [=, this]{
        // TODO: Bunlar da ayrı thread'e, olmuyomuş::(
        vis1->update(next_data);
        vis2->update(next_data);
        vis3->update(read_data, cur_freq);
        vis_timer->start(1900);
    });

}
#endif

MainWindow::~MainWindow()
{
    rawDataProvider->close();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if(running){
        running=false;
        audio_sink->setRunning(false);
        return;
    }

    buff_no=1;
    emit reader->operate("signal emmitted");
    running=true;
    audio_sink->setRunning(true);
    audio_sink->play();
    vis_timer->start(3000);
}


void MainWindow::prepare(int section_no=1){

    // read_data = rawDataProvider->readData();
    // next_data = fm->aa(read_data);

    for (int i = 0; i < fm->data_N; i++)
    {
        // qreal sinVal = (qreal)qSin(2.0 * M_PI * 2000 * i / sampleRate);  // create sine wave data samples, one at a time

        // break down one float into four bytes
        // float sample = (float)sinVal;  // save one data sample in a local variable, so I can break it down into four bytes
        float sample = next_data[i];
        char *ptr = (char*)(&sample);  // assign a char* pointer to the address of this data sample
        char byte00 = *ptr;         // 1st byte
        char byte01 = *(ptr + 1);   // 2nd byte
        char byte02 = *(ptr + 2);   // 3rd byte
        char byte03 = *(ptr + 3);   // 4th byte

        // put byte data into QByteArray, one byte at a time
        (*byteBuffer2)[4 * i] = byte00;       // put 1st byte into QByteArray
        (*byteBuffer2)[4 * i + 1] = byte01;   // put 2nd byte into QByteArray
        (*byteBuffer2)[4 * i + 2] = byte02;   // put 3rd byte into QByteArray
        (*byteBuffer2)[4 * i + 3] = byte03;   // put 4th byte into QByteArray
    }

    // std::cout << input->size() << " | " << input->pos() << "|" << input->bytesAvailable() << std::endl;

    // audio_sink->byteBuffer->replace(80'000*(section_no-1), 80'000, *byteBuffer2);
    audio_sink->byteBuffer->push_back(*byteBuffer2);
    // if(section_no==2) audio_sink->input->seek(0);

    // std::cout << byteBuffer->size() << " | " << byteBuffer2->size() << "|" << (s-1) * 80'000 << std::endl;
}

void MainWindow::handleStateChanged(QAudio::State newState)
{
    audio_sink->handleStateChanged(newState);
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->pushButton->setDisabled(true);

    std::vector<std::complex<float>> spect_data;
    rawDataProvider->spectrum(spect_data);

    spec->prepare(spect_data);

    ui->pushButton->setDisabled(false);
}
