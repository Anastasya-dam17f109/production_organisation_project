#include "graphicEdit/paintwidget.h"
#include <QPushButton>

//виджет редактора схемы

PaintWidget::PaintWidget(): QWidget(0)
{
    setFixedHeight(1000);
    GraphWidget *widget0  = new GraphWidget(this);
    GraphWidget *widget1  = new GraphWidget(this,0);
    QHBoxLayout *layout   = new QHBoxLayout;
    QWidget     *widList  = new QWidget;
    QVBoxLayout *laytList = new QVBoxLayout;
    QLabel      * label   = new QLabel("ЭЛЕМЕНТЫ СХЕМЫ");
    QPushButton *button1  = new QPushButton("Очистить сцену");
    QPushButton *button2  = new QPushButton("Сохранить изменения");
    QPushButton *button3  = new QPushButton("Добавить операцию");
    QPushButton *button4  = new QPushButton("Добавить склад");

    widList-> setFixedHeight(1000);
    widget0->setFixedHeight(900);
    label->setFixedHeight(20);
    widList->setFixedWidth(200);
    laytList->setAlignment(Qt::AlignTop);
    laytList->addWidget(label);
    laytList->addWidget(widget1);
    laytList->addSpacing(60);
    laytList->addWidget(button1);
    laytList->addSpacing(30);
    laytList->addWidget(button2);
    laytList->addSpacing(30);
    laytList->addWidget(button3);
    laytList->addSpacing(30);
    laytList->addWidget(button4);
    widList->setLayout(laytList);
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(widList);
    layout->addWidget(widget0);
    connect(widget0,SIGNAL( callQuery()),       this,    SLOT(sendQuery()));
    connect(this,   SIGNAL(newQuery(QSqlQuery)),widget0, SLOT(catchQuery(QSqlQuery)));
   // qDebug()<<"gggggg";
    widget0->browseScheme();

    connect(widget0,SIGNAL(sendCommand(QString)),this, SLOT(execComandFromGraph(QString)));
    connect(widget0,SIGNAL(updateBrowsedSheme()),this, SLOT(sendUpdateSignal()));
    connect(button1,SIGNAL(clicked()),        widget0, SLOT(menuClearScene()));
    connect(this,   SIGNAL(clearSignal()),    widget0, SLOT(menuClearScene()));

    connect(this,   SIGNAL(saveSignal()),     widget0, SLOT(menuSave()));
    connect(button2,SIGNAL(clicked()),        widget0, SLOT(menuSave()));

    connect(this,   SIGNAL(stockSignal()),    widget0, SLOT(menuAddStock()));
    connect(button4,SIGNAL(clicked()),        widget0, SLOT(menuAddStock()));

    connect(this,   SIGNAL(operationSignal()),widget0, SLOT(menuAddOperation()));
    connect(button3,SIGNAL(clicked()),        widget0, SLOT(menuAddOperation()));

    setLayout(layout);
}

// устанавливает название текущей продукции

QString PaintWidget::setProdName()
{

    QSqlQuery myQuery(dbg);
    myQuery.exec("select* from name_list");
    QString name = "";
    while(myQuery.next())
    {
        name = myQuery.value(0).toString();
    }
    dbg.close();
    return name;
}

// устанавливает к какой базе подключаемся

void PaintWidget:: setBaseAndQuery()
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

// виджет со схемой

PaintWidget::PaintWidget(int ): QWidget(0)
{
    setFixedHeight(1000);
    sizeTableHead<<"Название"<<"Размер";
    QWidget     * back1   = new QWidget;
    QWidget     * back2   = new QWidget;
    shemeBrowseWidg       = new GraphWidget(this);
    QVBoxLayout *laytList = new QVBoxLayout;
    QVBoxLayout *typeList = new QVBoxLayout;
    QLabel      * label   = new QLabel("СХЕМА ПРОИЗВОДСТВА");
    QLabel      *label1   = new QLabel("НАЗВАНИЕ ПРОДУКЦИИ");
    QLabel      *label2   = new QLabel("ТАБЛИЦА РАЗМЕРОВ");
    productionName        = new QLineEdit;
    QHBoxLayout *layout   = new QHBoxLayout;
    QPushButton *button1  = new QPushButton("Редактировать схему");
    QPushButton *button2  = new QPushButton("Изменить название продукции");
    QPushButton *button3  = new QPushButton("Добавить новый размер");
    QPushButton *button4  = new QPushButton("Убрать существующий размер");
    sizeTableView         = new QTableView;

    shemeBrowseWidg->editable     = false;
    back1->setFixedHeight(1000);
    back1->setFixedWidth(1600);
    sizeTableView->setFixedHeight(600);
    back2->setFixedWidth(300);
    label->setFixedHeight(20);
    label1->setFixedHeight(20);
    productionName->resize(40,100);
    productionName->setReadOnly(true);
    productionName->setText(setProdName());
    sizeTableView->setModel(updateSomeView("select * from size_list",sizeTableHead));
    sizeTableView->setColumnWidth(0, 150);

    sizeTableView->setContextMenuPolicy(Qt::NoContextMenu);
    setContextMenuPolicy(Qt::NoContextMenu);

    laytList->setAlignment(Qt::AlignTop);
    laytList->addWidget(label);
    laytList->addWidget(shemeBrowseWidg);
    laytList->addWidget(button1);
    back1->setLayout(laytList);

    typeList->setAlignment(Qt::AlignTop);
    typeList->addWidget(label1);
    typeList->addWidget(productionName);
    typeList->addWidget(button2);
    typeList->addSpacing(80);
    typeList->addWidget(label2);
    typeList->addWidget(sizeTableView);
    typeList->addWidget(button3);
    typeList->addWidget(button4);
    typeList->addSpacing(80);
    back2->setLayout(typeList);

    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(back1);
    layout->addWidget(back2);
    connect(shemeBrowseWidg,SIGNAL( callQuery()),       this,    SLOT(sendQuery()));
    connect(this,   SIGNAL(newQuery(QSqlQuery)),shemeBrowseWidg, SLOT(catchQuery(QSqlQuery)));
    shemeBrowseWidg->browseScheme();

    setLayout(layout);
    connect(button2, SIGNAL(clicked()), this, SLOT(callNameDialog()));
    connect(button1, SIGNAL(clicked()), this, SLOT(callShemeEdit()));
    connect(button3, SIGNAL(clicked()), this, SLOT(callAddSizeDialog()));
    connect(button4, SIGNAL(clicked()), this, SLOT(callDeleteSizeDialog()));

}

//виджет с заказами

PaintWidget::PaintWidget(QString ): QWidget(0)
{

    setFixedHeight(1000);
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);

    myQuery.exec("CREATE TABLE order_list (ordNumber varchar,resp_id int, responsible varchar, order_description varchar,"
                 " size varchar, Adress varchar, LogServise varchar, Name varchar, Phone varchar, TechMak varchar, startDate timestamp, endDate timestamp, deliverDate timestamp, pack varchar,"
                 " state varchar, build varchar, deliver_state varchar,PRIMARY KEY(OrdNumber))");
    qDebug()<<myQuery.lastError();
    myQuery.exec("CREATE TABLE concr_order_operation_list (ordNumber varchar, operNumber int, worker int, state varchar, PRIMARY KEY(OrdNumber,operNumber))");
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
//
    orderHeader<<"Номер заказа"<<"Ответственный"<<"Описание заказа"
              <<"Размер"<<"Адрес доставки"<<"Сервис логистики"
              <<"Имя"<<"Телефон"<<"Тех.Макет"<<"Вр.приема з."
              <<"Вр.вып. з."<<"Вр. доставки"<<"Упаковка"
              <<"Готовность"<<"Статус"<<"Статус доставки";
    updateOrderTable();
    label1->setFixedHeight(20);
    allOrdersTableView ->setFixedHeight(800);
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
    ordersWidgetFlag  = true;
    contextMenu       = new QMenu(this);
    setContextMenuPolicy(Qt::NoContextMenu);
    deleteAction      = contextMenu->addAction("Удалить заказ", this, SLOT(choseOrder()));
    updateAction      = contextMenu->addAction("Изменить ответственного", this, SLOT(changeResponsible()));
    showAction        = contextMenu->addAction("Показать состояние", this, SLOT(showState()));
    showDeliverAction = contextMenu->addAction("Ответственный за доставку", this, SLOT(callDeliverDialog()));

    allOrdersTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    allOrdersTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    allOrdersTableView->setTextElideMode(Qt::ElideRight);

    connect (allOrdersTableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(prepareMenu()));
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
    layout2->addWidget(back2);
    layout2->addSpacing(20);
    layout2->addWidget(allOrdersTableView);
    layout2->addSpacing(20);
    layout2->addWidget(back1);
    setLayout(layout2);
    connect(button1,SIGNAL(clicked()),this,SLOT(callAddOrderDialog()));
    connect(button2,SIGNAL(clicked()),this,SLOT(callDeleteOrderDialog()));
    connect(button4,SIGNAL(clicked()),this,SLOT(updateOrderTable()));
    connect(button3,SIGNAL(clicked()),this,SLOT(readOrderDataFromFile()));
}

//выбрать заказ для удаления

void PaintWidget::choseOrder()
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

//изменить ответственного

void PaintWidget::changeResponsible()
{
    int count = allOrdersTableView->selectionModel()->selectedRows().size();
    QModelIndexList list = allOrdersTableView->selectionModel()->selectedRows();
    QList<QString> indexes;
    for(int i =0; i<count ;i++)
    {

        QModelIndex indID = allOrdersTableView->model()->index(list.at(i).row(),0);
        indexes<<indID.data().toString();
    }

    responsibleDialog *dial = new  responsibleDialog(indexes);

    connect(dial, SIGNAL(callQuery()),       this, SLOT(sendQuery()));
    connect(dial, SIGNAL(updateOrderList()), this,SLOT(updateOrderTable()));
    dial->exec();
}

//инфа о конкретном складе

void PaintWidget::showState()
{
    QModelIndex indID = allOrdersTableView->model()->index(HighlightIndex.row(),0);
    currentOrderView *dial = new  currentOrderView(indID.data().toString(),false);

    connect(dial, SIGNAL(callQuery()),       this, SLOT(sendQuery()));

    dial->exec();
}

//инфа о доставщике

void PaintWidget::callDeliverDialog()
{
    QModelIndex indID = allOrdersTableView->model()->index(HighlightIndex.row(),0);

    deliverDialog *dial = new  deliverDialog(indID.data().toString(),false);

    connect(dial, SIGNAL(callQuery()),       this, SLOT(sendQuery()));

    dial->exec();
}

//вызов контекстного меню

void PaintWidget::  onCustomContextMenu(const QPoint & point)
{
    if(ordersWidgetFlag )
    {
        HighlightIndex = allOrdersTableView->indexAt(point);
        if(allOrdersTableView->model()->rowCount()==0)
        {
            deleteAction->setEnabled(false);
            updateAction->setEnabled(false);
            showAction->setEnabled(false);
            showDeliverAction->setEnabled(false);
        }
        else
        {
            deleteAction->setEnabled(true);
            updateAction->setEnabled(true);
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
    else
    {

        // HighlightIndex.column()

        if(myStocks->currentIndex()==0)
        {
            deleteAction->setEnabled(false);
            updateAction->setEnabled(false);
        }
        else
        {

            HighlightIndex = selectStockTableView->indexAt(point);
            // HighlightIndex.column()
            if(selectStockTableView->model()->rowCount()==0&&myStocks->currentIndex()!=0)
            {
                deleteAction->setEnabled(false);
                updateAction->setEnabled(false);
            }
            else
            {
                if(!personalize)
                {
                    deleteAction->setEnabled(true);
                    updateAction->setEnabled(true);

                    QModelIndex id1 =  selectStockTableView->model()->index(HighlightIndex.row(), 3);
                    if(id1.data().toInt()==0)
                        deleteAction->setEnabled(false);
                }
                else
                {
                    deleteAction->setEnabled(true);
                    updateAction->setEnabled(false);
                    QModelIndex id1 =  selectStockTableView->model()->index(HighlightIndex.row(), 3);
                    if(id1.data().toInt()==0)
                        deleteAction->setEnabled(false);
                }
            }
            contextMenu->exec(selectStockTableView->mapToGlobal(point));
        }
    }

}

//стот обработки добавления нажатия на кнопку добавления из файла

void PaintWidget::readOrderDataFromFile()
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


                params.append(str );
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


void PaintWidget::addOrderDataFromFile(QStringList params)
{
    QSqlQuery orderDialogQuery(dbg);
     //size_list  (prodName,size)
    orderDialogQuery.exec("select prodName from size_list where size = '"+params.at(0)+"'");
    qDebug()<<orderDialogQuery.lastError();
    orderDialogQuery.next();
    QString description = orderDialogQuery.value(0).toString();
    if(orderDialogQuery.size()!=0)
    {

        orderDialogQuery.exec("select Id from"
                              " (select id as Id, order_amount as amount from user_and_oper_list where id IN "
                              "(select id as ID from user_list where sphere = 'Услуги' ) )as foo1 where foo1.amount = "
                              "(select min(amount) from (select id as Id, order_amount as  amount from user_and_oper_list  where id "
                              "IN (select id as ID from user_list where sphere = 'Услуги') ) as foo1)");
        orderDialogQuery.next();
        QString responsible = orderDialogQuery.value(0).toString();
        qDebug()<<responsible;
        //if( respId!="-1")
          //  responsible = respId;
        orderDialogQuery.exec(" select  name from  user_list where id = " + responsible);
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

//вызов диалога добавления заказа

void PaintWidget::callAddOrderDialog()
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

void PaintWidget::callDeleteOrderDialog()
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

void PaintWidget::deleteOrder(QString ordNumber)
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

void PaintWidget::updateOrderTable()
{

    allOrdersTableView->setModel(updateSomeView("SELECT ordNumber,responsible,order_description,size,Adress, LogServise,Name,Phone,TechMak,startDate,endDate,deliverDate,pack,state,build ,deliver_state FROM  order_list ",orderHeader));
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

//виджет с пользователями

PaintWidget::PaintWidget(QString ,int): QWidget(0)
{
    QSqlQuery myQuery(dbg);
    myQuery.exec("create table position_list(position varchar, sphere varchar, primary key (position))");
    myQuery.exec("create table delivery_list( id_deliver int , n_ord varchar)");
    myQuery.exec("create table user_list(id int, name varchar, position varchar, sphere varchar, login varchar,  password varchar, primary key (id))");
    myQuery.exec("create table user_and_oper_list(id int , oper_id int, oper_desc varchar, order_amount int, stock_amount int, room_amount int, equipment_amount int, primary key (id,oper_id))");
    myQuery.exec("create table user_and_order_list( id int , n_ord varchar, n_oper int, oper_desc varchar, n_stock int, stock_desc varchar, n_room int,  n_equipment int, personality varchar, det_amount int, primary key (id,n_ord,n_oper,n_stock,n_room,n_equipment))");
    QWidget     * back1   = new QWidget;
    QWidget     * back2   = new QWidget;
    QWidget     * back3   = new QWidget;
    QWidget     * back4   = new QWidget;
    usersTableView        = new QTableView;
    usersOperTableView    = new QTableView;
    usersOrderTableView   = new QTableView;
    positionTableView     = new QTableView;
    QLabel      * label1  = new QLabel("СПИСОК ПЕРСОНАЛА");
    QLabel      * label2  = new QLabel("РЕЗУЛЬТАТ ВЫБОРКИ");
    QLabel      * label4  = new QLabel("ДОЛЖНОСТИ");
    QHBoxLayout *layout1  = new QHBoxLayout;
    QHBoxLayout *layout5  = new QHBoxLayout;
    QPushButton *button1  = new QPushButton("Открыть редактор списка персонала");
    QPushButton *button2  = new QPushButton("Добавить должность");
    QPushButton *button3  = new QPushButton("Удалить должность");
    QPushButton *button4  = new QPushButton("Сделать выборку из списка пользователей");
    //QPushButton *button5  = new QPushButton("Изменить распределение нагрузки на работников");
    QPushButton *button6  = new QPushButton("Общая информация");
    QPushButton *button7  = new QPushButton("Детальная информация");
    QPushButton *button8  = new QPushButton("Обновить данные по данной выборке");

    QVBoxLayout *layout2  = new QVBoxLayout;
    QVBoxLayout *layout3  = new QVBoxLayout;
    QVBoxLayout *layout4  = new QVBoxLayout;

    positionTableHead<<"Должность"<<"Сфера";
    usersTableHead<<"Номер"<<"Имя"<<"Должность"<<"Логин"<<"Пароль";

    button6->setFixedWidth(300);
    button7->setFixedWidth(300);
    back1->setFixedHeight(1000);
    back1->setFixedWidth(650);
    back3->setFixedHeight(1000);
    back3->setFixedWidth(350);
    back4->setFixedHeight(50);
    back4->setFixedWidth(1000);
    usersTableView->setFixedHeight(800);
    usersTableView->setFixedWidth(600);
    usersTableView->setTextElideMode(Qt::ElideRight);
    positionTableView->setFixedHeight(800);
    positionTableView->setTextElideMode(Qt::ElideRight);
    usersOperTableView->setFixedHeight(800);
    usersOperTableView->setFixedWidth(850);
    back2->setFixedWidth(1000);
    back2->setFixedHeight(1000);
    label1->setFixedHeight(20);
    label2->setFixedHeight(20);
    label4->setFixedHeight(20);

    positionTableView->setModel(updateSomeView("select * from position_list",positionTableHead));
    positionTableView->setColumnWidth(0, 150);
    positionTableView->setColumnWidth(1, 150);
    usersTableView->setModel(updateSomeView("select id, name, position, login, password from user_list",usersTableHead));
    usersTableView->setColumnWidth(0, 70);
    usersTableView->setColumnWidth(1, 130);

    positionTableView->setContextMenuPolicy(Qt::NoContextMenu);
    usersTableView->setContextMenuPolicy(Qt::NoContextMenu);
    usersOperTableView->setContextMenuPolicy(Qt::NoContextMenu);
    setContextMenuPolicy(Qt::NoContextMenu);
    layout5->setAlignment(Qt::AlignTop);
    layout5->addWidget(button6);
    layout5->addSpacing(230);
    layout5->addWidget(button7);
    back4->setLayout(layout5);

    layout4->setAlignment(Qt::AlignTop);
    layout4->addWidget(label4);
    layout4->addWidget(positionTableView);
    layout4->addWidget(button2);
    layout4->addWidget(button3);
    back3->setLayout(layout4);

    layout2->setAlignment(Qt::AlignTop);
    layout2->addWidget(label1);
    layout2->addWidget(usersTableView);
    layout2->addWidget(button1);
    layout2->addWidget(button4);
    //layout2->addWidget(button5);
    back1->setLayout(layout2);

    layout3->setAlignment(Qt::AlignTop);
    layout3->addWidget(label2);
    layout3->addWidget(back4);
    layout3->addWidget(usersOperTableView);
    layout3->addWidget(button8);
    back2->setLayout(layout3);

    layout1->setAlignment(Qt::AlignTop);
    layout1->addWidget(back3);
    layout1->addWidget(back1);
    layout1->addWidget(back2);
    setLayout(layout1);

    connect(button1, SIGNAL(clicked()), this, SLOT(callUserDialog()));
    connect(button2, SIGNAL(clicked()), this, SLOT(callAddPositionDialog()));
    connect(button3, SIGNAL(clicked()), this, SLOT(callDeletePositionDialog()));
    connect(button4, SIGNAL(clicked()), this, SLOT(callSelectUserDialog()));
    connect(button6, SIGNAL(clicked()), this, SLOT(showNotSpesInformation()));
    connect(button7, SIGNAL(clicked()), this, SLOT(showSpesInformation()));
    connect(button8, SIGNAL(clicked()), this, SLOT(updateInformationTable()));

}


//виджет со складами и операциями

PaintWidget::PaintWidget(int ,int): QWidget(0)
{
    setBaseAndQuery();
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);

    myQuery.exec("CREATE TABLE stock_list (id_stock int, det_size int, stock_description varchar, personalize  varchar,  x float, y float, PRIMARY KEY(id_stock,det_size))");
    myQuery.exec("CREATE TABLE not_pers_stock_list (id_stock int,stock_description varchar, det_size varchar, amount int, PRIMARY KEY(id_stock,det_size))");
    myQuery.exec("CREATE TABLE pers_stock_list (id_stock int,stock_description varchar, n_ord varchar, amount int)");
    myQuery.exec("CREATE TABLE id_stock_list (id int)");
    myQuery.exec("CREATE TABLE id_stock_list (id int)");
    myQuery.exec("CREATE TABLE id_oper_list (id int)");
    myQuery.exec("CREATE TABLE operation_list (id_operation int, operation_description varchar, id_source_stock int, id_dest_stock  int, x float, y float, PRIMARY KEY(id_operation, id_source_stock ))");
    myQuery.exec("create table userPersSt (id_user int, id_stock int, stock_description varchar)");
    myQuery.exec("create table minimalAmount (amount int)");
    myQuery.exec("select amount from  minimalAmount");
    myQuery.next();
    if(myQuery.size()==0)
        myQuery.exec("insert into minimalAmount (amount) values(0)");
    QWidget     * back1   = new QWidget;
    QWidget     * back2   = new QWidget;
    QWidget     * back3   = new QWidget;
    QWidget     * back4   = new QWidget;

    QLabel      * label1  = new QLabel("СПИСОК СКЛАДОВ");
    QLabel      * label2  = new QLabel("ИНФОРМАЦИЯ О КОНКРЕТНОМ СКЛАДЕ");
    QLabel      * label4  = new QLabel("СПИСОК ОПЕРАЦИЙ");
    QLabel      * label5  = new QLabel("Минимальное число деталей для неперс. склада");
    QLabel      * label6  = new QLabel("Склад № ");

    QPushButton *button1  = new QPushButton("Открыть редактор склада");
    button4               = new QPushButton("Установить");
    QPushButton *button5  = new QPushButton("Изменить содержимое выбранного склада");
    QPushButton *button8  = new QPushButton("Переименовать операцию");   

    QHBoxLayout *layout1  = new QHBoxLayout;
    QHBoxLayout *layout5  = new QHBoxLayout;

    QVBoxLayout *layout2  = new QVBoxLayout;
    QVBoxLayout *layout3  = new QVBoxLayout;
    QVBoxLayout *layout4  = new QVBoxLayout;

    allStockTableView     = new QTableView;
    allOperationTableView = new QTableView;
    selectStockTableView  = new QTableView;
    myStocks              = new QComboBox;
    minAmount             = new QLineEdit;
    ordersWidgetFlag      = false;

    allStockTableView->setContextMenuPolicy(Qt::NoContextMenu);
    allOperationTableView->setContextMenuPolicy(Qt::NoContextMenu);
    setContextMenuPolicy(Qt::NoContextMenu);

    allStockTableView->setTextElideMode(Qt::ElideRight);
    allOperationTableView->setTextElideMode(Qt::ElideRight);
    selectStockTableView->setTextElideMode(Qt::ElideRight);

    setCurrStockList();
    allStockTableHead<<"Номер"<<"Описание"<<"Персонализированный";
    allOperTableHead<<"Номер"<<"Описание"<<"Деталь на вход"<<"Деталь на выход";

    myQuery.exec("select amount from  minimalAmount");
    myQuery.next();
    minAmount->setText(myQuery.value(0).toString());

    int minimalAmount ;
    if(myQuery.size()!=0)
        minimalAmount = myQuery.value(0).toInt();
    else {
        minimalAmount = 0;
    }
    selectStockTableView->setItemDelegate(new NewItemDelegate(minimalAmount,
                                                             selectStockTableView));
    contextMenu    = new QMenu(this);

    deleteAction   = contextMenu->addAction("Удалить детали", this, SLOT(deleteDetail()));
    updateAction   = contextMenu->addAction("Добавить детали", this, SLOT(addDetail()));

    selectStockTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    selectStockTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect (selectStockTableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));
    back1->setFixedHeight(1000);
    back1->setFixedWidth(500);
    back2->setFixedWidth(750);
    back2->setFixedHeight(1000);
    back3->setFixedHeight(1000);
    back3->setFixedWidth(600);
    back4->setFixedHeight(50);
    back4->setFixedWidth(700);

    selectStockTableView->setFixedHeight(800);
    selectStockTableView->setFixedWidth(700);
    allStockTableView->setFixedHeight(800);
    allOperationTableView->setFixedHeight(800);
    allOperationTableView->setFixedWidth(580);

    label1->setFixedHeight(20);
    label2->setFixedHeight(20);
    label4->setFixedHeight(20);
    label5->setFixedHeight(20);
    label5->setFixedWidth(300);
    label6->setFixedWidth(100);
    label6->setFixedHeight(20);
    button4->setFixedWidth(100);

    minAmount->setFixedWidth(50);
    myStocks->setFixedWidth(50);

    layout5->setAlignment(Qt::AlignHorizontal_Mask);
    layout5->addWidget(label6);
    layout5->addWidget(myStocks);
    layout5->addSpacing(50);
    layout5->addWidget(label5);
    layout5->addWidget(minAmount);
    layout5->addWidget(button4);
    back4->setLayout(layout5);

    layout4->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    layout4->addWidget(label1);
    layout4->addWidget(allStockTableView);
    layout4->addWidget(button1);
    back1->setLayout(layout4);

    layout2->setAlignment(Qt::AlignTop);
    layout2->addWidget(label2);
    layout2->addWidget(back4);
    layout2->addWidget(selectStockTableView);
    layout2->addWidget(button5);
    back2->setLayout(layout2);

    layout3->setAlignment(Qt::AlignTop);
    layout3->addWidget(label4);
    layout3->addWidget(allOperationTableView);
    layout3->addWidget(button8);
    back3->setLayout(layout3);

    layout1->setAlignment(Qt::AlignTop);
    layout1->addWidget(back1);
    layout1->addWidget(back2);
    layout1->addWidget(back3);
    setLayout(layout1);

    usedOnSchemeStocks();
    usedOnSchemeOperations();

    connect(button1,SIGNAL(clicked()),this,SLOT(callStockDialog()));
    connect(button8,SIGNAL(clicked()),this,SLOT(callOperationDialog()));
    connect(myStocks,SIGNAL(currentIndexChanged(int)),this,SLOT(showInfAboutStock()));
    connect(button5,SIGNAL(clicked()),this,SLOT(callUpgradeDialog()));
    connect(minAmount,SIGNAL(editingFinished()),this,SLOT(returnValue()));
    connect(button4,SIGNAL(clicked()),this,SLOT(setValue()));

}



//удалить детали

void PaintWidget::deleteDetail()
{
    if(!personalize)
    {
        QModelIndex id1 =  selectStockTableView->model()->index(HighlightIndex.row(), 0);
        QModelIndex id2 =  selectStockTableView->model()->index(HighlightIndex.row(), 2);
        stockerDetDialog *dial = new stockerDetDialog(1, id1.data().toString(),id2.data().toString());
        connect(dial, SIGNAL(callQuery()), this, SLOT(sendQuery()));
        connect(dial, SIGNAL(updateCurrStockTable()), this, SLOT(showInfAboutStock()));

        dial->exec();
    }
    else
    {
        QModelIndex id1 =  selectStockTableView->model()->index(HighlightIndex.row(), 0);
        QModelIndex id2 =  selectStockTableView->model()->index(HighlightIndex.row(), 2);
        QSqlQuery myQuery(dbg);

        myQuery.exec("update  pers_stock_list set amount = 0 where id_stock = "+id1.data().toString()
                              +"  and n_ord = '"+id2.data().toString()+"'");
        myQuery.exec("update user_and_order_list set det_amount = 0 where n_stock = "+id1.data().toString()+"  and n_ord = '"+id2.data().toString()+"'");


        setOpersNotdone(id1.data().toString(),id2.data().toString());
        showInfAboutStock();
    }
}



void  PaintWidget::setOpersNotdone(QString destStockIndex, QString delOrd )
{
    QSqlQuery myQuery(dbg);
    myQuery.exec("select id_source_stock, id_operation from operation_list where id_dest_stock = "+destStockIndex);
    QList<QString> oper_id;
    QList<QString> source_stock;
    while (myQuery.next())
    {
        oper_id<<myQuery.value(1).toString();
        source_stock<<myQuery.value(0).toString();      
    }

    for(int i = 0; i<oper_id.size();++i)
    {
     myQuery.exec("update concr_order_operation_list set state  = 'не выполнена' where operNumber = "+
                           oper_id.at(i)+" and ordNumber = '"+delOrd+"'");
    }
    for(int i = 0; i<source_stock.size();++i)
    {
        myQuery.exec("select id_source_stock, id_operation from operation_list where id_dest_stock = "+source_stock.at(i));
        if(myQuery.size()!=0)
            setOpersNotdone(source_stock.at(i),delOrd);
    }
}
//добавить детали

void  PaintWidget::addDetail()
{
   QModelIndex id1 =  selectStockTableView->model()->index(HighlightIndex.row(), 0);
   QModelIndex id2 =  selectStockTableView->model()->index(HighlightIndex.row(), 2);
   stockerDetDialog *dial = new stockerDetDialog(0, id1.data().toString(),id2.data().toString());
   connect(dial, SIGNAL(callQuery()), this, SLOT(sendQuery()));
   connect(dial, SIGNAL(updateCurrStockTable()), this, SLOT(showInfAboutStock()));

   dial->exec();
}

// внести текущее значение минимально возможного количества деталей в базу


void PaintWidget::setValue()
{
    if(minAmount->text()!="")
    {
        QSqlQuery myQuery(dbg);
        myQuery.exec("delete from minimalAmount ");
        myQuery.exec("insert into minimalAmount (amount) values("+minAmount->text()+")");
        delete selectStockTableView->itemDelegate();
        selectStockTableView->setItemDelegate(new NewItemDelegate(minAmount->text().toInt(),
                                                                 selectStockTableView));

    }

}

// выставить текущее значение минимально возможного количества деталей

void PaintWidget::returnValue()
{

    if(focusWidget ()==button4)
    {
        qDebug()<<"zzzzzzzzzzzzzz";
    }
    else
    {
        QSqlQuery myQuery(dbg);
        myQuery.exec("select amount from  minimalAmount");
        myQuery.next();
        minAmount->setText(myQuery.value(0).toString());
    }
}

//показать информацию по выбранному складу

void PaintWidget::showInfAboutStock()
{
    QSqlQuery myQuery(dbg);

    myQuery.exec("select personalize from stock_list where id_stock = "+myStocks->currentText());
    myQuery.next();
    if(myQuery.value(0).toString()=="нет")
    {
       QList<QString> list;
       list<<"Номер"<<"Описание склада"<<"Размер детали"<<"Количество";
        selectStockTableView->setModel(updateSomeView("select id_stock, stock_description, det_size, amount from  not_pers_stock_list where id_stock = "+myStocks->currentText(),list));
        selectStockTableView->setColumnWidth(0, 100);
        selectStockTableView->setColumnWidth(1, 180);
        selectStockTableView->setColumnWidth(2, 200);
        selectStockTableView->setColumnWidth(3, 200);
        personalize = false;
    }
    else
    {
        QList<QString> list;
        list<<"Номер"<<"Описание склада"<<"Номер заказа"<<"Количество деталей";
         selectStockTableView->setModel(updateSomeView("select id_stock, stock_description, n_ord, amount from  pers_stock_list where id_stock = "+myStocks->currentText(),list));
         selectStockTableView->setColumnWidth(0, 100);
         selectStockTableView->setColumnWidth(1, 180);
         selectStockTableView->setColumnWidth(2, 200);
         selectStockTableView->setColumnWidth(3, 200);
         personalize = true;
    }
}

//выпадающий список складов

void PaintWidget::setCurrStockList()
{
    QSqlQuery myQuery(dbg);
    QList<QString> stocks;
    stocks<<"";
    myQuery.exec(" select id_stock from stock_list  ORDER BY id_stock  ");
    while(myQuery.next())
         stocks<<myQuery.value(0).toString();
    myStocks->clear();
    myStocks->addItems(stocks);
}

//показать используемые в схеме склады

void PaintWidget:: usedOnSchemeStocks()
{
    allStockTableView->setModel(updateSomeView("select id_stock, stock_description, personalize from  stock_list ORDER BY id_stock",allStockTableHead));
    allStockTableView->setColumnWidth(0, 100);
    allStockTableView->setColumnWidth(1, 150);
    allStockTableView->setColumnWidth(2, 200);
}

//показать используемые в схеме операции

void PaintWidget::usedOnSchemeOperations()
{
    allOperationTableView->setModel(updateSomeView("select id_operation, operation_description, (select stock_description from stock_list where id_stock = id_source_stock) as source, (select stock_description from stock_list where id_stock = id_dest_stock) as dest from operation_list ORDER BY id_operation",allOperTableHead));
    allOperationTableView->setColumnWidth(0, 100);
    allOperationTableView->setColumnWidth(1, 150);
    allOperationTableView->setColumnWidth(1, 200);
    allOperationTableView->setColumnWidth(1, 200);
}

//вызов диалога работы с таблицей операций

void PaintWidget::callOperationDialog()
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);
    myQuery.exec("select *from order_list");
    myQuery.next();
    if(myQuery.size()==0)
    {
        operationDialog *dial = new  operationDialog();

        connect(dial, SIGNAL(callQuery()),       this, SLOT(sendQuery()));
        connect(dial, SIGNAL(updateOperTable()), this, SLOT(usedOnSchemeOperations()));
        connect(dial, SIGNAL(updateScheme()), this, SLOT(callUpdateSheme()));
        dial->exec();
    }
    else
    {
        QMessageBox mes;
        mes.setText("невозможно переименовать операцию. еще есть невыполненые заказы");
        mes.exec();
    }

}

//вызов диалога работы с таблицей всех складов

void PaintWidget::callStockDialog()
{
    QSqlQuery myQuery(dbg);
    myQuery.exec("select *from order_list");
    myQuery.next();
    if(myQuery.size()==0)
    {
        stockDialog *dial = new  stockDialog();
        connect(dial, SIGNAL(callQuery()), this, SLOT(sendQuery()));
        connect(dial, SIGNAL(updateAllStockTable()), this, SLOT(usedOnSchemeStocks()));
        connect(dial, SIGNAL(updateScheme()), this, SLOT(updateShemeAfterSaving()));

        dial->exec();
    }
    else
    {
        QMessageBox mes;
        mes.setText("невозможно внести изменения. еще есть невыполненые заказы");
        mes.exec();
    }
}

//вызов диалога работы с текущим складом

void PaintWidget::callUpgradeDialog()
{
    if(myStocks->currentIndex()!=0)
    {
        stockDialog *dial = new  stockDialog(myStocks->currentText().toInt());
        connect(dial, SIGNAL(callQuery()), this, SLOT(sendQuery()));
        connect(dial, SIGNAL(updateAllStockTable()), this, SLOT(usedOnSchemeStocks()));
        connect(dial, SIGNAL(updateScheme()), this, SLOT(updateShemeAfterSaving()));
        connect(dial, SIGNAL(updateCurrStockTable()), this, SLOT(showInfAboutStock()));

        dial->exec();
    }
    else {
        QMessageBox mes;
        mes.setText("Выберите склад");
        mes.exec();
    }
}

//обновить данные в таблице-представлении

QSqlQueryModel * PaintWidget:: updateSomeView(QString command,QList<QString> header)
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

//установить текущий запрос для показания таблицы

void  PaintWidget::setCurrentRequest (QString nsp, QString sp,QList<QString> nsph,QList<QString> sph)
{
    nSpCurrHeader.clear();
    nSpCurrHeader  = nsph;
    SpCurrHeader.clear();
    SpCurrHeader   = sph;
    notSpesCurrReq = nsp;
    spesCurrReq    = sp;

    if(selectedName!=0)
    {
        nSpCurrHeader.clear();
        SpCurrHeader.clear();
        if(!dbg.isOpen())
            dbg.open();
        QSqlQuery myQuery(dbg);
        myQuery.exec("select sphere from user_list where id = "+QString::number(selectedName) );
        myQuery.next();
        QString sphereUser = myQuery.value(0).toString();

        if(sphereUser=="Закупка")
        {
            notSpesCurrReq = "(select name, position  , stock_description, personalize from (select name AS name, position AS position, n_stock as n_stock from user_list, user_and_order_list where user_list.id = "
                    +QString::number(selectedName)
                    +" and user_and_order_list.id = "
                    +QString::number(selectedName)
                    +" group by name, position, n_stock ) as foo,stock_list  where foo.n_stock = stock_list.id_stock)"+
                    " union (select name, position  , stock_description , personalize from (select name AS name, position AS position, id_stock as n_stock  from user_list, userPersSt where user_list.id = "
                    +QString::number(selectedName)
                    +" and userPersSt.id_user = "
                    +QString::number(selectedName)
                    +" group by name, position, id_stock ) as foo1,stock_list  where foo1.n_stock = stock_list.id_stock)";

            spesCurrReq = "select name, stock_desc, personality,n_ord, det_amount from user_and_order_list,user_list where user_list.id = "
                    +QString::number(selectedName)
                    +" and user_and_order_list.id = "
                    +QString::number(selectedName);

            nSpCurrHeader<<"Имя"<<"Должность"<<"Склад"<<"Персонализированный";
            SpCurrHeader<<"Имя"<<"Склад"<<"Персонализированный"<<"Номер Заказа"<<"Количество деталей";

        }
        if(sphereUser=="Выпуск")
        {
            notSpesCurrReq =" select name, position, oper_desc,  order_amount from user_and_oper_list,user_list where user_list.id = user_and_oper_list.id and  user_and_oper_list.id =  "
                    +QString::number(selectedName);

            spesCurrReq = "select name, n_ord, oper_desc, state from user_and_order_list,user_list,concr_order_operation_list where user_list.id = "
                    +QString::number(selectedName)
                    +" and user_and_order_list.id = "
                    +QString::number(selectedName)+
                    " and  concr_order_operation_list.operNumber = user_and_order_list.n_oper and user_and_order_list.n_ord = concr_order_operation_list.ordNumber";

            nSpCurrHeader<<"Имя"<<"Должность"<<"Oперация"<<"Количество заказов";
            SpCurrHeader<<"Имя"<<"Номер Заказа"<<"Операция"<<"Статус";

        }

        if(sphereUser=="Услуги")
        {
            notSpesCurrReq =" select name, position, order_amount from user_and_oper_list,user_list where user_list.id = user_and_oper_list.id and  user_and_oper_list.id =  "
                    +QString::number(selectedName);

            spesCurrReq =  "select user_list.name, n_ord, order_description,startDate, state from user_and_order_list,user_list,order_list where user_list.id = "
                    +QString::number(selectedName)
                    +" and user_and_order_list.id = "
                    +QString::number(selectedName)+
                    " and  order_list.ordNumber = user_and_order_list.n_ord ";


            nSpCurrHeader<<"Имя"<<"Должность"<<"Количество заказов";
            SpCurrHeader<<"Имя"<<"Номер Заказа"<<"Описание заказа"<<"Дата принятия"<<"Статус";

        }

        if(sphereUser=="Доставка")
        {
            notSpesCurrReq =" select name, position, order_amount from user_and_oper_list,user_list where user_list.id = user_and_oper_list.id and  user_and_oper_list.id =  "
                    +QString::number(selectedName);

            spesCurrReq =  "select user_list.name as NAM_1, delivery_list.n_ord as ORD_1,  order_description AS DESCR_1,"
                           " Adress AS ADR_1 ,deliverDate AS DATE_1, deliver_state AS STATE_1 "
                           "from order_list,user_list, delivery_list  where user_list.id = "
                           +QString::number(selectedName)
                           +" and delivery_list.id_deliver = "
                           +QString::number(selectedName)
                           +" and delivery_list.n_ord = order_list.ordNumber group by"
                            " NAM_1, ORD_1, DESCR_1, ADR_1, DATE_1, STATE_1 ";
            myQuery.exec("select n_ord from delivery_list where id_deliver = "
                         +QString::number(selectedName));
            myQuery.next();
            nSpCurrHeader<<"Имя"<<"Должность"<<"Количество заказов";
            if(myQuery.size()!=0)
           {
                //nSpCurrHeader<<"Имя"<<"Должность"<<"Количество заказов";
            SpCurrHeader<<"Имя"<<"Номер Заказа"<<"Описание заказа"<<"Адрес"<<"Дата передачи курьеру"<<"Статус доставки";
            }
            else
            {
                SpCurrHeader = nSpCurrHeader;
                spesCurrReq  = notSpesCurrReq;
            }

        }
    }
     usersOperTableView->setModel(updateSomeView(notSpesCurrReq ,nSpCurrHeader));
    for(int i = 0; i<nSpCurrHeader.size();++i )
        usersOperTableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
}

//показать общую информацию по  выборке

void PaintWidget::showNotSpesInformation()
{
    usersOperTableView->setModel(updateSomeView(notSpesCurrReq,nSpCurrHeader));
    for(int i = 0; i<nSpCurrHeader.size();++i )
        usersOperTableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
}

//показать общую информацию по  выборке

void PaintWidget::showSpesInformation()
{
    usersOperTableView->setModel(updateSomeView(spesCurrReq ,SpCurrHeader));
    for(int i = 0; i<SpCurrHeader.size();++i )
        usersOperTableView->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
}

//обрабока кнопки обновления

void PaintWidget::updateInformationTable()
{
    if(selectedName!=0)
    {
        QList<QString> nsph;
        QList<QString> sph;
        setCurrentRequest ("", "",nsph,sph);
    }
    else
       showNotSpesInformation();
}

//вызов окошка работы с таблицей пользователей

void PaintWidget:: callUserDialog()
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);
    myQuery.exec("select * from position_list");
    QList<QString> positions;
    QList<QString> description;
    positions<<"";

    if(myQuery.size()!=0)
    {
        while(myQuery.next())
        {
            positions<< myQuery.value(0).toString();
            description<<myQuery.value(1).toString();
        }

        usersDialog *dial = new  usersDialog(positions,description);
        connect(dial, SIGNAL(createNewUser(QString, QString, QString, QString, QList<int>)), this, SLOT(addNewUser(QString, QString, QString, QString, QList<int>)));
        connect(dial, SIGNAL(callQuery()), this, SLOT(sendQuery()));
        connect(this, SIGNAL(newQuery(QSqlQuery)), dial, SLOT(setQuery(QSqlQuery)));
        connect(dial, SIGNAL(deleteChosenUser(QString)), this, SLOT(deleteUser(QString)));
        connect(dial, SIGNAL(updateUser(QString, QString, QList<int>)), this, SLOT(setInformationAboutUser(QString, QString, QList<int>)));
        connect(dial, SIGNAL(updateUserTable()), this, SLOT(makeUpdateUserTable()));
        dial->exec();
    }
    else {
        QMessageBox mes;
          mes.setText("невозможно открыть редактор списка пользователей. не задан список должностей");
          mes.exec();
    }
}

//костыль - вызов обновления при изменении данных пользователя в основном списке пользователей

void PaintWidget:: makeUpdateUserTable()
{
    usersTableView->setModel(updateSomeView("select id, name, position, login, password from user_list",usersTableHead));
    usersTableView->setColumnWidth(0, 70);
    usersTableView->setColumnWidth(1, 130);
}

//установка имени пользователя, по которому прроизводится выборка

void PaintWidget:: setSelectedName(int name)
{
    selectedName = name;
}

//вызов окошка выборки

void PaintWidget::callSelectUserDialog()
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);
    myQuery.exec("select * from position_list");
    if(myQuery.size()!=0)
    {
        myQuery.exec("select * from user_list");
        if(myQuery.size()!=0)
        {
            userSelectDialog *dial = new  userSelectDialog();
            connect(dial, SIGNAL(callQuery()), this, SLOT(sendQuery()));
            connect(dial, SIGNAL(sendCommands(QString,QString,QList<QString>,QList<QString>)), this, SLOT(setCurrentRequest(QString, QString,QList<QString>,QList<QString>)));
            connect(dial, SIGNAL(sendName(int)), this, SLOT(setSelectedName(int)));

            dial->exec();
        }
        else
        {
            QMessageBox mes;
            mes.setText("невозможно сделать выборку. не задан список пользователей");
        }
    }
    else
    {
        QMessageBox mes;
        mes.setText("невозможно сделать выборку. не задан список должностей");
        mes.exec();
    }
}

//добавление нового работника

void PaintWidget:: addNewUser(QString name, QString login, QString password, QString position, QList<int> operations)
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);

    //вычисление номера элемента
    QString count;
    myQuery.exec("SELECT COUNT(*) FROM user_list");
    myQuery.next();
    if(myQuery.value(myQuery.record().indexOf("count")).toInt()==0)
        count = QString::number(myQuery.value(myQuery.record().indexOf("count")).toInt()+1);
    else
    {
        myQuery.exec("SELECT max(id) FROM user_list");
        myQuery.next();
        count = QString::number(myQuery.value(0).toInt()+1);
    }
    //конец блока

    QString sphereUser = "select sphere from position_list where position = '"+position+"'";
    myQuery.exec(sphereUser);
    myQuery.next();
    sphereUser = myQuery.value(0).toString();

    QString findSameLogin = "select from user_list where login = '" +login+"'";
    myQuery.exec(findSameLogin);
    //добавление пользователя в список персонала
    if(myQuery.size()==0)
    {
        QString command = "insert into user_list(id, name, position, sphere, login, password) values("+count+",'"
                +name+"','"+position+"','"+sphereUser+"','"
                +login+"','"+password+"')";
        myQuery.exec(command);
        //конец блока

        setInformationAboutUser( count, sphereUser , operations);

        usersTableView->setModel(updateSomeView("select id, name, position, login, password from user_list",usersTableHead));
        usersTableView->setColumnWidth(0, 70);
        usersTableView->setColumnWidth(1, 130);
    }
    else
    {
        QMessageBox message;
        message.setText("Невозможно добавить пользователя. есть пользователь с таким же логином ");
        message.exec();
    }

}

//добавление информации нового работника - все скклады и операции

void PaintWidget:: setInformationAboutUser(QString count, QString sphere , QList<int> operations)
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);

    qDebug()<<myQuery.lastError();
    for(auto i = operations.begin(); i!=operations.end();++i)
    {
        if(sphere=="Выпуск")
        {
            qDebug()<<sphere;
            QString command3 = "select operation_description from operation_list where id_operation = " +QString::number((*i));
            myQuery.exec(command3);
            myQuery.next();
            QString command2 = "insert into user_and_oper_list(id , oper_id, oper_desc,order_amount, stock_amount, room_amount, equipment_amount) values("
                    +count+","+QString::number((*i))+",'"+myQuery.value(0).toString()+"',0,-1,-1,-1)";
            myQuery.exec(command2);
        }
        else
        {

            if(sphere=="Закупка")
            {
                QString command3 = "select stock_description from stock_list where id_stock = " +QString::number((*i));
                myQuery.exec(command3);
                myQuery.next();
                command3 = myQuery.value(0).toString();
                QString command2 = "insert into user_and_oper_list(id , oper_id, oper_desc,order_amount, stock_amount, room_amount, equipment_amount) values("
                        +count+",-1,'',-1,"+QString::number(operations.size())+",-1,-1)";
                myQuery.exec(command2);

                command2 = "select personalize from stock_list where id_stock = " + QString::number((*i));
                myQuery.exec(command2);
                myQuery.next();
                qDebug()<<myQuery.value(0).toString();
                if(myQuery.value(0).toString()=="нет")
                {
                    QString amount = "select sum(amount) from not_pers_stock_list where id_stock = " + QString::number((*i));
                    myQuery.exec(amount);
                    myQuery.next();
                    amount = myQuery.value(0).toString();
                    command2 = "insert into user_and_order_list( id, n_ord, n_oper,oper_desc,n_stock,stock_desc, n_room, n_equipment, personality, det_amount) values("+
                            count+", '',-1,'',"+ QString::number((*i))+",'"+command3+"',-1,-1,'нет',"+amount+")";

                    myQuery.exec(command2);
                    qDebug()<<myQuery.lastError();
                }
                else
                {
                    qDebug()<<"myQuery.lastError();";
                    myQuery.exec("insert into  userPersSt (id_user, id_stock,stock_description) values ("+ count+","+QString::number((*i))+",'"+command3+"')");
                    qDebug()<<myQuery.lastError();
                    QSqlQuery secondQuery(dbg);

                    QString amount = "select n_ord, amount from pers_stock_list where id_stock = " + QString::number((*i));
                    myQuery.exec(amount);
                    while(myQuery.next());
                    {
                        command2 = "insert into user_and_order_list( id, n_ord, n_oper,oper_desc,n_stock,stock_desc, n_room, n_equipment, personality, det_amount) values("+
                                count+", '"+myQuery.value(0).toString()+"',-1,'',"+ QString::number((*i))+",'"+command3+"',-1,-1,'да',"+myQuery.value(1).toString()+")";
                        secondQuery.exec(command2);
                    }

                }
            }


        }
    }
    if(sphere=="Закупка"&&operations.size()==0)
    {
        QString command2 = "insert into user_and_oper_list(id , oper_id, oper_desc,order_amount, stock_amount, room_amount, equipment_amount) values("
                +count+",-1,'',-1,0,-1,-1)";
        myQuery.exec(command2);
    }
    if(sphere=="Услуги")
    {
        QString command2 = "insert into user_and_oper_list(id , oper_id,oper_desc, order_amount, stock_amount, room_amount, equipment_amount) values("
                +count+",-1,'',0,-1,-1,-1)";
        myQuery.exec(command2);
    }
    if(sphere=="Доставка")
    {
        QString command2 = "insert into user_and_oper_list(id , oper_id,oper_desc, order_amount, stock_amount, room_amount, equipment_amount) values("
                +count+",-1,'',0,-1,-1,-1)";
        myQuery.exec(command2);
    }
 }

//удаление пользователя из бд

void PaintWidget::deleteUser(QString id)
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);
    myQuery.exec("select sphere, name from user_list where id = " + id);
    myQuery.next();
    QString sphere = myQuery.value(0).toString();
    QString name   = myQuery.value(1).toString();
    QList<QString> orders;
    myQuery.exec("select n_ord from user_and_order_list where id = " + id+" group by n_ord");
    while(myQuery.next())
        orders<<myQuery.value(0).toString();
    QString command1 = "delete from user_list where id = " + id;
    myQuery.exec(command1);
    command1 = "delete from user_and_oper_list where id = " + id;
    myQuery.exec(command1);
    command1 = "delete from userPersSt where id_user = " + id;
    myQuery.exec(command1);
    myQuery.exec("update user_and_order_list set id = null  where id = " + id);

    if(sphere == "Выпуск")
       {
        myQuery.exec("update user_and_order_list set id = null  where id = " + id);
        myQuery.exec("update concr_order_operation_list  set  worker = null  where  worker = " + id);
    }
    else
    {
        if(sphere == "Услуги")
        {
            for(int i = 0; i<orders.size();++i)
                myQuery.exec("update order_list  set  responsible = 'не определен'  where  ordNumber = '" +orders.at(i)+"'");
            myQuery.exec("update user_and_order_list set id = null  where id = " + id);
        }
        if(sphere == "Закупка")
        {
            myQuery.exec("delete from user_and_order_list where id = " + id);
        }
        if(sphere=="Доставка")
        {

            myQuery.exec("update delivery_list set id_deliver = -1 where id_deliver = "+ id);
        }

    }
    usersTableView->setModel(updateSomeView("select id, name, position, login, password from user_list",usersTableHead));
    usersTableView->setColumnWidth(0, 70);
    usersTableView->setColumnWidth(1, 130);
    /////
    myQuery.exec(command1);
    emit updateOrderList();
}

// вызов окошка добавления новой должности

void PaintWidget::callAddPositionDialog()
{
    sizeDialog *dial = new  sizeDialog();
    connect(dial, SIGNAL(NewPosition(QString,QString)), this, SLOT(addNewPosition(QString,QString )));
    dial->exec();

}

// вызов окошка, в котором удаляется размер

void PaintWidget::callDeletePositionDialog()
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);
    myQuery.exec("select * from position_list");
    QList<QString> sizes;
    sizes<<"";
    if(!myQuery.record().isEmpty())
    {
        while(myQuery.next())
            sizes<< myQuery.value(0).toString();
        sizeDialog *dial = new  sizeDialog( sizes);
        connect(dial, SIGNAL(sendDelPos(QString)), this, SLOT(deletePosition(QString )));
        dial->exec();
    }
    else
    {
        QMessageBox message;
        message.setText("Невозможно удалить должность. ");
        message.exec();
    }
}

//  добавление новой должности

void PaintWidget::addNewPosition(QString str, QString sphere)
{
    if(str!="")
    {
        if(!dbg.isOpen())
            dbg.open();
        QSqlQuery myQuery(dbg);
        QString command = "insert into position_list  (position,sphere) values ('" +str+"','"+ sphere+"')";
        myQuery.exec(command);
        positionTableView->setModel(updateSomeView("select * from position_list",positionTableHead));
        positionTableView->setColumnWidth(0, 150);
        positionTableView->setColumnWidth(1, 150);
    }
}

//  удаление должности

void PaintWidget::deletePosition(QString str)
{
    if(str!="")
    {
        if(!dbg.isOpen())
            dbg.open();
        QSqlQuery myQuery(dbg);
        QString command = "delete from position_list where position = '"+str+"'";
        qDebug()<<myQuery.lastError();
        myQuery.exec(command);
        positionTableView->setModel(updateSomeView("select * from position_list",positionTableHead));
        positionTableView->setColumnWidth(0, 150);
        positionTableView->setColumnWidth(1, 150);

        command ="update user_list set position = 'не определена  ' where position = '" +str+"'";
        myQuery.exec(command);
        usersTableView->setModel(updateSomeView("select id, name, position, login, password from user_list",usersTableHead));
        usersTableView->setColumnWidth(0, 70);
        usersTableView->setColumnWidth(1, 130);
    }

}

//выполнение(внесение в базу данных) команды, пришедшей с графического поля

void PaintWidget::execComandFromGraph(QString command)
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);
    myQuery.exec(command);
    qDebug()<<myQuery.lastError();
}

//установка нового имени продукции

void PaintWidget::setNewName(QString str)
{
    if(str!="")
    {
        if(str!=productionName->text())
        {
            if(!dbg.isOpen())
                dbg.open();
            QSqlQuery myQuery(dbg);
            myQuery.exec("create table name_list (prodName varchar)");
            myQuery.exec("delete from name_list");
            myQuery.exec("create table size_list (prodName varchar,  size varchar,  PRIMARY KEY(prodName,size))");
            myQuery.exec("delete from size_list");

            QString command = "insert into name_list (prodName) values ('" +str+"')";
            myQuery.exec(command);
            productionName->setText(str);
            sizeTableView->setModel(updateSomeView("select * from size_list",sizeTableHead));
            sizeTableView->setColumnWidth(0, 150);
        }
    }
}

//вызов окошка-редактора производственной схемы

void PaintWidget:: callShemeEdit()
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);
    myQuery.exec("select * from size_list");
    if(productionName->text()!="")
    {
        myQuery.exec("select * from order_list");
        myQuery.next();
        if(myQuery.size()==0)
        {
            graphicEdit   * editWindow = new graphicEdit();
            editWindow->setWindowTitle("Редактор производственной схемы");
            editWindow->show();
            connect(editWindow, SIGNAL(updateItNow()), this, SLOT(updateShemeAfterSaving()));
        }
        else
        {
            QMessageBox message;
            message.setText("Невозможно открыть редактор. Есть невыполненные заказы");
            message.exec();
        }
    }
    else
    {
        QMessageBox message;
        message.setText("Невозможно открыть редактор. Не установлено название продукции");
        message.exec();
    }
}

//установка размера

void PaintWidget::addNewSize(QString str)
{
    if(str!="")
    {
        if(!dbg.isOpen())
            dbg.open();
        QSqlQuery myQuery(dbg);
        QSqlQuery myQuery1(dbg);
        QString command = "insert into size_list  (prodName,size) values ('" +productionName->text()+"','"+str+ "')";
        myQuery.exec(command);
        sizeTableView->setModel(updateSomeView("select * from size_list",sizeTableHead));
        sizeTableView->setColumnWidth(0, 150);
        shemeBrowseWidg->menuSave();
        myQuery1.exec("select id_stock,stock_description from not_pers_stock_list");
        while (myQuery1.next())
        {
            command = "insert into not_pers_stock_list (id_stock,stock_description, det_size, amount ) values (" +myQuery1.value(0).toString()+",'"+myQuery1.value(1).toString()+"',"+str+ ",0)";
            myQuery.exec(command);
        }

    }
}

// для загрузки графики передается сцене (работает внутри метода browesheme

void  PaintWidget::sendQuery()
{
    //if(!dbg.isOpen())
    dbg.close();
        dbg.open();
    QSqlQuery myQuery(dbg);
    emit newQuery(myQuery);
}

// вызов окошка, в котором изменяется название продукции

void PaintWidget::callNameDialog()
{
    QSqlQuery myQuery(dbg);
    myQuery.exec("select * from order_list");
    myQuery.next();
    if(myQuery.size()==0)
    {
        nameDialog *dial = new nameDialog(productionName->text());
        connect(dial, SIGNAL(setNewName(QString)), this, SLOT(setNewName(QString )));
        dial->exec();
    }
    else
    {
        QMessageBox mes;
        mes.setText("невозможно переименовать продукцию. есть еще не выполненные заказы");
        mes.exec();
    }
}

//изменение таблицы размеров при удалении одного размера

void PaintWidget::deleteSize(QString str)
{
    if(str!="")
    {
        if(!dbg.isOpen())
            dbg.open();
        QSqlQuery myQuery(dbg);

        myQuery.exec("select ordNumber from order_list where size  = '"+str+"'");
        if(myQuery.size()==0)
        {
            QString command = "delete from size_list where size = '"+str+"'";
            myQuery.exec(command);
            sizeTableView->setModel(updateSomeView("select * from size_list",sizeTableHead));
            sizeTableView->setColumnWidth(0, 150);
            myQuery.exec("delete from not_pers_stock_list where det_size = '" + str+"'");

        }
        else
        {
            QMessageBox message;
            message.setText("Невозможно удалить размер. продукция такого размера в процессе производства");
            message.exec();
        }
    }
}

// вызов окошка, в котором удаляется размер

void PaintWidget::callDeleteSizeDialog()
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);
    myQuery.exec("select * from size_list");
    QList<QString> sizes;
    sizes<<"";
    if(myQuery.size()!=0)
    {
        if(productionName->text()!="")
        {
            while(myQuery.next())
            {
                sizes<< myQuery.value(1).toString();
            }
            sizeDialog *dial = new  sizeDialog(productionName->text(), sizes);
            connect(dial, SIGNAL(sendDelSize(QString)), this, SLOT(deleteSize(QString )));
            dial->exec();
        }
        else
        {
            QMessageBox mes;
            mes.setText("не возможно удалить размер. не установлено имя продукции");
            mes.exec();
        }
    }
    else
    {
        QMessageBox mes;
        mes.setText("не возможно удалить размер. таблица пуста");
        mes.exec();
    }
}

// вызов окошка, в котором добавляется размер

void PaintWidget::callAddSizeDialog()
{
    if(productionName->text()!="")
    {
        sizeDialog *dial = new  sizeDialog(productionName->text());
        connect(dial, SIGNAL(setNewSize(QString)), this, SLOT(addNewSize(QString )));
        dial->exec();
    }
    else
    {
        QMessageBox message;
        message.setText("Невозможно добавить новый размер. Не установлено название продукции");
        message.exec();
    }
}

//пересылка сигнала меню\кнопки в выпадающем окне графиеского редактора

void PaintWidget::clearSceneFromMenu()
{
    emit clearSignal();
}

//пересылка сигнала меню\кнопки в выпадающем окне графиеского редактора

void PaintWidget::saveSceneFromMenu()
{
    emit saveSignal();
}

//пересылка сигнала меню\кнопки в выпадающем окне графиеского редактора

void PaintWidget::addStock()
{
    emit stockSignal();
}

//пересылка сигнала меню\кнопки в выпадающем окне графиеского редактора

void PaintWidget::addOperation()
{
    emit operationSignal();
}

//обновить одну из таблиц


void  PaintWidget::sendUpdateSignal()
{
    emit updating();
}

//обновить вид схемы после сохранения

void PaintWidget::updateShemeAfterSaving()
{
    shemeBrowseWidg->menuClearScene();
    shemeBrowseWidg->browseScheme();
    emit updateStockList();
}

//вызвать обновление схемы после сохранения

void PaintWidget::callUpdateSheme()
{
    emit updateScheme();
}
