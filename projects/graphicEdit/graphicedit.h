#pragma once
#ifndef GRAPHICEDIT_H
#define GRAPHICEDIT_H

#include <QMainWindow>
#include <QObject>

class graphicEdit: public QMainWindow
{
    Q_OBJECT
public:

    graphicEdit();
    graphicEdit(int);

public slots:

    void sendUS()
    {
       emit updateItNow();
    }

signals:

    void updateItNow();
};

#endif // GRAPHICEDIT_H
