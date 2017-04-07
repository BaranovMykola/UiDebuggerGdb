#include "gdb.h"

#include <QDebug>
#include <iostream>
#include <QRegExp>

Gdb::Gdb()
{

}

Gdb::Gdb(QString gdbPath):
    mCaptureLocalVar(false),
    mCaptureLocalVarSeveralTimes{0}
{
    mGdbFile.setFileName(gdbPath);
    connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(slotReadStdOutput()), Qt::UniqueConnection);
    connect(this, SIGNAL(readyReadStandardError()), this, SLOT(slotReadErrOutput()), Qt::UniqueConnection);
}

void Gdb::start(const QStringList &arguments, QIODevice::OpenMode mode)
{   //starts QPRocess, opens $mGdbFile.fileName()$ and passes $arguments$ as arguments
    QProcess::start(mGdbFile.fileName(), arguments, mode);
}

void Gdb::write(QByteArray &command)
{   //wrtie command to GDB. You shouldn't pass command with '\n' It will appended here.
    QByteArray enter("\n");
    command.append(enter);
    QProcess::write(command);
}

void Gdb::readStdOutput()
{   //Reads all standart output from GDB
    mBuffer = QProcess::readAll();
    //^error,msg="The program is not being run."
    QRegExp errorMatch("\\^error");
    if(errorMatch.indexIn(mBuffer) != -1)
    {
        QRegExp errorMsg("msg=[\\w\\s\"]+");
        errorMsg.indexIn(mBuffer);
        mErrorMessage = errorMsg.cap();
        emit signalErrorOccured(mErrorMessage);
    }
}

void Gdb::readErrOutput()
{
    mBuffer = QProcess::readAllStandardError();
}

const QString &Gdb::getOutput() const
{   //Retrun mBuffer output
    return mBuffer;
}

QStringList Gdb::getLocalVar()
{   //finds and returns all local variable names
    /*
    ~"comp = {real = 3"
    ~", imaginary = 4"
    ~"}"
    */
    write(QByteArray("info local"));
    QProcess::waitForReadyRead(1000);
    QRegExp varMatch("\"\\w+\\s="); // find substring from '"' to '=' included only characters,
                                    // digits and whitespaces (it's a var name)
    int pos = 0;
    QStringList locals;
    while(pos != -1)// reads all matches
    {
        pos = varMatch.indexIn(mBuffer, pos+1);//find next matches
        QRegExp clean("\"|\\s|=");// find all garbage characters
        QString varName = varMatch.cap().replace(clean, "").trimmed();// clean garbage and whitespaces
        if(!varName.isEmpty())
        {
            locals << varName;
        }
    }
    return locals;
}

QStringList Gdb::getArgVar()
{
    write(QByteArray("info arg"));
    QProcess::waitForReadyRead(1000);
    QRegExp varMatch("\"\\w+\\s="); // find substring from '"' to '=' included only characters,
                                    // digits and whitespaces (it's a var name)
    int pos = 0;
    QStringList locals;
    while(pos != -1)// reads all matches
    {
        pos = varMatch.indexIn(mBuffer, pos+1);//find next matches
        QRegExp clean("\"|\\s|=");// find all garbage characters
        QString varName = varMatch.cap().replace(clean, "").trimmed();// clean garbage and whitespaces
        if(!varName.isEmpty())
        {
            locals << varName;
        }
    }
    return locals;
}

void Gdb::openProject(const QString &fileName)
{   //opens file $fileName$ in gdb to debug it via target exec and file
    write(QByteArray("target exec ").append(fileName));
    write(QByteArray("file ").append(fileName));
//    write(QByteArray("set new-console on"));
}

void Gdb::run()
{   //run debugging
    write(QByteArray("run"));
}

void Gdb::stepOver()
{   //goes to the next line of code
    write(QByteArray("next"));
}

void Gdb::setBreakPoint(unsigned int line)
{   //set simple breakpoint at line $line$
    write(QByteArray("b ").append(QString::number(line)));
}

void Gdb::clearBreakPoint(unsigned int line)
{   //clear breakpoint at line $line$
    write(QByteArray("clear ").append(QString::number(line)));
}

void Gdb::stepIn()
{   //step into function under cursor
    write(QByteArray("step"));
}

void Gdb::stepOut()
{   //step out of current function\method
    write(QByteArray("finish"));
}

int Gdb::getCurrentLine()
{   //returns current line of code or -1 if any aerror occured
    /*
    ~"#0  main () at main.cpp:46\n"
    ~"46\t\tcout << \"Finished main\";\n"
    ^done
    */
    write(QByteArray("frame"));
    QProcess::waitForReadyRead();
    QRegExp rx(":\\d+"); //finds ':46'
    if(rx.indexIn(mBuffer) == -1)
    {
        return -1; //not found
    }
    QStringList lst = rx.capturedTexts(); //found :46
    QString line = lst[0];// first matches
    return line.split(':').last().toInt();// (int)"46"
}

void Gdb::updateBreakpointsList()
{   //update std::vector<Breakpoint>  mBreakpointsList with relevant info
    /*
        &"info b\n"
        ~"Num     Type           Disp Enb Address    What\n"

        ~"1       breakpoint     keep y   0x00401516 in main() at main.cpp:46\n"
        ~"\tbreakpoint already hit 1 time\n"
        ~"2       breakpoint     keep y   0x00401516 in main() at main.cpp:46\n"
        ^done
    */
    write(QByteArray("info b"));
    QProcess::waitForReadyRead(1000);
    QStringList lines = mBuffer.split('~'); // split by CLI output lines
    mBreakpointsList.clear(); // clear old info
    for(int i=2;i<lines.size();++i) //read all lines except of first two (command line and topic line)
    {
        Breakpoint currentBreakpoint;
        QString currentLine = lines[i];
        try
        {
            currentBreakpoint.parse(currentLine); // full breakpoint from $currentLine$
            mBreakpointsList.push_back(currentBreakpoint); // write relevant breakpoint to vector
        }
        catch(std::exception)
        {   //split breakpoint, if there are some error while processing $currentLine$
        }
    }
}

void Gdb::updateLocalVariables()
{   //update std::vector<Variables> mVariablesList with relevant info
    QStringList locals = getLocalVar(); // get all locals var
    mVariablesList.clear(); // clear old info
    for(auto i : locals)
    {
        Variable var(i, getVarType(i), getVarContent(i));
        mVariablesList.push_back(var);
    }
}

void Gdb::updateArgVariables()
{
    QStringList locals = getArgVar(); // get all arg var
    for(auto i : locals)
    {
        Variable var(i, getVarType(i), getVarContent(i));
        mVariablesList.push_back(var);
    }
}

std::vector<Breakpoint> Gdb::getBreakpoints() const
{   //returns list of all breakpoint
    return mBreakpointsList;
}

std::vector<Variable> Gdb::getLocalVariables() const
{   //returns list of all variables
    return mVariablesList;
}

QString Gdb::getVarContent(const QString& var)
{   //return variables content in QString format
    /*
        ~"$1 = {digit = {real = 3"
        ~", imaginary = 4"
        ~"}"
        ~"}"
        ~"\n"
        ^done
    */
    write(QByteArray("print ").append(var));
    QProcess::waitForReadyRead(1000);
//    QRegExp errorMatch("\\^done");
//    if(errorMatch.indexIn(mBuffer) == -1)
//    {
//        //throw std::exception("Error while var reading");
//    }
    QRegExp content("=\\s.*\\^done"); // match string beginning with '= ' and ending with '^done'
    QRegExp clean("[\\\\|\|\"|~]"); // find all garbage characters '\', '"', '~'
    QRegExp pointerMatch("\\(.*\\s*\\)\\s0x[\\d+abcdef]+"); // try to regonize pointer content
                                                            // (SOME_TYPE *) 0x6743hf2
    if(pointerMatch.indexIn(mBuffer) != -1)
    {
        QString addres = pointerMatch.cap().split(' ').last();  // get only hex addres
        return addres;
    }
    content.indexIn(mBuffer);
    QString res = content.cap().replace(clean, "").replace("^done", "").trimmed();
    res.resize(res.size()-1); // last character is garbate too
    /* Removed all line breaks */
    auto lst = res.split('\n');
    for(QString& i : lst)
    {
        i = i.trimmed();
    }
    QString withoutLines = lst.join(""); // complete one QString again
    withoutLines.remove(0, 2); // first two charactes are garbage too '= '
    return withoutLines;
}

QString Gdb::getVarType(const QString &variable)
{   //finds and returns type of variable $variable$
    /*
        &"whatis conj\n"
        ~"type = Conjugate\n"
        ^done
    */
    write(QByteArray("whatis ").append(variable));
    QProcess::waitForReadyRead(1000);
    QRegExp findType("type\\s=\\s[\\w:\\*\\s\<\>\,]+"); // find string after 'type = ' included only characters,
                                                 // digits, uderscores, '*' and whitespaces
    if(findType.indexIn(mBuffer) == -1) // if not found
    {
        return QString();
    }
    QString type = findType.cap();
    QString bareType = type.split('=')[1].trimmed(); // type is string after 'type = '
    return bareType;
}

void Gdb::globalUpdate()
{   // update all informations
    updateBreakpointsList();
    updateLocalVariables();
    updateArgVariables();   //TODO (clear info in this method);
}

void Gdb::slotReadStdOutput()
{
    readStdOutput();
}

void Gdb::slotReadErrOutput()
{
    readErrOutput();
}

void Gdb::slotReadLocalVar()
{
    mLocalVar = QProcess::readAll();
}
