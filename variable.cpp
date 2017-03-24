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

QStringList Variable::getSubVariables() const
{  // returns QStringList consists from nested variable's names (only first level)
   //  {digit = {real = 3, imaginary = 4}}
    QRegExp isPointerMatch("\\*");
    if(isPointerMatch.indexIn(mType) != -1) // if it is pointer
    {
        QString newName = mName;
        newName.prepend('*');
        return QStringList() << newName; // returns only addres of pointed object
    }
    int brackets = 0;
    QString currentName;
    QStringList nestedVareables;
    bool capture = true;
    for(int i=0;i<mContent.size();++i)
    {
        QChar ch = mContent[i];
        /* Control of nesting level */
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
        /* if captured one name should to write it int nestedVariables */
        if(ch == ' ' && !currentName.isEmpty())
        {
            capture = false;
            nestedVareables << currentName;
            currentName.clear();
            continue;
        }
        /* start captured from the next nested type (after coma) */
        if(ch == ',')
        {
            capture = true;
            continue;
        }
        /* capture only characters if nesting level = 1 and capturing is allowed */
        if(ch != ' ' && ch != '=' && ch!= ',' && brackets == 1 && capture)
        {
            currentName.append(ch);
        }
    }
    return nestedVareables;
}

QString Variable::getName() const
{
    return mName;
}

QString Variable::getType() const
{
    return mType;
}

QString Variable::getContent() const
{
    return mContent;
}
