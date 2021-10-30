#ifndef DELIVERYWIDGET_H
#define DELIVERYWIDGET_H
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

class deliveryWidget:public QWidget
{
    Q_OBJECT
public:

    QTableView  * allOrdersTableView;
    QMenu*  contextMenu;

    QAction *updateAction;
    QModelIndex HighlightIndex;
    QList<QString> orderHeader;
    QSqlDatabase dbg;
    QString id;

    deliveryWidget(QString);
    virtual void contextMenuEvent(QContextMenuEvent* pe)
    {
        contextMenu->exec(pe->globalPos());
    }

    void setBase();


public slots:

   // void updateOrderTable();
    void deliverIt();
    void updateView();
    void sendQuery();
    void onCustomContextMenu(const QPoint & point);
    QSqlQueryModel * updateSomeView(QString command,QList<QString> header);

signals:

    void newQuery(QSqlQuery q);
};


class deliverItemDelegate: public QItemDelegate
{
public:

  deliverItemDelegate( QObject* pParent = 0) : QItemDelegate(pParent)
  {

  }


 void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
 {

   QStyleOptionViewItem o = option;
   QModelIndex idx = index.model()->index(index.row(), 5);


   if(idx.data().toString()=="не доставлен")
       o.palette.setColor(QPalette::Text, Qt::red);

   QItemDelegate::paint(painter,o,index);
 }


};
#endif // DELIVERYWIDGET_H
