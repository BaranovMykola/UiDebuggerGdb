#include "gdb.h"

#include <QDebug>
#include <iostream>

Gdb::Gdb()
{

}

Gdb::Gdb(QString gdbPath):
    mCaptureLocalVar(false)
{
    mGdbFile.setFileName(gdbPath);
    connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReadStdOutput()), Qt::UniqueConnection);
    connect(this, SIGNAL(readyReadStandardError()), this, SLOT(slotReadErrOutput()), Qt::UniqueConnection);
}

void Gdb::start(const QStringList &arguments, QIODevice::OpenMode mode)
{
    QProcess::start(mGdbFile.fileName(), arguments, mode);
}

void Gdb::write(QByteArray &command)
{
    QByteArray enter("\n");
    command.append(enter);
    QProcess::write(command);
}

void Gdb::readStdOutput()
{
    mBuffer = QProcess::readAll();
    if(mCaptureLocalVar)
    {
        mLocalVar = mBuffer;
        mCaptureLocalVar = false;
        emit signalLocalVarRecieved(mLocalVar);
    }
}

void Gdb::readErrOutput()
{
    mBuffer = QProcess::readAllStandardError();

}

const QString &Gdb::getOutput() const
{
    return mBuffer;
//    return mLocalVar;
}

QStringList Gdb::getLocalVar()
{
    if(QProcess::state() == QProcess::Running)
    {
//        disconnect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReadStdOutput()));
//        connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReadLocalVar()), Qt::UniqueConnection);
        QProcess::write("info local\n");
    //    QProcess::waitForBytesWritten(500);
        mCaptureLocalVar = true;
    }
    return QStringList() << mBuffer;
}

const QString &Gdb::peekLocalVar() const
{
    return mLocalVar;
}

void Gdb::slotReadStdOutput()
{
    readStdOutput();
}

void Gdb::slotReadErrOutput()
{
    readErrOutput();
}

void Gdb::slotReadLocalVar()
{
    mLocalVar = QProcess::readAll();
}
