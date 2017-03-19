#include "gdb.h"

#include <QDebug>
#include <iostream>

Gdb::Gdb()
{

}

Gdb::Gdb(QString gdbPath)
{
    mGdbFile.setFileName(gdbPath);
    connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReadStdOutput()), Qt::UniqueConnection);
    connect(this, SIGNAL(readyReadStandardError()), this, SLOT(slotReadErrOutput()), Qt::UniqueConnection);
}

void Gdb::start(const QStringList &arguments, QIODevice::OpenMode mode)
{
    QProcess::start(mGdbFile.fileName(), arguments, mode);
    QProcess::waitForReadyRead(500);
}

void Gdb::write(QByteArray &command)
{
    //qDebug() << "Write command to process";
    QByteArray enter("\n");
    command.append(enter);
    QProcess::write(command);
    //qDebug() << "Process is waiting for bytes written: " << QProcess::waitForBytesWritten(3000);
    //QProcess::waitForReadyRead(500);
}

void Gdb::readStdOutput()
{
    mBuffer = QProcess::readAll();
}

void Gdb::readErrOutput()
{
    mBuffer = QProcess::readAllStandardError();
}

const QString &Gdb::getOutput() const
{
    return mBuffer;
}

void Gdb::slotReadStdOutput()
{
    readStdOutput();
}

void Gdb::slotReadErrOutput()
{
    readErrOutput();
}
