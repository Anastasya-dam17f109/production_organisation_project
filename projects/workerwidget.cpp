#include "workerwidget.h"

workerWidget::workerWidget(QString idW)
{
    setStyleSheet("workerWidget{background-color: #E6E6FA;};");
    setWindowTitle("Производство - Рабочий");
    id = idW;
    resize(800,1000);
    setBase();
    QSqlQuery myQuery(dbg);
    myQuery.exec("select name,position from user_list where id = "+id);
    myQuery.next();

    QVBoxLayout* layout1 = new QVBoxLayout;
    QHBoxLayout* layout4 = new QHBoxLayout;
    QHBoxLayout* layout5 = new QHBoxLayout;
    QVBoxLayout* layout7 = new QVBoxLayout;

    QPushButton* button2 = new QPushButton;
    button2->setIcon(QIcon(":/new/prefix1/back.png"));

    QPushButton* button3 = new QPushButton;
    button3->setIcon(QIcon(":/new/prefix1/forward1.png"));
    QPushButton* button4 = new QPushButton("Сделать деталь");
    QPushButton* button5 = new QPushButton("Забраковать деталь");

    QWidget *widget1 = new QWidget;
    QWidget *widget2 = new QWidget;
    QWidget *widget3 = new QWidget;

    QLabel *label1 = new QLabel("ИМЯ  :  ");
    QLabel *label2 = new QLabel("МОИ ЗАКАЗЫ  :   ");
    QLabel *label3 = new QLabel(myQuery.value(0).toString());
    QLabel *label4 = new QLabel("ДОЛЖНОСТЬ  :  ");
    QLabel *label5 = new QLabel(myQuery.value(1).toString());

    currentHeader  = new QLabel("                                                ВСЕ ЗАКАЗЫ ");
    ordTableView   = new QTableView;
    operTableView  = new QTableView;
    opers          = new QTreeView;

    widget1->setFixedWidth(800);
    widget1->setFixedHeight(50);
    label1->setFixedHeight(50);
    label3->setFixedHeight(50);
    label4->setFixedHeight(50);
    label5->setFixedHeight(50);

    label1->setFixedWidth(60);
    label3->setFixedWidth(200);
    layout4->addWidget(label1);
    layout4->addWidget(label3);
    layout4->addWidget(label4);
    layout4->addWidget(label5);
    layout4->addSpacing(430);
    widget1->setLayout(layout4);

    widget2->setFixedWidth(800);
    widget2->setFixedHeight(50);
    button2->setFixedWidth(40);
    button3->setFixedWidth(40);
    currentHeader->setFixedWidth(520);
    layout5->addWidget(button2);
    layout5->addWidget(currentHeader);
    layout5->addWidget(button3);
    layout5->addSpacing(30);
    widget2->setLayout(layout5);

    widget3->setFixedWidth(800);
    widget3->setFixedHeight(900);
    ordTableView->setFixedWidth(780);
    layout1->setAlignment(Qt::AlignTop);
    layout1->addWidget(label2);
    layout1->addWidget(widget2);
    layout1->addWidget(ordTableView);
    layout1->addWidget(button4);
    layout1->addWidget(button5);
    widget3->setLayout(layout1);

    ordTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ordTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    contextMenu    = new QMenu(this);
    connect (ordTableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));

    deleteAction   = contextMenu->addAction("Забраковать деталь", this, SLOT(menuRegectOperation()));
    updateAction   = contextMenu->addAction("Сделать деталь", this, SLOT(menuExecuteOperationtion()));


    layout7->setAlignment(Qt::AlignTop);
    layout7->addWidget(widget1);
    layout7->addWidget(widget3);
    updateView();
    setLayout(layout7);

    connect(button3,SIGNAL(clicked()),this,SLOT(nextButton()));
    connect(button2,SIGNAL(clicked()),this,SLOT(backButton()));
    connect(button4,SIGNAL(clicked()),this,SLOT(callExecuteDialog()));
    connect(button5,SIGNAL(clicked()),this,SLOT(callRegectDialog()));
}

//выборка всех возможных к выполнению операций

QString workerWidget::createBigCommand()
{
    QSqlQuery myQuery(dbg);
    QString bigCommand = "";
    QList<QString> operations;
    QList<QString> orders;
    QList<QString> sizes;
    myQuery.exec("select concr_order_operation_list.ordNumber as ORD,concr_order_operation_list.operNumber as OPER ,size as SIZE from concr_order_operation_list,order_list"
                  " where concr_order_operation_list.state = 'не выполнена' and concr_order_operation_list.ordNumber = order_list.ordNumber and worker = "
                       +id);
    while(myQuery.next())
    {
        operations<<myQuery.value(1).toString();
        orders<<myQuery.value(0).toString();
        sizes<<myQuery.value(2).toString();
    }

    for(int i = 0; i<operations.size(); ++i)
    {
        myQuery.exec("select id_source_stock from operation_list where id_operation = "+operations.at(i));
        QList<QString> s_stocks;
        while(myQuery.next())
           s_stocks<<myQuery.value(0).toString();
        int count = 0;

        for(int j =0; j<s_stocks.size();++j  )
         {
            myQuery.exec("select personalize from stock_list where id_stock = "+s_stocks.at(j));
            myQuery.next();
            if(myQuery.value(0).toString()=="нет")
            {
                myQuery.exec("select amount from not_pers_stock_list where id_stock = "+s_stocks.at(j)+ " and det_size = '"+ sizes.at(i)+"'");
                myQuery.next();

                if(myQuery.value(0).toString()!="0")
                    count++;
            }
            else
            {
                myQuery.exec("select amount from pers_stock_list where id_stock = "+s_stocks.at(j)+ " and n_ord = '"+ orders.at(i)+"'");
                myQuery.next();
                if(myQuery.value(0).toString()!="0")
                    count++;
            }
        }
        if(count==s_stocks.size())
        {
            if(bigCommand!="")
                bigCommand+="union";
            bigCommand+="(select '"+orders.at(i)+"' as ORD,operation_description AS DESK, '"+sizes.at(i)+"' AS SIZE,'не выполнена' AS STATE from operation_list where id_operation = "+operations.at(i)+" )";

        }

    }
    return bigCommand;
}


//вызов меню

void workerWidget::onCustomContextMenu(const QPoint & point)
{
    HighlightIndex    = ordTableView->indexAt(point);
    QModelIndex indID = ordTableView->model()->index(HighlightIndex.row(),3);
    if(ordTableView->model()->rowCount()!=0)
    {
        if(indID.data().toString()=="выполнена")
        {
            deleteAction->setEnabled(true);
            updateAction->setEnabled(false);
        }
        else
        {
            deleteAction->setEnabled(false);
            updateAction->setEnabled(true);
        }
    }
    else
    {
        deleteAction->setEnabled(false);
        updateAction->setEnabled(false);
    }
    contextMenu->exec(ordTableView->mapToGlobal(point));

}

//на вкладку вперед

void workerWidget::nextButton()
{
    if(currentPage==0)
    {
        currentPage += 1;
        currentHeader->setText("                                          НЕВЫПОЛНЕННЫЕ ЗАКАЗЫ ");
        updateView();

    }
    else
    {
        if(currentPage==1)
        {
            currentPage += 1;
            currentHeader->setText("                                            ВЫПОЛНЕННЫЕ ЗАКАЗЫ ");
            updateView();
        }
        else
        {

            currentPage = 0;
            currentHeader->setText("                                                ВСЕ ЗАКАЗЫ ");
            updateView();
        }
    }
}

//выполнить операцию

void workerWidget::menuExecuteOperationtion()
{
    QSqlQuery myQuery(dbg);
    QModelIndex indID = ordTableView->model()->index(HighlightIndex.row(),0);
    myQuery.exec("select ORD,operation_description, SIZE from("+createBigCommand()+") as foo6 where foo6.ORD = '"+indID.data().toString()+"'");

    if(myQuery.size()!=0)
    {
        myQuery.exec("select id_operation from operation_list where operation_description = '"
                     +ordTableView->model()->index(HighlightIndex.row(),1).data().toString()+"'");
        myQuery.next();
        executeOperation(indID.data().toString(),myQuery.value(0).toString() );

    }
    else
    {
        QMessageBox message;
        message.setText("Невозможно выполнить. Нет необходимого материала.");
        message.exec();
        updateView();
    }
}


void workerWidget::menuRegectOperation()
{
    QSqlQuery myQuery(dbg);
    QModelIndex indID = ordTableView->model()->index(HighlightIndex.row(),0);
    myQuery.exec("select ORD,operation_description, SIZE from(( select concr_order_operation_list.ordNumber as order,operation_description, size, 'выполнена' from operation_list, order_list,concr_order_operation_list "
                 " where concr_order_operation_list.ordNumber = order_list.ordNumber and concr_order_operation_list.worker = "+id+
             " and concr_order_operation_list.operNumber = operation_list.id_operation and concr_order_operation_list.state = 'выполнена'"
             " group by concr_order_operation_list.ordNumber,operation_description, size )) as foo6 where foo6.ORD = '"+indID.data().toString()+"'");

    if(myQuery.size()!=0)
    {
        myQuery.exec("select id_operation from operation_list where operation_description = '"
                     +ordTableView->model()->index(HighlightIndex.row(),1).data().toString()+"'");
        myQuery.next();
        regectDetail(indID.data().toString(),myQuery.value(0).toString() );

    }
    else
    {
        QMessageBox message;
        message.setText("Невозможно выполнить. Уже забраковали.");
        message.exec();
        updateView();
    }
}
//отправка новой query

void  workerWidget::sendQuery()
{
    //if(!dbg.isOpen())
    dbg.close();
        dbg.open();
    QSqlQuery myQuery(dbg);
    emit newQuery(myQuery);
}

//вызов диалога-выполнения операции

void workerWidget::callExecuteDialog()
{
    executeOperDialog* dial = new executeOperDialog(id,"выполнить");

    connect(dial,SIGNAL(callQuery()),this,SLOT(sendQuery()));
    connect(this,SIGNAL(newQuery(QSqlQuery )),dial,SLOT(setQuery(QSqlQuery)));
    connect(dial,SIGNAL(executeIt(QString, QString)),this,SLOT(executeOperation(QString,QString)));

    dial->exec();
}

//вызов диалога-отмены операции

void workerWidget::callRegectDialog()
{
    executeOperDialog* dial = new executeOperDialog(id,"отменить");

    connect(dial,SIGNAL(callQuery()),this,SLOT(sendQuery()));
    connect(this,SIGNAL(newQuery(QSqlQuery )),dial,SLOT(setQuery(QSqlQuery)));
    connect(dial,SIGNAL(regectIt(QString, QString)),this,SLOT(regectDetail(QString,QString)));

    dial->exec();
}

//непосредственно выполнение

void workerWidget::executeOperation(QString ordNumber,QString operNumber )
{
    QSqlQuery myQuery(dbg);
   // QModelIndex indID = ordTableView->model()->index(HighlightIndex.row(),0);
    QString size = "";
    QString destStock = "";
    QList<QString> source_stocks;
    QString next_stock = "";

    myQuery.exec(" select id_source_stock from operation_list where id_operation = "+operNumber);
    while(myQuery.next())
    source_stocks<<myQuery.value(0).toString();

    myQuery.exec(" select id_dest_stock from operation_list where id_operation = "+operNumber+" group by id_dest_stock");
    myQuery.next();
    destStock = myQuery.value(0).toString();

    myQuery.exec(" select id_dest_stock from operation_list where  id_source_stock = "+destStock);
    myQuery.next();

    next_stock = myQuery.value(0).toString();

    myQuery.exec(" select size from order_list where ordNumber = '"+ordNumber+"'");
    myQuery.next();
    size = myQuery.value(0).toString();

    for(int i =0; i< source_stocks.size(); ++i)
    {
        myQuery.exec("select personalize from stock_list where id_stock = "
                     +source_stocks.at(i));
        myQuery.next();
        if(myQuery.value(0).toString()=="нет")
        {
            myQuery.exec("select amount from not_pers_stock_list where id_stock = "
                         +source_stocks.at(i)+ " and det_size = '"+ size+"'");
            myQuery.next();
            int amount = myQuery.value(0).toInt();
            myQuery.exec("update not_pers_stock_list set amount = "
                         + QString::number(amount-1) +" where id_stock = "+source_stocks.at(i)+ " and det_size = '"+ size+"'");

        }
        else
        {
            myQuery.exec("select amount from pers_stock_list where id_stock = "+source_stocks.at(i)+ " and n_ord = '"+ ordNumber+"'");
            myQuery.next();
            int amount = myQuery.value(0).toInt();
            myQuery.exec("update pers_stock_list set amount = "
                         + QString::number(amount-1) +" where id_stock = "+source_stocks.at(i)+ " and n_ord = '"+ ordNumber+"'");
        }
    }

    myQuery.exec("select personalize from stock_list where id_stock = "
                 +destStock);
    myQuery.next();
    if(myQuery.value(0).toString()=="нет")
    {
        myQuery.exec("select amount from not_pers_stock_list where id_stock = "
                     +destStock+ " and det_size = '"+ size+"'");
        myQuery.next();
        int amount = myQuery.value(0).toInt();
        myQuery.exec("update not_pers_stock_list set amount = "
                     + QString::number(amount+1) +" where id_stock = "+destStock+ " and det_size = '"+ size+"'");

    }
    else
    {
        myQuery.exec("select amount from pers_stock_list where id_stock = "+destStock+ " and n_ord = '"+ ordNumber+"'");
        myQuery.next();
        int amount = myQuery.value(0).toInt();
        myQuery.exec("update pers_stock_list set amount = "
                     + QString::number(amount+1) +" where id_stock = "+destStock+ " and n_ord = '"+ ordNumber+"'");

    }

    myQuery.exec("update concr_order_operation_list set state = 'выполнена' where operNumber = "+operNumber
                  +" and ordNumber = '"+ ordNumber+"'");
    myQuery.exec("select operNumber from  concr_order_operation_list where state = 'не выполнена' and  ordNumber = '"+ ordNumber+"'");
    myQuery.next();

    if(myQuery.value(0).toString() == "")
    {
        myQuery.exec("update order_list set state = 'готов' where  ordNumber = '"+ ordNumber+"'");
        myQuery.exec("update order_list set endDate = NOW() where  ordNumber = '"+ ordNumber+"'");
    }
    if(next_stock == "")
    {
        myQuery.exec("update order_list set build = 'собран' where  ordNumber = '"+ ordNumber+"'");
        myQuery.exec("update order_list set deliverDate = NOW() where  ordNumber = '"+ ordNumber+"'");

    }
    myQuery.exec("select  build  from order_list where  ordNumber = '"+ ordNumber+"'");
    myQuery.next();

    if(myQuery.value(0).toString()== "собран")
    {
        myQuery.exec("select id_deliver from delivery_list where n_ord = '"+ ordNumber+"'");
        myQuery.next();
        if(myQuery.size()==0)
        {
            qDebug()<<"im here";
        myQuery.exec("select Id from (select id as Id, order_amount as amount from user_and_oper_list where"
                     " id IN (select id as ID from user_list where sphere = 'Доставка' ) )as foo1 "
                     "where foo1.amount = (select min(amount) from (select id as Id, order_amount as  "
                     "amount from user_and_oper_list  where id IN (select id as ID from user_list where sphere = 'Доставка') ) as foo1)");
        myQuery.next();

        QString responsible;
        if(myQuery.size()!=0)
            responsible = myQuery.value(0).toString();
        else
            responsible = "-1";

        myQuery.exec("insert into delivery_list(id_deliver, n_ord) values ("+ responsible+",'"+ordNumber+"')");
        if(responsible != "-1")
        {
            myQuery.exec("select  order_amount from user_and_oper_list where id =  " + responsible);
            myQuery.next();
            QString curOrdAmount = myQuery.value(0).toString();
            myQuery.exec("update user_and_oper_list set order_amount = "
                                  +QString::number(curOrdAmount.toInt() +1)+" where id =  " + responsible);
        }
        }
    }
    updateView();
}

//забраковать деталь

void workerWidget::regectDetail(QString ordNumber,QString operNumber)
{
    QSqlQuery myQuery(dbg);
    myQuery.exec("select id_dest_stock from operation_list where id_operation = "+ operNumber);
    myQuery.next();
    QString destStock = myQuery.value(0).toString();

    myQuery.exec(" select size from order_list where ordNumber = '"+ordNumber+"'");
    myQuery.next();
    QString size = myQuery.value(0).toString();



    myQuery.exec("select personalize from stock_list where id_stock = "
                 +destStock);
    myQuery.next();

    if(myQuery.value(0).toString()=="нет")
    {
        myQuery.exec("select amount from not_pers_stock_list where id_stock = "
                     +destStock+ " and det_size = '"+ size+"'");
        myQuery.next();

        int amount = myQuery.value(0).toInt();
         qDebug()<<amount;
         if(amount!=0)
         {
             myQuery.exec("update not_pers_stock_list set amount = "
                          + QString::number(amount-1) +" where id_stock = "+destStock+ " and det_size = '"+ size+"'");
             myQuery.exec("select amount from not_pers_stock_list where id_stock = "
                          +destStock+ " and det_size = '"+ size+"'");
             myQuery.next();
             qDebug()<<myQuery.value(0).toInt();
             setOpersNotdone(destStock, ordNumber);
             //myQuery.exec("update order_list set build = 'не собран' where  ordNumber = '"+ ordNumber+"'");
             myQuery.exec("update order_list set state = 'не готов' where  ordNumber = '"+ ordNumber+"'");
             //типа если неперсонализированная , то заказ может быть выполнен
//             myQuery.exec("update order_list set deliver_state = 'не доставлен' where  ordNumber = '"+ ordNumber+"'");


//             myQuery.exec("select id_deliver from delivery_list where n_ord ='"+ordNumber+"'");
//             myQuery.next();
//             if(myQuery.size()!=0)
//             {
//                 QString responsible = myQuery.value(0).toString();
//                 if(responsible != "-1")
//                 {
//                     myQuery.exec("select  order_amount from user_and_oper_list where id =  " + responsible);
//                     myQuery.next();
//                     QString curOrdAmount = myQuery.value(0).toString();
//                     myQuery.exec("update user_and_oper_list set order_amount = "
//                                  +QString::number(curOrdAmount.toInt() -1)+" where id =  " + responsible);
//                 }
//                 myQuery.exec("delete from delivery_list where where n_ord ='"+ordNumber+"'");
//             }
         }
         else
         {
             QMessageBox mes;
             mes.setText("невозможно забраковать деталь. она уже ушла дальше по производству.");
             mes.exec();
         }
    }
    else
    {
        myQuery.exec("select amount from pers_stock_list where id_stock = "+destStock+ " and n_ord = '"+ ordNumber+"'");
        myQuery.next();
        int amount = myQuery.value(0).toInt();
        if(amount!=0)
        {
            myQuery.exec("update pers_stock_list set amount = "
                     + QString::number(amount-1) +" where id_stock = "+destStock+ " and n_ord = '"+ ordNumber+"'");
            setOpersNotdone(destStock, ordNumber);
            myQuery.exec("update order_list set build = 'не собран' where  ordNumber = '"+ ordNumber+"'");
            myQuery.exec("update order_list set state = 'не готов' where  ordNumber = '"+ ordNumber+"'");

            myQuery.exec("update order_list set deliver_state = 'не доставлен' where  ordNumber = '"+ ordNumber+"'");


            myQuery.exec("select id_deliver from delivery_list where n_ord ='"+ordNumber+"'");
            myQuery.next();
            if(myQuery.size()!=0)
            {
                QString responsible = myQuery.value(0).toString();
                if(responsible != "-1")
                {
                    myQuery.exec("select  order_amount from user_and_oper_list where id =  " + responsible);
                    myQuery.next();
                    QString curOrdAmount = myQuery.value(0).toString();
                    myQuery.exec("update user_and_oper_list set order_amount = "
                                          +QString::number(curOrdAmount.toInt() -1)+" where id =  " + responsible);
                }
                myQuery.exec("delete from delivery_list where where n_ord ='"+ordNumber+"'");
            }
        }
        else
        {
            QMessageBox mes;
            mes.setText("невозможно забраковать деталь. она уже ушла дальше по производству.");
            mes.exec();
        }

    }



    updateView();
}


void workerWidget::setOpersNotdone(QString destStockIndex, QString delOrd )
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

//на вкладку назад

void workerWidget::backButton()
{
    if(currentPage==0)
    {
        currentPage = 2;
        currentHeader->setText("                                            ВЫПОЛНЕННЫЕ ЗАКАЗЫ ");

        updateView();

    }
    else
    {
        if(currentPage==2)
        {
            currentPage = 1;
            currentHeader->setText("                                          НЕВЫПОЛНЕННЫЕ ЗАКАЗЫ ");
            updateView();
        }
        else
        {

            currentPage = 0;
            currentHeader->setText("                                                ВСЕ ЗАКАЗЫ ");
            updateView();
        }
    }
}


//подключение к базе

void workerWidget:: setBase()
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

//обновить вид содержимого таблицы

void workerWidget::updateView()
{
    QSqlQuery myQuery(dbg);
    if(currentPage==0)
    {
        QString bigCommand = "(select ORD, DESK, SIZE,STATE from ("+createBigCommand() + ") as foo  group by ORD, DESK, SIZE,STATE)";
        myQuery.exec(bigCommand);
        myQuery.next();

        if(createBigCommand()!="")
        {
            if(bigCommand!="")
                bigCommand += " union";
        }
        else
        {
            bigCommand = "";
        }
        bigCommand+=" ( select concr_order_operation_list.ordNumber as order,operation_description, size, 'выполнена' from operation_list, order_list,concr_order_operation_list "
                    " where concr_order_operation_list.ordNumber = order_list.ordNumber and concr_order_operation_list.worker = "+id+
                " and concr_order_operation_list.operNumber = operation_list.id_operation and concr_order_operation_list.state = 'выполнена'"
                " group by concr_order_operation_list.ordNumber,operation_description, size )";
        myQuery.exec(bigCommand);
    }
    if(currentPage==1)
        myQuery.exec("(select ORD, DESK, SIZE,STATE from ("+createBigCommand() + ") as foo  group by ORD, DESK, SIZE,STATE)");

    if(currentPage==2)
        myQuery.exec(
                    "( select concr_order_operation_list.ordNumber as order,operation_description, size, 'выполнена' from operation_list, order_list,concr_order_operation_list "
                    " where concr_order_operation_list.ordNumber = order_list.ordNumber and concr_order_operation_list.worker = "+id+
                    " and concr_order_operation_list.operNumber = operation_list.id_operation and concr_order_operation_list.state = 'выполнена'"
                    " group by concr_order_operation_list.ordNumber,operation_description, size )");

    QSqlQueryModel * model = new QSqlQueryModel ;
    model->setQuery(myQuery);
    QList<QString> header;
    header<<"Номер заказа"<<"Операция"<<"Размер"<<"Статус операции";

    for(int i= 0; i< header.size(); i++)
    {
        const char* s2 = header.at(i).toStdString().c_str();
        model->setHeaderData( i , Qt :: Horizontal , tr(s2 ) );
    }

    ordTableView->setModel(model);
    ordTableView->setColumnWidth(1,260);
    ordTableView->setColumnWidth(0,200);
    ordTableView->setColumnWidth(2,160);
}











QStandardItemModel* workerWidget::setTreeModel()
{
    QSqlQuery myQuery(dbg);
    QList<QString>  myOpers;
    QStandardItemModel* model = new  QStandardItemModel(0,1);
    // user_and_oper_list(id int , oper_id int, oper_desc varchar, order_amount int, stock_amount int, room_amount
    myQuery.exec("select oper_id from user_and_oper_list where id = "+ id+" group by oper_id");
    while(myQuery.next())
        myOpers<<myQuery.value(0).toString();
    for(int i  =0; i<  myOpers.size(); ++i )
    {
        myQuery.exec("select operation_description from operation_list where id_operation = "+ myOpers.at(i));
        myQuery.next();
        string name = myQuery.value(0).toString().toLocal8Bit().constData();
        QString naming( QTextCodec::codecForName("windows-1251")-> toUnicode(name.c_str()));
        model->appendRow(new QStandardItem(naming));
    }

    for(int i = 0; i<myOpers.size(); ++i)
    {
        QModelIndex index =  model->index ( i , 0) ;
        model->insertRows(0,2,index);
        model->insertColumns(0,1,index);
        model ->setData(model ->index( 0, 0, index),"Детали на вход");
        model ->setData(model ->index( 1, 0, index),"Деталь на выход");
        QModelIndex index1 = model->index(0,0,index);
        QList<QString> stocks;
        myQuery.exec("select stock_list.stock_description from stock_list, (select id_source_stock as source from operation_list where id_operation ="
                     + myOpers.at(i)+ " group by id_source_stock ) as foo  where stock_list.id_stock = foo.source" );
        while(myQuery.next())
            stocks<<myQuery.value(0).toString();
        model->insertRows(0,stocks.size(),index1);
        model->insertColumns(0,1,index1);
        model -> insertColumns (1 , 1 , index1 );
        for(int j = 0; j<stocks.size();++j)
            model ->setData(model ->index( j, 0, index1 ),stocks.at(j));
        myQuery.exec("select stock_list.stock_description from stock_list, (select id_dest_stock as source from operation_list where id_operation ="
                     + myOpers.at(i)+ " group by id_dest_stock ) as foo  where stock_list.id_stock = foo.source" );
        myQuery.next();

        QModelIndex index2 = model->index(1,0,index);
        model->insertRows(0,1,index2);
        model->insertColumns(0,1,index2);
        model -> insertColumns (1 , 1 , index2 );
        model ->setData(model ->index( 0, 0, index2 ), myQuery.value(0).toString());

    }
    return model;
}
