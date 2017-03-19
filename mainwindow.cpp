#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QProcess>
#include <QDebug>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mProcess{new Gdb("debug/gdb/gdb.exe")}
{
    ui->setupUi(this);

    connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReadOutput()), Qt::UniqueConnection);
    connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(slotReadOutput()), Qt::UniqueConnection);
    QFile file(qApp->applicationDirPath().append("/gdb/gdb.exe"));
    qDebug() << "File exist: " << (file.exists());
    mProcess->start(QStringList() << "--interpreter=mi");

    connect(ui->command, SIGNAL(returnPressed()), this, SLOT(slotWriteToProcess()), Qt::UniqueConnection);
    ui->command->setText("target exec debug/gdb/compl.exe");
    ui->command->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// target exec D:\Studying\Programming\Qt\My Project\build-UiDebuggerGdb-Custom_Kit-Debug\debug\gdb\gdb.exe

void MainWindow::slotReadOutput()
{
    QString processDataString(mProcess->getOutput());
    ui->echo->appendPlainText(processDataString);
}

void MainWindow::slotWriteToProcess()
{
    QByteArray data;
//    data.append(ui->command->text());
//    data.append('\n');
    QByteArray enter("\n");
    data.append(ui->command->text());
    //data.append(enter);
    mProcess->write(data);

    ui->command->clear();
}
