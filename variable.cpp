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
