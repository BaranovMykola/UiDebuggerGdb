#include "gdb.h"

#include <QDebug>
#include <iostream>
#include <QRegExp>

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
    //qDebug() << "readStdOutpur " << mBuffer << "\n\n";
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
{   //opens file $fileName$ in gdb to debug it via target exec and file
    write(QByteArray("target exec ").append(fileName));
    write(QByteArray("file ").append(fileName));
}

void Gdb::run()
{   //run debugging
    write(QByteArray("run"));
}

void Gdb::stepOver()
{   //goes to the next line of code
    write(QByteArray("next"));
}

void Gdb::setBreakPoint(unsigned int line)
{   //set simple breakpoint at line $line$
    write(QByteArray("b ").append(QString::number(line)));
}

void Gdb::clearBreakPoint(unsigned int line)
{
    write(QByteArray("clear ").append(QString::number(line)));
}

void Gdb::stepIn()
{   //step into function under cursor
    write(QByteArray("step"));
}

void Gdb::stepOut()
{   //step out of current function\method
    write(QByteArray("finish"));
}

int Gdb::getCurrentLine()
{   //returns current line of code or -1 if any aerror occured
    write(QByteArray("frame"));
    QProcess::waitForReadyRead();
    QRegExp rx(":\\d+");
    if(rx.indexIn(mBuffer) == -1)
    {
        return -1;
    }
    QStringList lst = rx.capturedTexts();
    QString line = lst[0];
    return line.split(':').last().toInt();
}

void Gdb::updateBreakpointsList()
{
    write(QByteArray("info b"));
    QProcess::waitForReadyRead(1000);
    QStringList lines = mBuffer.split('~');
    for(auto i : lines)
    {
        qDebug() << i << "\n";
    }
    mBreakpointsList.clear();

    QString topic = lines[0];
    for(int i=2;i<lines.size();++i)
    {
        Breakpoint addBrk;
        QString currentLine = lines[i];
        try
        {
            addBrk.parse(currentLine);
            mBreakpointsList.push_back(addBrk);
        }
        catch(std::exception)
        {
        }
    }
}

std::vector<Breakpoint> Gdb::getBreakpoints() const
{
    return mBreakpointsList;
}

QString Gdb::getVarContent(const QString& var)
{
    write(QByteArray("print ").append(var));
    QProcess::waitForReadyRead(1000);
    QRegExp errorMatch("\\^done");
    if(errorMatch.indexIn(mBuffer) == -1)
    {
        throw std::exception("Error while var reading");
    }
    QRegExp content("=\\s.*\\^done");
    QRegExp clean("[\\\\|\|\"|~]");
//    QRegExp clean("\\\\n");
    qDebug() << ((content.indexIn(mBuffer) == -1) ? "Nothing captured" : "Captured smth succsesfully");
    QString res = content.cap().replace(clean, "").replace("^done", "").trimmed();
    res.resize(res.size()-1);
    auto lst = res.split('\n');
    for(QString& i : lst)
    {
        i = i.trimmed();
    }
    QString withoutLines = lst.join("");
    return withoutLines;

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
