#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "variable.h"

class MainWindow;

namespace Ui {
class DebugWindow;
}
class DebugWindow : public QWidget
{
    friend class MainWindow;
    Q_OBJECT

public:
    explicit DebugWindow(QWidget *parent = 0);
    ~DebugWindow();
    void addTreeRoot(Variable var);
    void addTreeChild(QTreeWidgetItem *parent,
                      QString name, QString description);
    void setMainWindow(MainWindow* mainWindow);
private:
    Ui::DebugWindow *ui;
    MainWindow* mMainWindow;
};

#endif // DEBUGWINDOW_H
