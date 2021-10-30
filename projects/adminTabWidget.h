#ifndef ADMINTABWIDGET_H
#define ADMINTABWIDGET_H
#include <QApplication>
#include <QWidget>
#include <QTabWidget>
#include <QTabBar>

class adminTabWidget: public QTabWidget
{
    Q_OBJECT
public:

    adminTabWidget( );
    void paintEvent(QPaintEvent *event);
};

#endif // ADMINTABWIDGET_H
