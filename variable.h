#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>
#include <vector>

class Variable
{
public:
    Variable();
    Variable(QString name, QString type, QString content);
    QStringList getSubVariables()const;
    QString getName()const;
    QString getType()const;
    QString getContent()const;
private:
    QString mName;
    QString mType;
    QString mContent;
};

#endif // VARIABLE_H
