#ifndef GDB_H
#define GDB_H

#include <QProcess>
#include <QFile>

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
public slots:
    void slotReadStdOutput();
    void slotReadErrOutput();
private:
    QFile mGdbFile;
    QString mBuffer;
};

#endif // GDB_H
