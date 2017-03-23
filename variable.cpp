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
    int brackets = 0;
    QString currentName;
    QStringList nestedVars;
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
            nestedVars << currentName;
            currentName.clear();
            i = mContent.indexOf(',', i+1)+1;
            continue;
        }
        if(ch != ' ' && ch != '=' && ch!= ',' && brackets == 1)
        {
            currentName.append(ch);

        }
    }
    return nestedVars;
}
