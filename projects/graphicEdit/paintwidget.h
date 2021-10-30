#pragma once
#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

#include <QObject>

#include <QWidget>
#include "graphwidget.h"
#include "edge.h"
#include "node.h"
#include "dialogs/namedialog.h"
#include "dialogs/responsibledialog.h"
#include "dialogs/currentorderview.h"
#include "dialogs/usersdialog.h"
#include "dialogs/stockdialog.h"
#include "dialogs/operationdialog.h"
#include "dialogs/userselectdialog.h"
#include "dialogs/addorderdialog.h"
#include "dialogs/deleteorderdialog.h"
#include "dialogs/sizedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QHeaderView>
#include "graphicEdit/graphicedit.h"
#include "dialogs/stockerdetdialog.h"
#include "dialogs/deliverdialog.h"
#include <QFileDialog>
class PaintWidget:public QWidget
{
    Q_OBJECT
public:
    PaintWidget();
    PaintWidget(int);
    PaintWidget(QString);
    PaintWidget(QString, int);
    PaintWidget(int, int);

    QSqlQueryModel * updateSomeView(QString command,QList<QString> header);
    QString setProdName();
    void setBaseAndQuery();

    QLineEdit *productionName;
    QLineEdit *minAmount;
    QComboBox  *myStocks;

    QSqlDatabase dbg;
    QList<QString> sizeTableHead;
    QList<QString> positionTableHead;
    QList<QString> usersTableHead;
    QList<QString> allStockTableHead;
    QList<QString> allOperTableHead;
    QList<QString> nSpCurrHeader;
    QList<QString> SpCurrHeader;
    QList<QString> orderHeader;

    QTableView  * sizeTableView;
    QTableView  * usersTableView;
    QTableView  * usersOperTableView;
    QTableView  * usersOrderTableView;
    QTableView  * positionTableView;
    QTableView  * allStockTableView;
    QTableView  * allOperationTableView;
    QTableView  * selectStockTableView;
    QTableView  * allOrdersTableView;

    GraphWidget * shemeBrowseWidg;
    QPushButton *button4;
    QString notSpesCurrReq = "";
    QString spesCurrReq = "";
    int    selectedName = 0;
    QMenu*  contextMenu;
    QAction *deleteAction;
    QAction *updateAction;
    QAction *showAction;
    QAction *showDeliverAction;
    QModelIndex HighlightIndex;
    bool ordersWidgetFlag;
    bool personalize = true;

    virtual void contextMenuEvent(QContextMenuEvent* pe)
    {
        contextMenu->exec(pe->globalPos());
    }

    virtual void mousePressEvent(QMouseEvent* event) override
    {
        setFocus();
    }
public slots:

    // все для виджета графического редактора
    void clearSceneFromMenu();
    void saveSceneFromMenu();
    void addStock();
    void addOperation();
    void execComandFromGraph(QString command);//выполнение(внесение в базу данных) команды, пришедшей с графического поля
    void sendQuery();// для загрузки графики передается сцене (работает внутри метода browesheme
    void sendUpdateSignal();

    //все для виджета-отображателя схемы
    void  callShemeEdit();
    void callNameDialog();// вызов окошка, в котором изменяется название продукции
    void callAddSizeDialog();// нажатие кнопки - вызов диалога добавления размера
    void callDeleteSizeDialog();// нажатие кнопки - вызов диалога удаления размера

    void setNewName(QString str);//установка нового имени продукции
    void addNewSize(QString str);//установка размера
    void deleteSize(QString str);//установка размера
    void updateShemeAfterSaving();
    void callUpdateSheme();


    //все для виджета персонала
    void callAddPositionDialog();// вызов окошка добавления новой должности
    void callDeletePositionDialog();//вызов окошка удаления новой должности
    void callUserDialog();
    void callSelectUserDialog();
    void callStockDialog();
    void callOperationDialog();
    void callUpgradeDialog();
    void callAddOrderDialog();
    void callDeleteOrderDialog();

    void addNewPosition(QString,QString );//добавление в базу новой должности
    void deletePosition(QString );//удаление из базы  должности
    void addNewUser(QString name, QString login, QString password, QString position, QList<int> operations);
    void deleteUser(QString id);

    void setInformationAboutUser(QString id, QString sphere , QList<int> operations);
    void setCurrentRequest (QString nsp, QString sp,QList<QString>,QList<QString>);
    void showNotSpesInformation();
    void showSpesInformation();
    void makeUpdateUserTable();
    void setSelectedName(int);
    void updateInformationTable();

    //все для виджета складов
    void usedOnSchemeStocks();
    void usedOnSchemeOperations();
    void setCurrStockList();
    void showInfAboutStock();
    void returnValue();
    void setValue();
    void addDetail();
    void setOpersNotdone(QString destStockIndex, QString delOrd );
    void deleteDetail();

    //все для виджета заказов
    void updateOrderTable();
    void deleteOrder(QString);
    void onCustomContextMenu(const QPoint &);
    void choseOrder();
    void changeResponsible();
    void showState();
    void callDeliverDialog();
    void readOrderDataFromFile();
    void addOrderDataFromFile(QStringList params);

    //void Value(QWidget*,QWidget*);

signals:

    void clearSignal();
    void saveSignal();
    void stockSignal();
    void operationSignal();
    void browseSignal();
    void newQuery(QSqlQuery q);
    void updating();
    void updateScheme();
    void updateStockList();
    void updateOrderList();


};



class NewItemDelegate: public QItemDelegate
{
public:
   int minAm ;
  NewItemDelegate(int min, QObject* pParent = 0) : QItemDelegate(pParent), minAm (min)
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
#endif // PAINTWIDGET_H
