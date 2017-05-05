#ifndef GDB_H
#define GDB_H

#include <QProcess>
#include <QFile>
#include <QStringList>

#include <vector>
#include <queue>
#include <list>

#include "breakpoint.h"
#include "variable.h"

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

public slots:
    void readFrame();
    void readType(const QString& varName);
    void readPrint(const QString& context);
public:

    QString getVarContentFromContext(const QString& context);
    void updateVariablesInFrame32x(const QString &frame);

    void readErrOutput();
    const QString& getOutput()const;
    void openProject(const QString& fileName);
    void run();
    void stepOver();
    void setBreakPoint(unsigned int line);
    void clearBreakPoint(unsigned int line);
    void stepIn();
    void stepOut();
    void stopExecuting();
    void stepContinue();
    int getCurrentLine();
    void updateBreakpointsList();
    std::vector<Breakpoint> getBreakpoints()const;
    std::vector<Variable> getLocalVariables()const;
    QString getVarContent(const QString& var);
    QString getVarType(Variable var);
    void updateCertainVariables(QStringList varList);
    QStringList getVariablesFrom(QStringList frame);
    QStringList getVariableList(const QString& frames);

    QStringList getVariableListFromContext(const QString& context);

    void updateVariable64x();

    void globalUpdate();
    void setGdbPath(const QString& path);
public slots:
    void slotReadStdOutput();
    void slotReadErrOutput();
signals:
    void signalLocalVarRecieved(const QString&);
    void signalErrorOccured(const QString&);
    void signalUpdatedVariables();
    void signalTypeUpdated(Variable var);
    void signalContentUpdated(Variable var);
private:
    QFile mGdbFile;
    QString mErrorMessage;
    QString mBuffer;
    std::vector<Breakpoint> mBreakpointsList;
    std::vector<Variable> mVariablesList;


    QString temp;
    bool mInfoCaptured;
    bool mWhatisCaptured;
    QString mWhatisBuffer;
    QString mPrintBuffer;
    bool collect;
    std::list<Variable> mVariableTypeQueue;
    std::list<QString> mPointerContentQueue;
    bool mPrintCaptured;
};

#endif // GDB_H
