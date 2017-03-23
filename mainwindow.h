#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gdb.h"

namespace Ui {
class MainWindow;
}

class QProcess;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slotReadOutput();
    void slotWriteToProcess();
    void slotGetLocalVar();
    void slotReadLocalVar(const QString& str);
    void slotRun();
    void slotStepOver();
    void slotSetBreakPoint();
    void slotClearBreakPoint();
    void slotStepIn();
    void slotStepOut();
    void slotCurrentLine();
    void slotShowBreakpoints();
    void slotShowVar();
    void slotShowLocal();
    void slotUpdtaeLocals();
    void slotGetVarType();
private:
    Ui::MainWindow *ui;
    Gdb *mProcess;
};

#endif // MAINWINDOW_H
