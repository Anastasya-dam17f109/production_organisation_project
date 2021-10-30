#ifndef CURRENTORDERVIEW_H
#define CURRENTORDERVIEW_H


#include <QDialog>
#include <QList>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QComboBox>
#include <QMessageBox>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QSqlQueryModel>
#include <QMenu>
#include "dialogs/workerdialog.h"


class currentOrderView:public QDialog
{
    Q_OBJECT
public:

    QComboBox * responsibleName;
    QTableView* curOrdView;
    QString     ordNumber;
    QSqlQuery   operDialogQuery;
    QAction *   updateAction;
    QMenu   *   contextMenu;
    QModelIndex HighlightIndex;

//создание таблицы-просмотра информации о конкретном складе

    currentOrderView(QString Number,bool managerFlag)
    {
        resize(580,300);
        setWindowTitle("Информация о заказе "+ Number);
        callQuery();
        ordNumber = Number;
        curOrdView = new QTableView;
        updateView();
        if(managerFlag)
        {
            contextMenu    = new QMenu(this);
            curOrdView->setContextMenuPolicy(Qt::CustomContextMenu);
            updateAction   = contextMenu->addAction("Изменить исполнителя", this, SLOT(changeWorker()));
            connect (curOrdView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(prepareMenu()));
            connect (curOrdView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));
        }
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(curOrdView);
        setLayout(layout);
    }

    virtual void contextMenuEvent(QContextMenuEvent* pe)
    {
        contextMenu->exec(pe->globalPos());
    }


public slots:

    //присосаться к базе

    void setQuery(QSqlQuery newQuery)
    {
        operDialogQuery = newQuery;
    }

    //вызов контекстного меню

    void   onCustomContextMenu(const QPoint & point)
    {
        HighlightIndex = curOrdView->indexAt(point);
        contextMenu->exec(curOrdView->mapToGlobal(point));
    }

    //изменить работника, выпоняющего операцию

    void changeWorker()
    {
        QModelIndex indOrd = curOrdView->model()->index(HighlightIndex.row(),0);
        QModelIndex indOp = curOrdView->model()->index(HighlightIndex.row(),1);
        operDialogQuery.exec("select id_operation from operation_list where operation_description = '"+indOp.data().toString()+"'");
        operDialogQuery.next();

        workerDialog *dial = new  workerDialog(indOrd.data().toString(),operDialogQuery.value(0).toString());
        connect(dial, SIGNAL( updateCurOrdView()), this, SLOT(updateView()));
        connect(dial, SIGNAL(callQuery()),         this, SLOT(sendQuery()));
        connect(this, SIGNAL(newQuery(QSqlQuery)), dial, SLOT(setQuery(QSqlQuery)));

        dial->exec();
    }

    void sendQuery()
    {
        emit newQuery(operDialogQuery);
    }

    void updateView()
    {
        operDialogQuery.exec("(select ordNumber, operation_description, name, state from concr_order_operation_list,user_list,operation_list where ordNumber = '"+
                             ordNumber+"' and user_list.id = concr_order_operation_list.worker and operation_list.id_operation  = concr_order_operation_list.operNumber) union"+
                             "(select ordNumber, operation_description, 'не определен',state from concr_order_operation_list,user_list,operation_list where ordNumber = '"+
                             ordNumber+"' and worker =0 and operation_list.id_operation  = concr_order_operation_list.operNumber )");
        qDebug()<<operDialogQuery.lastError();
        QSqlQueryModel * model = new QSqlQueryModel ;
        model->setQuery(operDialogQuery);
        QList<QString> header;
        header<<"Номер заказа"<<"Операция"<<"Имя исполнителя"<<"Статус операции";
        for(int i= 0; i< header.size(); i++)
        {
            const char* s2 = header.at(i).toStdString().c_str();
            model->setHeaderData( i , Qt :: Horizontal , tr(s2 ) );
        }

        curOrdView->setModel(model);
        curOrdView->setColumnWidth(2,160);
    }
signals:
    void callQuery();
    void updateOrderList();
    void newQuery(QSqlQuery f);
};
#endif // CURRENTORDERVIEW_H
