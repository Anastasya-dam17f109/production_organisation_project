#include "deliverywidget.h"

deliveryWidget::deliveryWidget(QString idW)
{
    setStyleSheet("deliveryWidget{background-color: #E6E6FA;};");
    setWindowTitle("Производство - Доставка продукции");
    id = idW;
    resize(800,1000);
    setBase();
    QSqlQuery myQuery(dbg);
    myQuery.exec("select name,position from user_list where id = "+id);
    myQuery.next();
    QLabel *label1 = new QLabel("ИМЯ  :  ");
    QLabel *label2 = new QLabel(myQuery.value(0).toString());
    QLabel *label3 = new QLabel("ДОЛЖНОСТЬ  :  ");
    QLabel *label4 = new QLabel(myQuery.value(1).toString());
    QWidget *widget1 = new QWidget;
    QHBoxLayout* layout1 = new QHBoxLayout;
    QPushButton *button = new QPushButton ("Обновить данные" );

    label1->setFixedHeight(50);
    label2->setFixedHeight(50);
    label3->setFixedHeight(50);
    label4->setFixedHeight(50);
    widget1->setFixedHeight(50);

    label1->setFixedWidth(60);
    label2->setFixedWidth(200);
    layout1->addWidget(label1);
    layout1->addWidget(label2);
    layout1->addWidget(label3);
    layout1->addWidget(label4);
    layout1->addSpacing(530);
    widget1->setLayout(layout1);

    QLabel *label = new QLabel ("МОИ ЗАКАЗЫ:");
    allOrdersTableView  = new QTableView;
    allOrdersTableView->setFixedHeight(820);
    allOrdersTableView->setFixedWidth(970);
    contextMenu    = new QMenu(this);

    updateAction   = contextMenu->addAction("Доставить заказ", this, SLOT(deliverIt()));

    allOrdersTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    allOrdersTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    allOrdersTableView->setTextElideMode(Qt::ElideRight);
    allOrdersTableView->setItemDelegate(new deliverItemDelegate(
                                                             allOrdersTableView));
    connect (allOrdersTableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));
    orderHeader<<"Имя"<<"Номер Заказа"<<"Описание заказа"<<"Адрес"<<"Дата передачи курьеру"<<"Статус доставки";
    updateView();
    QVBoxLayout * layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(widget1);
    layout->addWidget(label);
    layout->addWidget(allOrdersTableView);
    layout->addWidget(button);
    setLayout(layout);
    connect(button,SIGNAL(clicked()),this,SLOT(updateView()));
}



 void deliveryWidget::deliverIt()
 {
     QSqlQuery myQuery(dbg);
     QModelIndex id1 =  allOrdersTableView->model()->index(HighlightIndex.row(), 1);
     myQuery.exec("update order_list set deliver_state = 'доставлен' where  ordNumber = '"+ id1.data().toString()+"'");
     updateView();
 }

//вызов контекстного  меню

 void deliveryWidget::  onCustomContextMenu(const QPoint & point)
 {

     QSqlQuery myQuery(dbg);
     myQuery.exec("select n_ord from delivery_list where id_deliver = " +id);
     myQuery.next();
     if(myQuery.size()!=0)
     {
         if(allOrdersTableView->model()->rowCount()==0)
         {
             updateAction->setEnabled(false);

         }
         else
         {
             HighlightIndex = allOrdersTableView->indexAt(point);
             if(allOrdersTableView->selectionModel()->selectedRows().size()>1)
                 updateAction->setEnabled(false);
             else
             {
                 QModelIndex id1 =  allOrdersTableView->model()->index(HighlightIndex.row(), 5);
                 if(id1.data().toString()=="не доставлен")
                     updateAction->setEnabled(true);
                 else
                     updateAction->setEnabled(false);
             }


         }
         contextMenu->exec(allOrdersTableView->mapToGlobal(point));
     }
 }

//обновить вид таблицы

void deliveryWidget::updateView()
{
    QSqlQuery myQuery(dbg);

    myQuery.exec("select n_ord from delivery_list where id_deliver = " +id);
    myQuery.next();
    if(myQuery.size()!=0)
    {
        allOrdersTableView->setModel(updateSomeView( "select user_list.name as NAM_1, delivery_list.n_ord as ORD_1,  order_description AS DESCR_1,"
                                                     " Adress AS ADR_1 ,deliverDate AS DATE_1, deliver_state AS STATE_1 "
                                                     "from order_list,user_list, delivery_list  where user_list.id = "
                                                     +id
                                                     +" and delivery_list.id_deliver = "
                                                     +id
                                                     +" and delivery_list.n_ord = order_list.ordNumber group by"
                                                      " NAM_1, ORD_1, DESCR_1, ADR_1, DATE_1, STATE_1 ",
                                                     orderHeader));
        allOrdersTableView->setColumnWidth(2,180);
        allOrdersTableView->setColumnWidth(4,180);
        allOrdersTableView->setColumnWidth(5,180);
        allOrdersTableView->setColumnWidth(1,180);
    }
}

//модель для представления

QSqlQueryModel * deliveryWidget:: updateSomeView(QString command,QList<QString> header)
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);
    myQuery.exec(command);
    QSqlQueryModel * model = new QSqlQueryModel ;
    model->setQuery(  myQuery );
    for(int i= 0; i< header.size(); i++)
    {
        const char* s2 = header.at(i).toStdString().c_str();
        model->setHeaderData( i , Qt :: Horizontal , tr(s2 ) );
    }
    qDebug()<<myQuery.lastError();
    return model;
}



void  deliveryWidget::sendQuery()
{
    //if(!dbg.isOpen())
    dbg.close();
        dbg.open();
    QSqlQuery myQuery(dbg);
    emit newQuery(myQuery);
}


void deliveryWidget:: setBase()
{
    QString host   = "";
    QString port   = "";
    QString dbName = "";
    QString usName = "";
    QString login  = "";
    QString bPas   = "";


    QFile file("enterParams.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    in>>host;
    in>>port;
    in>>dbName;
    in>>usName;
    in>>login;
    in>>bPas;
    file.close();
    if(!dbg.isOpen())
    {
        dbg = QSqlDatabase::addDatabase("QPSQL");
        dbg.setHostName(host);
        dbg.setPort(port.toInt());
        dbg.setDatabaseName(dbName);
        dbg.setUserName(usName);
        dbg.setPassword(bPas);
        dbg.open();
        if(!dbg.isOpen())
        {
            QMessageBox mes;
            mes.setText("Невозможно открыть базу данных");
            mes.exec();
        }
    }
}
