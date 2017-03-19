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

private:
    Ui::MainWindow *ui;
    Gdb *mProcess;
};

#endif // MAINWINDOW_H
