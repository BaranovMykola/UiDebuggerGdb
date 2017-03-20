#ifndef GDB_H
#define GDB_H

#include <QProcess>
#include <QFile>
#include <QStringList>

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
};

#endif // GDB_H
