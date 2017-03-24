#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gdb.h"
#include "debugwindow.h"

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
    void addTreeRoot(Variable var);
    void addTreeChild(QTreeWidgetItem *parent,
                      Variable var, QString prefix);
    void addTreeChildren(QTreeWidgetItem* parrent,
                      Variable var, QString prefix);
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
    void slotReadPointer();
private:
    Ui::MainWindow *ui;
    Gdb *mProcess;
    DebugWindow mDebugWindow;
};

#endif // MAINWINDOW_H
