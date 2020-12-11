#ifndef UILOGGER_H
#define UILOGGER_H

#include <QObject>

class UiLogger : public QObject
{
    Q_OBJECT
public:
    explicit UiLogger(QObject *parent = nullptr);

signals:

};

#endif // UILOGGER_H
