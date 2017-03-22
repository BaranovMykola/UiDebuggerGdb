#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include <QString>

struct Breakpoint
{
public:
    enum Disposition{Keep, Delete};
    void parse(const QString& line);// todo;
    Breakpoint();
    Breakpoint(int line, QString what, bool enabled, Disposition disposition);
//private:
    int mLine;
    QString mWhat;
    bool mEnabled;
    Disposition mDisposition;
};

#endif // BREAKPOINT_H
