#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QProcess>
#include <QDebug>
#include <QTextStream>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mProcess{new Gdb("debug/gdb/gdb.exe")},
    mDebugWindow{}
{
    ui->setupUi(this);

    connect(mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReadOutput()), Qt::UniqueConnection);
    connect(mProcess, SIGNAL(readyReadStandardError()), this, SLOT(slotReadOutput()), Qt::UniqueConnection);
    connect(mProcess, SIGNAL(signalLocalVarRecieved(QString)), this, SLOT(slotReadLocalVar(QString)), Qt::UniqueConnection);
    connect(ui->command, SIGNAL(returnPressed()), this, SLOT(slotWriteToProcess()), Qt::UniqueConnection);
    connect(ui->butLocalVar, SIGNAL(clicked(bool)), this, SLOT(slotGetLocalVar()), Qt::UniqueConnection);
    connect(ui->butRun, SIGNAL(clicked(bool)), this, SLOT(slotRun()), Qt::UniqueConnection);
    connect(ui->butStepOver, SIGNAL(clicked(bool)), this, SLOT(slotStepOver()), Qt::UniqueConnection);
    connect(ui->butBreakPoint, SIGNAL(clicked(bool)), this, SLOT(slotSetBreakPoint()), Qt::UniqueConnection);
    connect(ui->butClearBreakpoint, SIGNAL(clicked(bool)), this, SLOT(slotClearBreakPoint()), Qt::UniqueConnection);
    connect(ui->butStepIn, SIGNAL(clicked(bool)), this, SLOT(slotStepIn()), Qt::UniqueConnection);
    connect(ui->butStepOut, SIGNAL(clicked(bool)), this, SLOT(slotStepOut()), Qt::UniqueConnection);
    connect(ui->butCurrLine, SIGNAL(clicked(bool)), this, SLOT(slotCurrentLine()), Qt::UniqueConnection);
    connect(ui->butShwBrk, SIGNAL(clicked(bool)), this, SLOT(slotShowBreakpoints()), Qt::UniqueConnection);
    connect(ui->butVar, SIGNAL(clicked(bool)), this, SLOT(slotShowVar()), Qt::UniqueConnection);
    connect(ui->butLocal, SIGNAL(clicked(bool)), this, SLOT(slotShowLocal()), Qt::UniqueConnection);
    connect(ui->butUpdateLocals, SIGNAL(clicked(bool)), this, SLOT(slotUpdtaeLocals()), Qt::UniqueConnection);
    connect(ui->butGetVarType, SIGNAL(clicked(bool)), this, SLOT(slotGetVarType()), Qt::UniqueConnection);
    connect(ui->butReadPointer, SIGNAL(clicked(bool)), this, SLOT(slotReadPointer()), Qt::UniqueConnection);
    connect(ui->butTestVar, SIGNAL(clicked(bool)), this, SLOT(slotTestVariable()), Qt::UniqueConnection);
    connect(ui->treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(slotItemExpanded(QTreeWidgetItem*)), Qt::UniqueConnection);
    connect(ui->butContinue, SIGNAL(clicked(bool)), this, SLOT(slotContinue()), Qt::UniqueConnection);
    connect(ui->butKill, SIGNAL(clicked(bool)), this, SLOT(slotKill()), Qt::UniqueConnection);
    connect(ui->butStopExecuting, SIGNAL(clicked(bool)), this, SLOT(slotStipExecuting()), Qt::UniqueConnection);

    QFile file(qApp->applicationDirPath().append("/gdb/gdb.exe"));
    qDebug() << "File exist: " << (file.exists());
    mProcess->start(QStringList() << "--interpreter=mi");

//    ui->command->setText("target exec debug/gdb/compl.exe");
    mProcess->openProject("debug/gdb/lst.exe");
    ui->command->setFocus();
    ui->treeWidget->setColumnCount(2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTreeRoot(Variable var)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(ui->treeWidget);

    // QTreeWidgetItem::setText(int column, const QString & text)
    treeItem->setText(0, var.getName());
    QString varContent = var.getContent();
    varContent.append(" (%1)");
    varContent = varContent.arg(var.getType());
    treeItem->setText(1, varContent);
    addTreeChildren(treeItem, var, "");
}

void MainWindow::addTreeChild(QTreeWidgetItem *parent, Variable var, QString prefix, bool internal = false)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem();
    QString plainName = var.getName().split('.').last();
    treeItem->setText(0, plainName);
    treeItem->setText(1, var.getContent().append(" (%1)").arg(var.getType()));
    if(!internal)
    {
        addTreeChildren(treeItem, var, prefix);
    }
    else
    {
        treeItem->setHidden(true);
    }
    parent->addChild(treeItem);
}

void MainWindow::addTreeChildren(QTreeWidgetItem *parrent, Variable var, QString prefix, bool drfPointer)
{
    std::vector<Variable> nestedTypes = var.getNestedTypes();
    if(drfPointer && nestedTypes.size() ==0)
    {
        addTreeChild(parrent, var, "", false);
    }
    if(var.isPointer())
    {
        QString dereferencedVarName = QString("*(%1)").arg(var.getName());
        addTreeChild(parrent, var, "", true);   //create fake node to enable expanding parent
        mPointersName[parrent] = var;   //Add pointer's node to map and attach to this node pointer
    }
    for(auto i : nestedTypes)
    {
        QString likelyType = mProcess->getVarType(i.getName());
        i.setType(likelyType.isEmpty() ? "<No info>" : likelyType);
        addTreeChild(parrent, i, prefix, false);
    }
}

void MainWindow::moidifyTreeItemPointer(QTreeWidgetItem *itemPointer)
{   //remove helper node and attachs content of dereferenced pointer
    Variable pointer = mPointersName[itemPointer];

    QString drfName = tr("(*%1)").arg(pointer.getName());
    QString drfAddressContent = mProcess->getVarContent(drfName);
    QString drfAddressType = mProcess->getVarType(drfName);
    Variable drfPointer(drfName, drfAddressType, drfAddressContent);

    QTreeWidgetItem* child = itemPointer->child(0); //Pointer's node always has ony one shils so it's index is '0'
    itemPointer->removeChild(child);    //remove internal node in tree
    if(drfPointer.getNestedTypes().size() == 0 && !drfPointer.isPointer())
    {
        addTreeChild(itemPointer, drfPointer, "", false);
        qDebug() << "drfPointer has only one nested type && it's not pointer";
        return;
    }
    addTreeChildren(itemPointer, drfPointer, "", true);   //append dereferenced pointer to node with addres
}

// target exec D:\Studying\Programming\Qt\My Project\build-UiDebuggerGdb-Custom_Kit-Debug\debug\gdb\gdb.exe

void MainWindow::slotReadOutput()
{
    QString processDataString(mProcess->getOutput());
    ui->echo->appendPlainText(processDataString);
}

void MainWindow::slotWriteToProcess()
{
    QByteArray data;
//    data.append(ui->command->text());
//    data.append('\n');
    QByteArray enter("\n");
    data.append(ui->command->text());
    //data.append(enter);
    mProcess->write(data);

    ui->command->clear();
}

void MainWindow::slotGetLocalVar()
{
//    mProcess->getLocalVar();

    //ui->echo->appendPlainText(mProcess->peekLocalVar());
}

void MainWindow::slotReadLocalVar(const QString &str)
{
    ui->echo->appendPlainText("\n*********************\t\t\t{\n");
    auto lst = str.split("\\n");
    for(QString i : lst)
    {
        i.replace("\"", "");
        i.replace("~", "");
        ui->echo->appendPlainText(i);
        ui->echo->appendPlainText("\n");
    }
    ui->echo->appendPlainText("\n*********************\t\t\t}\n");
}

void MainWindow::slotRun()
{
    mProcess->run();
}

void MainWindow::slotStepOver()
{
    mProcess->stepOver();
}

void MainWindow::slotSetBreakPoint()
{
    mProcess->setBreakPoint(9);
}

void MainWindow::slotClearBreakPoint()
{
    mProcess->clearBreakPoint(40);
}

void MainWindow::slotStepIn()
{
    mProcess->stepIn();
}

void MainWindow::slotStepOut()
{
    mProcess->stepOut();
}

void MainWindow::slotCurrentLine()
{
    qDebug() << "Current line is:" << mProcess->getCurrentLine();
}

void MainWindow::slotShowBreakpoints()
{
    mProcess->updateBreakpointsList();
    auto brkLst = mProcess->getBreakpoints();
    for(Breakpoint i : brkLst)
    {
        QString disposition = (i.getDisposition() == Breakpoint::Disposition::Keep) ? "Keep" : "Delete";
        ui->designOutput->appendPlainText(QString("Breakpoint. Disposition: %1 Function: %2 Line: %3 Enabled %4\n").arg(disposition)
                                  .arg(i.getFrame()).arg(QString::number(i.getLine()))
                                  .arg(i.isEnabled() ? "True" : "False"));
    }
}

void MainWindow::slotShowVar()
{
    try
    {
        auto content = mProcess->getVarContent("conj");
            ui->designOutput->appendPlainText(content);
    }
    catch(...)
    {
        ui->designOutput->appendPlainText("Exception handled\n");
    }
}

void MainWindow::slotShowLocal()
{
//    auto vars = mProcess->getLocalVar();
//    for(auto i : vars)
//    {
//        ui->designOutput->appendPlainText(i.append(mProcess->getVarContent(i)).append("\n"));
//    }
}

void MainWindow::slotUpdtaeLocals()
{
//    mProcess->updateLocalVariables();
//    mProcess->updateArgVariables();
    mProcess->globalUpdate();
    auto locals = mProcess->getLocalVariables();
    ui->treeWidget->clear();
    for(auto i : locals)
    {
        addTreeRoot(i);
//        ui->designOutput->appendPlainText(QString("Name: %1 Value: %2 Type: %3").arg(i.getName())
//                                          .arg(i.getContent()).arg(i.getType()));
//        std::vector<Variable> nestedTypes = i.getNestedTypes();
//        for(auto n : nestedTypes)
//        {
//            QString likelyType = mProcess->getVarType(n.getName());
//            n.setType(likelyType.isEmpty() ? "<No info>" : likelyType);
//            ui->designOutput->appendPlainText(QString("\tName: %1 Value: %2 Type: %3").arg(n.getName())
//                                              .arg(n.getContent()).arg(n.getType()));
//        }
    }
}

void MainWindow::slotGetVarType()
{
    mProcess->getVarType("conj.digit");
}

void MainWindow::slotReadPointer()
{
    ui->designOutput->appendPlainText(mProcess->getVarContent("pointer"));
}

void MainWindow::slotTestVariable()
{
    QString vec = "~\"$1 = {< std::_Vector_base < int, std::allocator < int > >>"
                  " = { _M_impl = {< std::allocator < int >> = {< __gnu_cxx::new_allocator < int >> = {"
                  "< No data fields>}, < No data fields>}, _M_start = 0x786e58, _M_finish = 0x786e74, "
                  "_M_end_of_storage = 0x786e74}\"~\"}, <No data fields>}~\"\n\"^ done(gdb)";
    Variable var("SomeName", "SomeType", vec);
    auto nestedTypes = var.readNestedStruct(var.getContent());
    for(auto i : nestedTypes)
    {
        qDebug() << i;
    }

}

void MainWindow::slotItemExpanded(QTreeWidgetItem *item)
{
    auto foundIterator = mPointersName.find(item);
    if(foundIterator != mPointersName.end())
    {
            moidifyTreeItemPointer(item);
            mPointersName.erase(foundIterator);
    }
}

void MainWindow::slotContinue()
{
    mProcess->stepContinue();
}

void MainWindow::slotKill()
{
    mProcess->kill();
}

void MainWindow::slotStipExecuting()
{
    mProcess->stopExecuting();
}
