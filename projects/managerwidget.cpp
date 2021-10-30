#include "managerwidget.h"

managerWidget::managerWidget(QString idW )
{
   setFixedHeight(1000);
    if(!dbg.isOpen())
        dbg.open();
    setStyleSheet("managerWidget{background-color: #E6E6FA;};");
    setWindowTitle("Производство - Менеджер");
    id = idW;
    setBase();
    QSqlQuery myQuery(dbg);

    myQuery.exec("select name,position from user_list where id = "+id);
    myQuery.next();
    QLabel *label10 = new QLabel("ИМЯ  :  ");
    QLabel *label3 = new QLabel(myQuery.value(0).toString());
    QLabel *label4 = new QLabel("ДОЛЖНОСТЬ  :  ");
    QLabel *label5 = new QLabel(myQuery.value(1).toString());
    QWidget *widget1 = new QWidget;
    QHBoxLayout* layout4 = new QHBoxLayout;

    label10->setFixedHeight(50);
    label3->setFixedHeight(50);
    label4->setFixedHeight(50);
    label5->setFixedHeight(50);
    widget1->setFixedHeight(50);

    label10->setFixedWidth(60);
    label3->setFixedWidth(200);
    layout4->addWidget(label10);
    layout4->addWidget(label3);
    layout4->addWidget(label4);
    layout4->addWidget(label5);
    layout4->addSpacing(1430);
    widget1->setLayout(layout4);


    QWidget     * back1   = new QWidget;
    QWidget     * back2   = new QWidget;
    QPushButton *button1  = new QPushButton("Добавить заказ");
    QPushButton *button2  = new QPushButton("Удалить заказ");
    QPushButton *button3  = new QPushButton("Добавить заказ из файла");
    QPushButton *button4  = new QPushButton("Обновить информацию по заказам");
    QLabel      * label1  = new QLabel("СПИСОК ЗАКАЗОВ");
    QHBoxLayout *layout1  = new QHBoxLayout;
    QHBoxLayout *layout3  = new QHBoxLayout;
    QVBoxLayout *layout2  = new QVBoxLayout;
    allOrdersTableView    = new QTableView;

    orderHeader<<"Номер заказа"<<"Ответственный"<<"Описание заказа"
              <<"Размер"<<"Адрес доставки"<<"Сервис логистики"
              <<"Имя"<<"Телефон"<<"Тех.Макет"<<"Вр.приема з."
              <<"Вр.вып. з."<<"Вр. доставки"<<"Упаковка"
              <<"Готовность"<<"Статус"<<"Статус доставки";
    updateOrderTable();
    label1->setFixedHeight(20);
     setContextMenuPolicy(Qt::NoContextMenu);
    allOrdersTableView ->setFixedHeight(780);
    allOrdersTableView ->setFixedWidth(1870);
    back2->setFixedWidth(1900);
    back2->setFixedHeight(40);
    label1->setFixedWidth(200);
    label1->setFixedHeight(30);
    button1->setFixedWidth(300);
    button2->setFixedWidth(300);
    button3->setFixedWidth(300);
    button4->setFixedWidth(400);
    button4->setFixedHeight(30);
    contextMenu    = new QMenu(this);

    deleteAction   = contextMenu->addAction("Удалить заказ", this, SLOT(choseOrder()));
    showAction     = contextMenu->addAction("Показать состояние", this, SLOT(showState()));
    showDeliverAction = contextMenu->addAction("Ответственный за доставку", this, SLOT(callDeliverDialog()));

    allOrdersTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    allOrdersTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    allOrdersTableView->setTextElideMode(Qt::ElideRight);
    connect (allOrdersTableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

    layout3->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    layout3->addWidget(label1);
    layout3->addSpacing(1190);
    layout3->addWidget(button4);
    back2->setLayout(layout3);

    layout1->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    layout1->addSpacing(30);
    layout1->addWidget(button1);
    layout1->addSpacing(300);
    layout1->addWidget(button2);
    layout1->addSpacing(300);
    layout1->addWidget(button3);
    back1->setLayout(layout1);

    layout2->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    layout2->addWidget(widget1);
    layout2->addWidget(back2);
    layout2->addSpacing(10);
    layout2->addWidget(allOrdersTableView);
    layout2->addSpacing(10);
    layout2->addWidget(back1);
    setLayout(layout2);
    connect(button1,SIGNAL(clicked()),this,SLOT(callAddOrderDialog()));
    connect(button2,SIGNAL(clicked()),this,SLOT(callDeleteOrderDialog()));
    connect(button4,SIGNAL(clicked()),this,SLOT(updateOrderTable()));
    connect(button3,SIGNAL(clicked()),this,SLOT(readOrderDataFromFile()));
}


void managerWidget::choseOrder()
{
    qDebug()<<allOrdersTableView->selectionModel()->selectedRows().size();
    int count = allOrdersTableView->selectionModel()->selectedRows().size();
    QModelIndexList list = allOrdersTableView->selectionModel()->selectedRows();
    QList<QString> indexes;
    for(int i =0; i<count ;i++)
    {

        QModelIndex indID = allOrdersTableView->model()->index(list.at(i).row(),0);
        indexes<<indID.data().toString();
    }
    for(int i =0; i<count ;i++)
        deleteOrder(indexes.at(i));
}



//инфа о конкретном заказе

void managerWidget::showState()
{
    QModelIndex indID = allOrdersTableView->model()->index(HighlightIndex.row(),0);
    currentOrderView *dial = new  currentOrderView(indID.data().toString(),true);

    connect(dial, SIGNAL(callQuery()),       this, SLOT(sendQuery()));

    dial->exec();
}

//инфа о доставщике

void managerWidget::callDeliverDialog()
{
    QModelIndex indID = allOrdersTableView->model()->index(HighlightIndex.row(),0);

    deliverDialog *dial = new  deliverDialog(indID.data().toString(),true);

    connect(dial, SIGNAL(callQuery()),       this, SLOT(sendQuery()));

    dial->exec();
}

//вызов контекстного меню

void managerWidget::  onCustomContextMenu(const QPoint & point)
{
    HighlightIndex = allOrdersTableView->indexAt(point);
   // HighlightIndex.column()
    if(allOrdersTableView->model()->rowCount()==0)
    {
        deleteAction->setEnabled(false);
        showAction->setEnabled(false);
        showDeliverAction->setEnabled(false);
    }
    else
    {
        deleteAction->setEnabled(true);
        showAction->setEnabled(true);
        if(allOrdersTableView->selectionModel()->selectedRows().size()>1)
        {
            showAction->setEnabled(false);
            showDeliverAction->setEnabled(false);
        }

        QModelIndex indBild = allOrdersTableView->model()->index(HighlightIndex.row(),14);
        if(indBild.data().toString()=="собран")
        {
           showDeliverAction->setEnabled(true);
        }
        else {
            showDeliverAction->setEnabled(false);
        }
    }
    contextMenu->exec(allOrdersTableView->mapToGlobal(point));

}

//вызов диалога добавления заказа

void managerWidget::callAddOrderDialog()
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);
    myQuery.exec("select * from size_list");
    if(myQuery.size()!=0)
    {
        myQuery.exec("select * from operation_list");
        if(myQuery.size()!=0)
        {
            addOrderDialog *dial = new  addOrderDialog("-1");

            connect(dial, SIGNAL(callQuery()),       this, SLOT(sendQuery()));
            connect(this, SIGNAL(newQuery(QSqlQuery)),  dial   ,  SLOT(setQuery(QSqlQuery )));
            connect(dial, SIGNAL(updateOrderList()), this,SLOT(updateOrderTable()));
            dial->exec();
        }
        else
        {
            QMessageBox message;
            message.setText("Невозможно добавить заказ. не заданы размеры");
            message.exec();
        }
    }
    else
    {
        QMessageBox message;
        message.setText("Невозможно добавить заказ. не задана схема производства");
        message.exec();
    }
}

//вызов диалога удаления заказа

void managerWidget::callDeleteOrderDialog()
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);
    myQuery.exec("select * from operation_list");
    if(myQuery.size()!=0)
    {
        myQuery.exec("select * from size_list");
        if(myQuery.size()!=0)
        {
            myQuery.exec("select * from order_list");
            if(myQuery.size()!=0)
            {
                deleteOrderDialog *dial = new  deleteOrderDialog("-1");

                connect(dial, SIGNAL(callQuery()),       this, SLOT(sendQuery()));
                connect(dial, SIGNAL(deleteThisOrd(QString)), this,SLOT(deleteOrder(QString)));
                dial->exec();
            }
            else
            {
                QMessageBox message;
                message.setText("Невозможно удалить заказ.Еще не было добавлено ни одного");
                message.exec();
            }
        }
        else
        {
            QMessageBox message;
            message.setText("Невозможно удалить заказ. не заданы размеры");
            message.exec();
        }
    }
    else
    {
        QMessageBox message;
        message.setText("Невозможно удалить заказ. не задана схема производства");
        message.exec();
    }
}

//удалить выбранный заказ

void managerWidget:: deleteOrder(QString ordNumber)
{
     QSqlQuery myQuery(dbg);
     myQuery.exec("select operNumber, worker from concr_order_operation_list  where ordNumber = '"+ordNumber+"'");
     QList<QString>operation;
     QList<QString>worker;
     while(myQuery.next())
     {
         operation<<myQuery.value(0).toString();
         worker<<myQuery.value(1).toString();
     }

     for(int i = 0; i<operation.size();++i)
     {
         QString currOrdAmount;
         myQuery.exec("select order_amount from user_and_oper_list where id = "
                      +worker.at(i)+" and oper_id = "+operation.at(i));

         myQuery.next();
         currOrdAmount = myQuery.value(0).toString();
         myQuery.exec("update user_and_oper_list set order_amount = "
                      + QString::number(currOrdAmount.toInt()-1)
                      + " where id = "
                      +worker.at(i)+" and oper_id = "+operation.at(i));
     }
     myQuery.exec("delete from user_and_order_list where n_ord = '"+ordNumber+"'");
     myQuery.exec("delete from pers_stock_list where n_ord = '"+ordNumber+"'");
     myQuery.exec("delete from concr_order_operation_list where ordNumber = '"+ordNumber+"'");
     QString responsible;
     myQuery.exec("select resp_id from order_list where ordNumber = '"+ordNumber+"'");
     myQuery.next();
     responsible = myQuery.value(0).toString();
     QString currOrdAmount;
     myQuery.exec("select order_amount from user_and_oper_list where id = "
                  +responsible);
     myQuery.next();
     currOrdAmount = myQuery.value(0).toString();
     myQuery.exec("update user_and_oper_list set order_amount = "
                  + QString::number(currOrdAmount.toInt()-1)
                  + " where id = "
                  +responsible);
     myQuery.exec("delete from order_list where ordNumber = '"+ordNumber+"'");
     myQuery.exec("select  id_deliver from delivery_list where n_ord = '"+ordNumber+"'");
     myQuery.next();
     if(myQuery.size()!=0)
     {
         QString id_deliver = myQuery.value(0).toString();
         currOrdAmount = myQuery.value(0).toString();
         myQuery.exec("update user_and_oper_list set order_amount = "
                      + QString::number(currOrdAmount.toInt()-1)
                      + " where id = "
                      +id_deliver);
          myQuery.exec("delete from delivery_list where n_ord = '"+ordNumber+"'");
     }
     updateOrderTable();
}

//обновить  вид таблицы заказов

void managerWidget:: updateOrderTable()
{

    allOrdersTableView->setModel(updateSomeView("SELECT ordNumber,responsible,order_description,size,Adress, LogServise,Name,Phone,TechMak,"
                                                "startDate,endDate,deliverDate,pack,state,build ,deliver_state FROM  order_list where resp_id = "+id,orderHeader));
    for(int i = 0; i<orderHeader.size();++i )
       {
        if(i!=11&&i!=10&&i!=4&&i!=5&&i!=6&&i!=7&&i!=8)
        allOrdersTableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }
    allOrdersTableView->setColumnWidth(4,150);
    allOrdersTableView->setColumnWidth(5,160);
    allOrdersTableView->setColumnWidth(6,140);
    allOrdersTableView->setColumnWidth(7,150);
    allOrdersTableView->setColumnWidth(8,150);
    allOrdersTableView->setColumnWidth(10,80);
    allOrdersTableView->setColumnWidth(11,100);
}



QSqlQueryModel * managerWidget:: updateSomeView(QString command,QList<QString> header)
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);
    myQuery.exec(command);
    QSqlQueryModel * model = new QSqlQueryModel ;
    model->setQuery(  myQuery );
    qDebug()<<myQuery.lastError();
    for(int i= 0; i< header.size(); i++)
    {
        const char* s2 = header.at(i).toStdString().c_str();
        model->setHeaderData( i , Qt :: Horizontal , tr(s2 ) );
    }
    qDebug()<<myQuery.lastError();
    return model;
}



void  managerWidget::sendQuery()
{
    //if(!dbg.isOpen())
    dbg.close();
        dbg.open();
    QSqlQuery myQuery(dbg);
    emit newQuery(myQuery);
}


void managerWidget:: setBase()
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


//стот обработки добавления нажатия на кнопку добавления из файла

void managerWidget::readOrderDataFromFile()
{
    QSqlQuery orderDialogQuery(dbg);
    //size_list  (prodName,size)
    orderDialogQuery.exec("select * from size_list  ");
    orderDialogQuery.next();

    if(orderDialogQuery.size()!=0)
    {
        QString filenam = QFileDialog::getOpenFileName(0, "Open Dialog", "", "*.txt  " );
        QFile* file = new QFile(filenam);
        QTextStream in( file);
        QString str="";
        QString name_and_phone = "";
        QString size = "";
        QStringList params;
        if ((file->exists())&&(file->open(QIODevice::ReadOnly)))
        {
            in>>str;
            QString line = in.readLine();
            QStringList result;
            while(!line.isNull())
            {
                result.append(line.split(QString("\t")));

                QString str = result.at(3);
                int ind     = str.lastIndexOf("х");
                size        = str.mid(0,ind);

                params.append(str);
                params.append(result.at(4));
                params.append(result.at(5));
                params.append(result.at(6));
                params.append(result.at(7));
                params.append(result.at(8));
                params.append(result.at(11));
                addOrderDataFromFile(params);

                result.clear();
                params.clear();
                line = in.readLine();
            }
        }
        updateOrderTable();
    }
    else
    {
        QMessageBox mes;
        mes.setText("невозможно добавить заказ из файла. не заданы возможные размеры");
        mes.exec();

    }
}


void managerWidget::addOrderDataFromFile(QStringList params)
{
    QSqlQuery orderDialogQuery(dbg);
     //size_list  (prodName,size)
    orderDialogQuery.exec("select prodName from size_list where size = '"+params.at(0)+"'");
    orderDialogQuery.next();
    QString description = orderDialogQuery.value(0).toString();
    if(orderDialogQuery.size()!=0)
    {

        orderDialogQuery.exec("select Id from (select id as Id, order_amount as amount from user_and_oper_list where id IN (select id as ID from user_list where sphere = 'Услуги' ) )as foo1 where foo1.amount = (select min(amount) from (select id as Id, order_amount as  amount from user_and_oper_list  where id IN (select id as ID from user_list where sphere = 'Услуги') ) as foo1)");
        orderDialogQuery.next();
        QString responsible = orderDialogQuery.value(0).toString();
        qDebug()<<responsible;
        //if( respId!="-1")
          //  responsible = respId;
        orderDialogQuery.exec(" select  name from  user_list where id = " + id);
        orderDialogQuery.next();
        QString responsibleName = "не определен";
        if(responsible!="")
            responsibleName = orderDialogQuery.value(0).toString();
        else {
            responsible  ="null";
        }

        QString  comand =
                "insert into order_list (ordNumber, resp_id, responsible, order_description, size, Adress, LogServise,"
                " Name, Phone, TechMak, startDate, endDate,deliverDate, pack, state,build,deliver_state ) values (CONCAT('#', CAST(ceiling(random()*100000000) AS VARCHAR)) ,"
                +responsible+", '"
                +responsibleName +"','"
                +description
                +"','" +params.at(0)
                +"','" +params.at(1)+"','"
                +  params.at(2)+"','"+params.at(3)+"','" +
                params.at(4)+"','" +
                params.at(5) +"',now(), null, null,'"+ params.at(6)+ "','не готов','не собран','не доставлен') returning ordNumber";
        qDebug()<<"hfhfhfhfhfh";
        orderDialogQuery.exec(comand);
        qDebug()<<orderDialogQuery.lastError();
        orderDialogQuery.next();
        QString ordNumber = orderDialogQuery.value(0).toString();
        orderDialogQuery.exec("select  order_amount from user_and_oper_list where id =  " + responsible);
        orderDialogQuery.next();
        QString curOrdAmount = orderDialogQuery.value(0).toString();
        orderDialogQuery.exec("update user_and_oper_list set order_amount = " +QString::number(curOrdAmount.toInt() +1)+" where id =  " + responsible);
        orderDialogQuery.exec("insert into user_and_order_list(id,n_ord,n_oper,oper_desc,n_stock,stock_desc, n_room,n_equipment,personality,det_amount) values("+responsible+",'"+ordNumber+"',-1,'',-1,'',-1,-1,'',-1 )");
        QList<QString> operations;
        orderDialogQuery.exec("select id_operation from operation_list group by id_operation");
        //operation_list (id_operation int, operation_description varchar, id_source_stock int, id_dest_stock  int, x float, y float, PRIMARY KEY(id_operation, id_source_stock ))");
        while (orderDialogQuery.next())
            operations<<orderDialogQuery.value(0).toString();
        for(int i  = 0; i<operations.size(); i++)
        {
            QString workerId;
            orderDialogQuery.exec("select ID from (select id as ID, order_amount as amount from user_and_oper_list where oper_id ="
                                  +operations.at(i)+") as foo where  amount = ( select  min(amount) from  (select id as ID, order_amount as amount from user_and_oper_list where oper_id ="
                                  +operations.at(i)+") as  foo )");
            orderDialogQuery.next();
            if(orderDialogQuery.size()!=0)
                workerId = orderDialogQuery.value(0).toString();
            else
                workerId = "0";
            orderDialogQuery.exec(" insert into concr_order_operation_list (ordNumber, operNumber, worker,state) values ('"+ ordNumber+"', "+operations.at(i)+","+workerId +","+"'не выполнена')");
            orderDialogQuery.exec("select operation_description from operation_list where id_operation = "+operations.at(i));
            orderDialogQuery.next();
            QString operation_description = orderDialogQuery.value(0).toString();
            orderDialogQuery.exec("insert into user_and_order_list(id,n_ord,n_oper,oper_desc,n_stock,stock_desc, n_room,n_equipment,personality,det_amount) values("
                                  +workerId+",'"+ordNumber+"',"
                                  +operations.at(i)+",'"
                                  +operation_description
                                  +"',-1,'',-1,-1,'',-1 )");
            orderDialogQuery.exec("select order_amount from user_and_oper_list where id = "+workerId+" and oper_id = "+ operations.at(i));
            orderDialogQuery.next();
            qDebug()<<orderDialogQuery.value(0).toString();
            QString order_amount = orderDialogQuery.value(0).toString();
            orderDialogQuery.exec("update user_and_oper_list set order_amount = "+QString::number(order_amount.toInt()+1)+" where id = "+workerId+" and oper_id = "+ operations.at(i));
            qDebug()<<workerId;
        }
        QList<QString> pers_stocks;
        orderDialogQuery.exec("select id_stock from stock_list where personalize = 'да'");
        while (orderDialogQuery.next())
            pers_stocks<<orderDialogQuery.value(0).toString();
        for(int i  = 0; i<pers_stocks.size(); i++)
        {
            orderDialogQuery.exec("select stock_description from stock_list where id_stock = "+pers_stocks.at(i));
            orderDialogQuery.next();
            QString stock_description = orderDialogQuery.value(0).toString();
            orderDialogQuery.exec("insert into pers_stock_list (id_stock,stock_description, n_ord, amount) values ("
                                  +pers_stocks.at(i)+",'"+stock_description+"','"+ordNumber+"',0)");
            orderDialogQuery.exec("select id_user from userPersSt where id_stock = "+pers_stocks.at(i));
            QList<QString> users;
            while(orderDialogQuery.next())
                users<<orderDialogQuery.value(0).toString();
            for(int j = 0; j<users.size(); ++j)
            orderDialogQuery.exec("insert into user_and_order_list (id,n_ord,n_oper,oper_desc,n_stock,stock_desc,n_room,n_equipment,personality,det_amount) values ("+
                                 users.at(j)+",'"+ordNumber+"',-1,''," +pers_stocks.at(i)+",'"+stock_description+"',-1,-1,'да',0)");

        }
    }
}
