#include "breakpoint.h"

#include <QStringList>
#include <QDebug>
#include <QRegExp>

void Breakpoint::parse(const QString &line)
{   // line should looks like $"1       breakpoint     keep y   0x0040149e in main() at main.cpp:40\n"$
    QRegExp rx(" [\\w\\d.:()]+");
//    int pos = 0;
//    qDebug() << "Line is: " << line << "\n";
//    QStringList properties;
//    int capIndex = 0;
//    while(pos != -1)
//    {
//        pos = rx.indexIn(line, pos+1);
//        QString match = rx.cap();
//        properties << match.trimmed();
//    }
//    "breakpoint"
//    "keep"
//    "y"
//    "0x0040149e"
//    "in"
//    "main()"
//    "at"
//    "main.cpp:40"
//    for(auto i : properties)
//    {
//        qDebug() << i;
//    }
//    if(properties[1] == "keep")
//    {
//        mDisposition = Disposition::Keep;
//    }
//    else
//    {
//        mDisposition = Disposition::Delete;
//    }
//    if(properties[2] == "y")
//    {
//        mEnabled = true;
//    }
//    else
//    {
//        mEnabled = false;
//    }
//    mWhat = properties[5];
//    QStringList frame = properties[7].split(':');
//    mLine = frame[1].toInt();1
      QRegExp disposition(" keep ");
      QRegExp enabled(" y ");
      QRegExp frame("\\s[:\\w]*\\([\\w\\s:,]*\\)\\s");
      QRegExp lineRegex(":\\d+");
      if(disposition.indexIn(line) == -1)
      {
          mDisposition = Disposition::Delete;
      }
      else
      {
          mDisposition = Disposition::Keep;
      }

      mEnabled = enabled.indexIn(line) == -1 ? false : true;
      lineRegex.indexIn(line);
      QString lineFrame = lineRegex.cap();
      mLine = lineFrame.split(':')[1].toInt();
      frame.indexIn(line);
      mWhat = frame.cap().trimmed();

}

Breakpoint::Breakpoint()
{
}

Breakpoint::Breakpoint(int line, QString what, bool enabled, Breakpoint::Disposition disposition):
    mLine(line),
    mWhat(what),
    mEnabled(enabled),
    mDisposition(disposition)
{
}
