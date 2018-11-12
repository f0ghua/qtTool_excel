#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worker.h"
#include "xlsxdocument.h"
#include "xlsxformat.h"
#include "xlsxcellrange.h"
#include "xlsxworksheet.h"

#include <QThread>
#include <QDebug>

QTXLSX_USE_NAMESPACE

// column is from 1
static QString col2Name(int column)
{
    QString colStr;
    int remainder;

    while (column) {
        remainder = column % 26;
        if (remainder == 0)
            remainder = 26;
        colStr.prepend(QChar('A'+remainder-1));
        column = (column - 1) / 26;
    }

    return colStr;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    startWorker();
}

MainWindow::~MainWindow()
{
    stopWorker();
    delete ui;
}

void MainWindow::startWorker()
{
    m_workThread = new QThread();
    m_worker = new Worker();
    m_worker->moveToThread(m_workThread);
    QObject::connect(m_workThread, &QThread::started, m_worker, &Worker::run);
    QObject::connect(m_workThread, &QThread::finished, m_worker, &Worker::deleteLater);
    QObject::connect(m_workThread, &QThread::finished, m_workThread, &QThread::deleteLater);
    //QObject::connect(this, &MainWindow::workStop, m_worker, &Worker::onWorkStop);

    m_workThread->start(QThread::HighPriority);
    qDebug() << "Worker thread started.";
}

void MainWindow::stopWorker()
{
    //emit workStop();

    if(m_workThread && (!m_workThread->isFinished())) {
        m_workThread->quit();
        if(!m_workThread->wait()) {
            qDebug() << "can't stop thread";
        }
        qDebug() << "Worker thread finished.";
    }
}

#define ACT_VLDT_START  "VLDT_START"
#define ACT_VLDT_STOP   "VLDT_STOP"

// 0-150 off
// (45 on, 15 off)*9
// 30 off
void MainWindow::on_pbGenXls1_clicked()
{
    Document xlsx;
    xlsx.addSheet("Steps");
    Format lAlign;
    lAlign.setHorizontalAlignment(Format::AlignLeft);

    int row = 1, col = 1;
    QString cellName;

    // append header
    col = 1;
    cellName = col2Name(col++) + QString::number(row);
    xlsx.write(cellName, "Time(s)", lAlign);
    cellName = col2Name(col++) + QString::number(row);
    xlsx.write(cellName, "Action", lAlign);

    const quint64 phase1OffDuration = 150 * 60;
    const quint64 phase2OnDuration  = 45 * 60;
    const quint64 phase2OffDuration = 15 * 60;
    const quint64 phase3OffDuration = 30 * 60;

    quint64 startSecond = 0;
    quint64 secondPoint = 0;

    for (int i = 0; i < 40; i++) {
        secondPoint += phase1OffDuration - phase2OffDuration;
        for (int j = 0; j < 9; j++) {
            row++;
            secondPoint += phase2OffDuration;
            col = 1;
            cellName = col2Name(col++) + QString::number(row);
            xlsx.write(cellName, secondPoint, lAlign);
            cellName = col2Name(col++) + QString::number(row);
            xlsx.write(cellName, ACT_VLDT_START, lAlign);

            row++;
            secondPoint += phase2OnDuration;
            col = 1;
            cellName = col2Name(col++) + QString::number(row);
            xlsx.write(cellName, secondPoint, lAlign);
            cellName = col2Name(col++) + QString::number(row);
            xlsx.write(cellName, ACT_VLDT_STOP, lAlign);
        }

        secondPoint += phase2OffDuration + phase3OffDuration;
    }

    xlsx.saveAs("scripts.xlsx");
}
