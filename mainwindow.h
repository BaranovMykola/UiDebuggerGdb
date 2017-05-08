#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include "gdb.h"

namespace Ui {
class MainWindow;
}

class QProcess;

struct VarComp {
    bool operator()(const Variable& a, const Variable& b) const {
        return a.getName() < b.getName();
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addTreeRoot(Variable var);
    void addTreeChild(QTreeWidgetItem *parent,
                      Variable var, QString prefix, bool internal);
    void addTreeChildren(QTreeWidgetItem* parrent,
                      Variable var, QString prefix, bool drfPointer = false);

    void moidifyTreeItemPointer(QTreeWidgetItem* itemPointer);
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

    void slotShowVariables();
    void slotTypeUpdated(Variable var);
    void slotDereferenceVar(Variable var);
    void slotDereferenceTypeVar(Variable var);
    void slotBreakpointHit(int line);
    void slotErrorOccured(QString error);

    void slotGetVarType();
    void slotReadPointer();
    void slotTestVariable();
    void slotItemExpanded(QTreeWidgetItem* item);
    void slotContinue();
    void slotKill();
    void slotStipExecuting();
private:
    Ui::MainWindow *ui;
    Gdb *mProcess;
    std::list<QTreeWidgetItem*> mPointers;
    std::map<QTreeWidgetItem*, Variable> mPointersName;
    std::map<Variable, QTreeWidgetItem*, VarComp> mTypeVar;
    std::map<Variable, QTreeWidgetItem*, VarComp> mPointersContent;
};

#endif // MAINWINDOW_H
