#include "debugwindow.h"
#include "ui_debugwindow.h"
#include <QTreeWidgetItem>

DebugWindow::DebugWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugWindow)
{
    ui->setupUi(this);
    ui->treeWidget->setColumnCount(2);

    // Add root nodes
//    addTreeRoot("A", "Root_first");
//    addTreeRoot("B", "Root_second");
//    addTreeRoot("C", "Root_third");
}

DebugWindow::~DebugWindow()
{
    delete ui;
}

void DebugWindow::addTreeRoot(Variable var)
{
    // QTreeWidgetItem(QTreeWidget * parent, int type = Type)
        QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);

        // QTreeWidgetItem::setText(int column, const QString & text)
        treeItem->setText(0, var.getName());
        treeItem->setText(1, var.getContent().append(" (%1)").arg(var.getType()));
//        addTreeChild(treeItem, name + "A", "Child_first");
//        addTreeChild(treeItem, name + "B", "Child_second");
}

void DebugWindow::addTreeChild(QTreeWidgetItem *parent, QString name, QString description)
{
    // QTreeWidgetItem(QTreeWidget * parent, int type = Type)
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();

    // QTreeWidgetItem::setText(int column, const QString & text)
    treeItem->setText(0, name);
    treeItem->setText(1, description);

    // QTreeWidgetItem::addChild(QTreeWidgetItem * child)
    parent->addChild(treeItem);
}

void DebugWindow::setMainWindow(MainWindow *mainWindow)
{
    mMainWindow = mainWindow;
}
