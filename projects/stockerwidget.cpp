#include "stockerwidget.h"

stockerWidget::stockerWidget(QString idW)
{


    setFixedHeight(1000);
    setFixedWidth(1050);
    if(!dbg.isOpen())
        dbg.open();
    setStyleSheet("stockerWidget{background-color: #E6E6FA;};");
    setWindowTitle("Производство - Кладовщик");
    id = idW;
    setBase();
    QSqlQuery myQuery(dbg);


    QWidget     * back1   = new QWidget;
    QWidget     * back2   = new QWidget;
    QWidget     * back3   = new QWidget;
    QWidget     * back4   = new QWidget;

    QLabel      * label1  = new QLabel("СПИСОК СКЛАДОВ");
    QLabel      * label2  = new QLabel("ИНФОРМАЦИЯ О КОНКРЕТНОМ СКЛАДЕ");
    QLabel      * label4  = new QLabel("СКЛАДЫ С НЕДОСТАТКОМ ДЕТАЛЕЙ");
    QLabel      * label6  = new QLabel("Склад № ");

    QPushButton *button5  = new QPushButton("Изменить содержимое выбранного склада");
    QPushButton *button8  = new QPushButton("Обновить информацию");

    QHBoxLayout *layout1  = new QHBoxLayout;
    QHBoxLayout *layout5  = new QHBoxLayout;
    QVBoxLayout *layout2  = new QVBoxLayout;
    QVBoxLayout *layout3  = new QVBoxLayout;
    QVBoxLayout *layout4  = new QVBoxLayout;

    allStockListView     = new QListView;
    byeStockListView     = new QListView;
    selectStockTableView = new QTableView;
    myStocks             = new QComboBox;

    setCurrStockList();
    allStockTableHead<<"Номер"<<"Описание"<<"Персонализированный";
    allOperTableHead<<"Номер"<<"Описание"<<"Деталь на вход"<<"Деталь на выход";

    back1->setFixedHeight(1000);
    back1->setFixedWidth(250);
    back2->setFixedWidth(750);
    back2->setFixedHeight(1000);
    back4->setFixedHeight(50);
    back4->setFixedWidth(720);

    selectStockTableView->setFixedHeight(750);
    selectStockTableView->setFixedWidth(730);
    allStockListView->setFixedHeight(400);
    allStockListView->setFixedWidth(200);
    byeStockListView->setFixedHeight(340);
    byeStockListView->setFixedWidth(200);

    label1->setFixedHeight(20);
    label2->setFixedHeight(20);
    label4->setFixedHeight(20);
    label6->setFixedWidth(100);
    label6->setFixedHeight(20);
    myStocks->setFixedWidth(50);
    button8->setFixedWidth(200);

    layout5->setAlignment(Qt::AlignHorizontal_Mask);
    layout5->addWidget(label6);
    layout5->addWidget(myStocks);
    layout5->addSpacing(600);
    back4->setLayout(layout5);

    layout4->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    layout4->addWidget(label1);
    layout4->addWidget(allStockListView);
    layout4->addWidget(label4);
    layout4->addWidget(byeStockListView);
    layout4->addWidget(button8);
    back1->setLayout(layout4);

    layout2->setAlignment(Qt::AlignTop);
    layout2->addWidget(label2);
    layout2->addWidget(back4);
    layout2->addWidget(selectStockTableView);
    layout2->addWidget(button5);
    back2->setLayout(layout2);
    myQuery.exec("select amount from  minimalAmount");
    myQuery.next();
    minimalAmount = myQuery.value(0).toInt();
    selectStockTableView->setItemDelegate(new MyItemDelegate(minimalAmount,
                                                             selectStockTableView));
    usedOnSchemeStocks();
    stocksWithoutDetail();
    myQuery.exec("select name,position from user_list where id = "+id);
    myQuery.next();
    QLabel  *label10     = new QLabel("ИМЯ  :  ");
    QLabel  *label3      = new QLabel(myQuery.value(0).toString());
    QLabel  *label7      = new QLabel("ДОЛЖНОСТЬ  :  ");
    QLabel  *label5      = new QLabel(myQuery.value(1).toString());
    QWidget *widget1     = new QWidget;
    QHBoxLayout* layout7 = new QHBoxLayout;

    label10->setFixedHeight(50);
    label3->setFixedHeight(50);
    label4->setFixedHeight(50);
    label5->setFixedHeight(50);
    label7->setFixedHeight(50);
    widget1->setFixedHeight(50);

    label10->setFixedWidth(60);
    label3->setFixedWidth(200);
    layout7->addWidget(label10);
    layout7->addWidget(label3);
    layout7->addWidget(label7);
    layout7->addWidget(label5);
    layout7->addSpacing(1430);
    widget1->setLayout(layout7);

    layout1->setAlignment(Qt::AlignTop);
    layout1->addWidget(back1);
    layout1->addWidget(back2);
    back3->setLayout(layout1);
    contextMenu    = new QMenu(this);

    deleteAction   = contextMenu->addAction("Удалить детали", this, SLOT(deleteDetail()));
    addAction     = contextMenu->addAction("Добавить детали", this, SLOT(addDetail()));

    selectStockTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    selectStockTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect (selectStockTableView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenu(const QPoint &)));


    layout3->setAlignment(Qt::AlignTop);
    layout3->addWidget(widget1);
    layout3->addWidget(back3);
    setLayout(layout3);

    connect(button8,SIGNAL(clicked()),this,SLOT(updateInformation()));
    connect(myStocks,SIGNAL(currentIndexChanged(int)),this,SLOT(showInfAboutStock()));
    connect(button5,SIGNAL(clicked()),this,SLOT(callUpgradeDialog()));
    connect(allStockListView,SIGNAL(doubleClicked(QModelIndex )),this,SLOT(showChosen(QModelIndex)));
    connect(byeStockListView,SIGNAL(doubleClicked(QModelIndex )),this,SLOT(showChosen(QModelIndex)));

}


//удалить детали

void stockerWidget::deleteDetail()
{
    if(!personalize)
    {
        QModelIndex id1 =  selectStockTableView->model()->index(HighlightIndex.row(), 0);
        QModelIndex id2 =  selectStockTableView->model()->index(HighlightIndex.row(), 2);
        stockerDetDialog *dial = new stockerDetDialog(1, id1.data().toString(),id2.data().toString());
        connect(dial, SIGNAL(callQuery()), this, SLOT(sendQuery()));
        connect(dial, SIGNAL(updateCurrStockTable()), this, SLOT(updateInformation()));

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
        updateInformation();
    }
}



void stockerWidget::setOpersNotdone(QString destStockIndex, QString delOrd )
{
    QSqlQuery myQuery(dbg);
    myQuery.exec("select id_source_stock, id_operation from operation_list where id_dest_stock = "+destStockIndex);
    QList<QString> oper_id;
    QList<QString> source_stock;
    while (myQuery.next())
    {
        oper_id<<myQuery.value(1).toString();
        qDebug()<<myQuery.value(1).toString();
        source_stock<<myQuery.value(0).toString();
        qDebug()<<myQuery.value(0).toString();
    }
    qDebug()<<"uiuiuiu";
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

void stockerWidget::addDetail()
{
   QModelIndex id1 =  selectStockTableView->model()->index(HighlightIndex.row(), 0);
   QModelIndex id2 =  selectStockTableView->model()->index(HighlightIndex.row(), 2);
   stockerDetDialog *dial = new stockerDetDialog(0, id1.data().toString(),id2.data().toString());
   connect(dial, SIGNAL(callQuery()), this, SLOT(sendQuery()));
   connect(dial, SIGNAL(updateCurrStockTable()), this, SLOT(updateInformation()));

   dial->exec();
}

//вызов контекстного меню

void stockerWidget:: onCustomContextMenu(const QPoint & point)
{
    HighlightIndex = selectStockTableView->indexAt(point);
   // HighlightIndex.column()
    if(selectStockTableView->model()->rowCount()==0)
    {
        deleteAction->setEnabled(false);
        addAction->setEnabled(false);
    }
    else
    {
        if(!personalize)
        {
            deleteAction->setEnabled(true);
            addAction->setEnabled(true);

            QModelIndex id1 =  selectStockTableView->model()->index(HighlightIndex.row(), 3);
            if(id1.data().toInt()==0)
                 deleteAction->setEnabled(false);
        }
        else
        {
            deleteAction->setEnabled(true);
            addAction->setEnabled(false);
            QModelIndex id1 =  selectStockTableView->model()->index(HighlightIndex.row(), 3);
            if(id1.data().toInt()==0)
                 deleteAction->setEnabled(false);
        }
    }
    contextMenu->exec(selectStockTableView->mapToGlobal(point));

}

//обновить вид view

QSqlQueryModel * stockerWidget:: updateSomeView(QString command,QList<QString> header, int idx)
{
    if(!dbg.isOpen())
        dbg.open();
    QSqlQuery myQuery(dbg);
    myQuery.exec(command);
    qDebug()<<myQuery.lastError();
    QSqlQueryModel * model = new QSqlQueryModel ;
    model->setQuery(  myQuery );
    if(myQuery.size()>=1)
    {
        for(int i= 0; i< header.size(); i++)
        {
            const char* s2 = header.at(i).toStdString().c_str();
            model->setHeaderData( i , Qt :: Horizontal , tr(s2 ) );
        }
    }
    qDebug()<<myQuery.lastError();
    if(idx==1)
    model->setData(model->index(0, 0 ),
                           QBrush( Qt::red ), Qt::BackgroundRole);
    return model;
}

//дать query диалогу

void  stockerWidget::sendQuery()
{
    //if(!dbg.isOpen())
    dbg.close();
        dbg.open();
    QSqlQuery myQuery(dbg);
    emit newQuery(myQuery);
}

//установить подключение к базе

void stockerWidget:: setBase()
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


//показать информацию по выбранному складу

void  stockerWidget::showInfAboutStock()
{
    QSqlQuery myQuery(dbg);

    myQuery.exec("select personalize from stock_list where id_stock = "+myStocks->currentText());
    myQuery.next();
    if(myQuery.value(0).toString()=="нет")
    {
        myQuery.exec("drop table tab");
        myQuery.exec("create  table tab as select id_stock as I1, stock_description as I2, det_size as I3, amount as I4,'0' as I5 from  not_pers_stock_list where id_stock = "+myStocks->currentText());
        QList<QString> list;
        list<<"Номер"<<"Описание склада"<<"Размер детали"<<"Количество"<<"Требуемое кол-во";
        selectStockTableView->setModel(updateSomeView("select * from tab",list,1));
        selectStockTableView->setColumnWidth(0, 100);
        selectStockTableView->setColumnWidth(1, 180);
        selectStockTableView->setColumnWidth(2, 180);
        selectStockTableView->setColumnWidth(3, 180);

        int count = selectStockTableView->model()->rowCount();

        for(int i = 0; i< count; i++)
        {
            QModelIndex id1 =  selectStockTableView->model()->index(i, 0);
            QModelIndex id2 =  selectStockTableView->model()->index(i, 2);
            QModelIndex id3 =  selectStockTableView->model()->index(i, 3);
            myQuery.exec("select id_operation from operation_list where id_source_stock = "+id1.data().toString());
            myQuery.next();
            QString findedOpId = myQuery.value(0).toString();
            myQuery.exec(" select resp_id from order_list,(select ordNumber as ord from concr_order_operation_list where operNumber = "
                         +findedOpId+") as foo where foo.ord = order_list.ordNumber and order_list.size = '"+id2.data().toString()+"'");
            myQuery.next();
            int required   = abs(myQuery.size());
            qDebug()<<myQuery.lastError();
            int currAmount = id3.data().toInt();
            myQuery.exec("select amount from  minimalAmount");
            myQuery.next();
            int mustHave = myQuery.value(0).toInt();
            int det = currAmount-required;
            qDebug()<<det;
            if(det<mustHave)
            {
                qDebug()<<"flag";
                myQuery.exec(" update tab set I5 = '"+QString::number(mustHave-det)+"' where I3 = '"+id2.data().toString()+"'");

            }

        }
        myQuery.exec("select amount from  minimalAmount");
        myQuery.next();
        int mustHave = myQuery.value(0).toInt();
        delete selectStockTableView->itemDelegate();
        selectStockTableView->setItemDelegate(new MyItemDelegate(mustHave,
                                                                 selectStockTableView));
         selectStockTableView->setModel(updateSomeView("select * from tab",list,1));
        personalize = false;

    }
    else
    {
        QList<QString> list;
        list<<"Номер"<<"Описание склада"<<"Номер заказа"<<"Количество деталей";
         selectStockTableView->setModel(updateSomeView("select id_stock, stock_description, n_ord, amount from  pers_stock_list where id_stock = "+myStocks->currentText(),list,0));
         selectStockTableView->setColumnWidth(0, 100);
         selectStockTableView->setColumnWidth(1, 200);
         selectStockTableView->setColumnWidth(2, 200);
         selectStockTableView->setColumnWidth(3, 200);
         personalize = true;
    }
}

//выпадающий список складов

void stockerWidget::setCurrStockList()
{
    QSqlQuery myQuery(dbg);
    QList<QString> stocks;
    stocks<<"";

    myQuery.exec("( select id_stock from userPersSt where  id_user = "
                 +id+" ORDER BY id_stock ) union(select n_stock from user_and_order_list where  id = "
                 +id+" and personality='нет' ORDER BY n_stock ) ");
    qDebug()<<myQuery.lastError();
    while(myQuery.next())
         stocks<<myQuery.value(0).toString();
    myStocks->clear();
    myStocks->addItems(stocks);
}

//показать используемые в схеме склады

void stockerWidget:: usedOnSchemeStocks()
{
    allStockListView->setModel(updateSomeView("select  stock_description from  stock_list , (( select id_stock as ID from userPersSt where  id_user = "
                                              +id+" ORDER BY id_stock ) union(select n_stock AS ID from user_and_order_list where  id = "
                                              +id+" and personality='нет' ORDER BY n_stock ) ) AS foo where id_stock =foo.ID",allStockTableHead,0));

}

//список складов, на которых недостаточно деталей

 void stockerWidget:: stocksWithoutDetail()
 {

     byeStockListView->setModel(updateSomeView("select  stock_description from  not_pers_stock_list , ((select n_stock AS ID from user_and_order_list where  id = "
                                               +id+" and personality='нет' ORDER BY n_stock ) ) AS foo where id_stock =foo.ID and  amount<"
                                               +QString::number(minimalAmount)+" group by stock_description",allStockTableHead,0));

 }

 //обновить информацию

 void stockerWidget::updateInformation()
 {
     stocksWithoutDetail();
     showInfAboutStock();
     usedOnSchemeStocks();
 }



 void stockerWidget::callUpgradeDialog()
 {
     if(myStocks->currentIndex()!=0)
     {
         stockDialog *dial = new  stockDialog(myStocks->currentText().toInt());
         connect(dial, SIGNAL(callQuery()), this, SLOT(sendQuery()));
         connect(dial, SIGNAL(updateAllStockTable()), this, SLOT(usedOnSchemeStocks()));
         connect(dial, SIGNAL(updateCurrStockTable()), this, SLOT(updateInformation()));

         dial->exec();
     }
     else {
         QMessageBox mes;
         mes.setText("Выберите склад");
         mes.exec();
     }

 }


 void stockerWidget::showChosen(QModelIndex idx)
 {
     QString tablename;
     tablename = idx.data().toString();
     QSqlQuery myQuery(dbg);
     myQuery.exec("select id_stock from stock_list where stock_description = '"+tablename+"'");
     myQuery.next();
     int index = myStocks->findText(myQuery.value(0).toString());
     myStocks->setCurrentIndex(index);
 }
