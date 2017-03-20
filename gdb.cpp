#include "gdb.h"

#include <QDebug>
#include <iostream>

Gdb::Gdb()
{

}

Gdb::Gdb(QString gdbPath):
    mCaptureLocalVar(false),
    mCaptureLocalVarSeveralTimes{0}
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
    if(mCaptureLocalVarSeveralTimes != 0)
    {
        --mCaptureLocalVarSeveralTimes;
        mLocalVar.append( mBuffer);
        mCaptureLocalVar = false;
        if(mCaptureLocalVarSeveralTimes == 0)
        {
            emit signalLocalVarRecieved(mBuffer);
        }
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
        mCaptureLocalVar = true;
        mLocalVar.clear();
        mCaptureLocalVarSeveralTimes = 2;
        QProcess::write("info local\n");
    }
    return QStringList() << mBuffer;
}

const QString &Gdb::peekLocalVar() const
{
    return mLocalVar;
}

void Gdb::openProject(const QString &fileName)
{
    write(QByteArray("target exec ").append(fileName));
    write(QByteArray("file ").append(fileName));
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
