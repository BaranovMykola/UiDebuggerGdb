#include "variable.h"

#include <QRegExp>
#include <QDebug>

Variable::Variable()
{

}

Variable::Variable(QString name, QString type, QString content):
    mName(name),
    mType(type),
    mContent(content)
{
}

void Variable::parseContent(const QString &line)
{
    /*
{real = 3, imaginary = 4}
    */
    QRegExp findContent(".*\\^done");
    qDebug() << ((findContent.indexIn(line) == -1) ? "Ntohing found" : "Something found");
}

QStringList Variable::getSubVariables() const
{
   //  {digit = {real = 3, imaginary = 4}}
    bool isPointer = false;
    QRegExp isPointerMatch("\\*");
    if(isPointerMatch.indexIn(mType) != -1)
    {
        QString newName = mName;
        newName.prepend('*');
        return QStringList() << newName;
    }
    int brackets = 0;
    QString currentName;
    QStringList nestedVars;
    bool capture = true;
    for(int i=0;i<mContent.size();++i)
    {
        QChar ch = mContent[i];
        if(ch == '{')
        {
            ++brackets;
            continue;
        }
        if(ch == '}')
        {
            --brackets;
            continue;
        }
        if(ch == ' ' && !currentName.isEmpty())
        {
            capture = false;
            nestedVars << currentName;
            currentName.clear();
            continue;
        }
        if(ch == ',')
        {
            capture = true;
            continue;
        }
        if(ch != ' ' && ch != '=' && ch!= ',' && brackets == 1 && capture)
        {
            currentName.append(ch);

        }
    }
    return nestedVars;
}
