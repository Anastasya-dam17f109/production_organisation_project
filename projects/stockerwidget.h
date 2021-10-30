#ifndef STOCKERWIDGET_H
#define STOCKERWIDGET_H
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
#include "dialogs/stockdialog.h"
#include "dialogs/stockerdetdialog.h"
#include <QHeaderView>
#include <QListView>
#include <QItemDelegate>
using namespace std;






class stockerWidget:public QWidget
{
        Q_OBJECT
public:

    QTableView  * selectStockTableView;
    QListView   * allStockListView;
    QListView   * byeStockListView;
    QComboBox   * myStocks;
    int minimalAmount;
    QMenu*  contextMenu;
    QAction *deleteAction;
    QAction *updateAction;
    QAction *addAction;
    QModelIndex HighlightIndex;
    bool personalize;
    QList<QString> allStockTableHead;
    QList<QString> allOperTableHead;

    QSqlDatabase dbg;
    QString id;

    stockerWidget(QString);
    virtual void contextMenuEvent(QContextMenuEvent* pe)
    {
        contextMenu->exec(pe->globalPos());
    }

    void setBase();
     friend class MyItemDelegate;

public slots:
    void usedOnSchemeStocks();
    void stocksWithoutDetail();
    void setCurrStockList();
    void showInfAboutStock();
    void sendQuery();
    void updateInformation();
    void callUpgradeDialog();
    void showChosen(QModelIndex ig);
    void onCustomContextMenu(const QPoint & point);
    void addDetail();
    void deleteDetail();
    void setOpersNotdone(QString destStockIndex, QString delOrd );
    QSqlQueryModel * updateSomeView(QString command,QList<QString> header,int idx);

signals:

    void newQuery(QSqlQuery q);
};


class MyItemDelegate: public QItemDelegate
{
public:
   int minAm ;
  MyItemDelegate(int min, QObject* pParent = 0) : QItemDelegate(pParent), minAm (min)
  {

  }


 void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
 {

   QStyleOptionViewItem o = option;
   QModelIndex idx = index.model()->index(index.row(), 3);
   QModelIndex idx1 = index.model()->index(index.row(), 2);

   if(minAm>idx.data().toInt()&&idx1.data().toString().lastIndexOf("#")==-1)
       o.palette.setColor(QPalette::Text, Qt::red);

   QItemDelegate::paint(painter,o,index);
 }


};
#endif // STOCKERWIDGET_H
