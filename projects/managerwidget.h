#ifndef MANAGERWIDGET_H
#define MANAGERWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMenu>
#include <QDebug>
#include <QComboBox>
#include <QTextStream>
#include <QTableView>
#include <QTreeView>
#include <QFile>
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QContextMenuEvent>
#include "dialogs/currentorderview.h"
#include "dialogs/addorderdialog.h"
#include "dialogs/deleteorderdialog.h"
#include "dialogs/deliverdialog.h"
#include <QHeaderView>
#include <QFileDialog>
using namespace std;


class managerWidget:public QWidget
{
    Q_OBJECT
public:

    QTableView  * allOrdersTableView;
    QMenu*  contextMenu;
    QAction *deleteAction;
    QAction *updateAction;
    QAction *showAction;
    QAction *showDeliverAction;
    QModelIndex HighlightIndex;
    QList<QString> orderHeader;
    QSqlDatabase dbg;
    QString id;

    managerWidget(QString);
    virtual void contextMenuEvent(QContextMenuEvent* pe)
    {
        contextMenu->exec(pe->globalPos());
    }

    void setBase();


public slots:

    void updateOrderTable();
    void choseOrder();
    void showState();
    void sendQuery();
    void deleteOrder(QString ordNumber);
    void onCustomContextMenu(const QPoint & point);
    QSqlQueryModel * updateSomeView(QString command,QList<QString> header);
    void callAddOrderDialog();
    void callDeleteOrderDialog();
    void callDeliverDialog();
    void readOrderDataFromFile();
    void addOrderDataFromFile(QStringList params);
signals:

    void newQuery(QSqlQuery q);
};

#endif // MANAGERWIDGET_H
