#ifndef VARIABLE_H
#define VARIABLE_H

#include <QString>

class Variable
{
public:
    Variable();
    Variable(QString name, QString type, QString content);
    void parseContent(const QString& line);
//private:
    QString mName;
    QString mType;
    QString mContent;
};

#endif // VARIABLE_H
