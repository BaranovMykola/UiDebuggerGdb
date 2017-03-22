#ifndef GDB_H
#define GDB_H

#include <QProcess>
#include <QFile>
#include <QStringList>

#include <vector>

#include "breakpoint.h"

class Gdb : public QProcess
{
    Q_OBJECT
public:
    Gdb();
    Gdb(QString gdbPath);
    void start(const QStringList &arguments = QStringList() << "--interpreter=mi",
                QProcess::OpenMode mode = QIODevice::ReadWrite);
    void write(QByteArray &command);
    void readStdOutput();
    void readErrOutput();
    const QString& getOutput()const;
    QStringList getLocalVar();
    const QString& peekLocalVar()const;
    void openProject(const QString& fileName);
    void run();
    void stepOver();
    void setBreakPoint(unsigned int line);
    void clearBreakPoint(unsigned int line);
    void stepIn();
    void stepOut();
    int getCurrentLine();
    void updateBreakpointsList();
    std::vector<Breakpoint> getBreakpoints()const;
    QString getVarContent(const QString& var);
public slots:
    void slotReadStdOutput();
    void slotReadErrOutput();
    void slotReadLocalVar();
signals:
    void signalLocalVarRecieved(const QString&);
private:
    QFile mGdbFile;
    QString mBuffer;
    QString mLocalVar;
    bool mCaptureLocalVar;
    int mCaptureLocalVarSeveralTimes;
    std::vector<Breakpoint> mBreakpointsList;
};

#endif // GDB_H
