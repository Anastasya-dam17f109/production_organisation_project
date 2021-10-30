#ifndef STOCKDIALOG_H
#define STOCKDIALOG_H

#include <QDialog>
#include <QList>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QComboBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QStackedLayout>
#include <QScrollArea>
#include <QCheckBox>
#include <QMessageBox>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>



struct n_p_stock
{
    int idx;
    QList<QString> size;
    QList<QString> amount;

};


struct p_stock
{
    int idx;
    QList<QString> n_ord;
    QList<QString> amount;

};

class stockDialog:public QDialog
{
    Q_OBJECT
public:
//для добавления деталей
    QLineEdit *addStockList;
    QComboBox *addSizes;
    QLineEdit *addCurrAmount;
    QLineEdit *addAmount;

//для удаления деталей
    QLineEdit *delStockList;
    QComboBox *delSizes;
    QLineEdit *delCurrAmount;
    QLineEdit *delAmount;
    QComboBox *delOrd;


    //для апдейта персонализации
    QComboBox *updateStockList; 
    QLineEdit *newNameLine;
    QComboBox *updatePersonalization;


    QWidget* firstPage;
    QWidget* addPage;
    QWidget* deletePage;
    QWidget* updatePage;

    QStackedLayout *stackedLayout;
    QSqlQuery stockDialogQuery;

    QPushButton* nextButton;
    QPushButton* delButton;
    QPushButton* addButton;

    QPushButton* updateButton;

    QList<QString> allStocks;
    QList<QString> allStockIndex;
    QList<QString> allStockPersonal;

    QList<n_p_stock> list_n_p;
    QList<p_stock> list_p;
    int currIdx = 0;
//создание диалога-добавления склада

    stockDialog( int index)
    {

        resize(400,500);
        setWindowTitle("Редактор склада");
        callQuery();
        QList<QString> personalize;

        personalize<<""<<"да"<<"нет";
        allStocks<<"";
        allStockIndex<<"";
        allStockPersonal<<"";

        stockDialogQuery.exec("select * from stock_list order by id_stock");
        while(stockDialogQuery.next())
        {
            allStocks<< stockDialogQuery.value(2).toString();
            allStockIndex<< stockDialogQuery.value(0).toString();
            allStockPersonal<< stockDialogQuery.value(3).toString();
        }

        addStockList  = new QLineEdit;
        addSizes      = new QComboBox;
        addCurrAmount = new QLineEdit;
        addAmount     = new QLineEdit;

        delStockList  = new QLineEdit;
        delSizes      = new QComboBox;
        delCurrAmount = new QLineEdit;
        delAmount     = new QLineEdit;
        delOrd        = new QComboBox;

        createFirstPage(index);
        createAddPage(index);
        createDeletePage(index);
        //createUpdatePage();

        QVBoxLayout *layout = new QVBoxLayout;
        stackedLayout = new QStackedLayout;
        stackedLayout->addWidget(firstPage);
        stackedLayout->addWidget(addPage);
        stackedLayout->addWidget(deletePage);


        layout->addLayout(stackedLayout);
        setLayout(layout);
    }

    stockDialog()
    {

        resize(400,500);
        setWindowTitle("Редактировать склад");
        callQuery();
        QList<QString> personalize;

        personalize<<""<<"да"<<"нет";
        allStocks<<"";
        allStockIndex<<"";
        allStockPersonal<<"";

        stockDialogQuery.exec("select * from stock_list");
        while(stockDialogQuery.next())
        {
            allStocks<< stockDialogQuery.value(2).toString();
            allStockIndex<< stockDialogQuery.value(0).toString();
            allStockPersonal<< stockDialogQuery.value(3).toString();
        }

        updatePersonalization = new QComboBox;
        updateStockList       = new QComboBox;
        updatePersonalization->addItems(personalize);
        updateStockList->addItems(allStocks);

        newNameLine = new QLineEdit;

        createUpdatePage();
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(updatePage);
        setLayout(layout);
    }

    //создание первой страницы

    void createFirstPage( int index)
    {
        firstPage           = new QWidget;
        QWidget * wid       = new QWidget;
        QLabel      * label = new QLabel("ВЫБЕРИТЕ ДЕЙСТВИЕ:");
        QVBoxLayout *layout = new QVBoxLayout;
        QHBoxLayout* lay    = new QHBoxLayout;
        nextButton          = new QPushButton("Далее");
        QRadioButton* b1    = new QRadioButton("Добавить детали на склад");
        QRadioButton* b2    = new QRadioButton("Удалить детали  со склада");


        QButtonGroup* groop = new QButtonGroup();
        nextButton ->setEnabled(false);
        label ->setFixedHeight(20);
        groop->setExclusive(true);
        groop->addButton(b1);
        groop->addButton(b2);

        lay->addSpacing(200);
        lay->addWidget(nextButton);
        wid->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label);
        layout->addSpacing(100);
        layout->addWidget(b1);
        layout->addSpacing(40);
        layout->addWidget(b2);
        layout->addSpacing(40);
        layout->addSpacing(40);
        layout->addSpacing(100);
        layout->addWidget(wid);

        firstPage->setLayout(layout);

        if(allStockPersonal.at(index)=="да")
        {
            b1->setEnabled(false);
        }
        connect(nextButton, SIGNAL(clicked()), this, SLOT(nextPage()));
        connect(groop, SIGNAL(buttonClicked(int )), this, SLOT(setCurrIdx(int )));
    }

    //создание страницы добавления

    void createAddPage(int index)
    {
        addPage              = new QWidget;
        QWidget     * wid1   = new QWidget;
        QLabel      *label1  = new QLabel("СКЛАД");
        QLabel      *label2  = new QLabel("ВЫБЕРИТЕ РАЗМЕР");
        QLabel      *label3  = new QLabel("ТЕКУЩЕЕ КОЛИЧЕСТВО ДЕТАЛЕЙ");
        QLabel      *label4  = new QLabel("КОЛИЧЕСТВО ДЕТАЛЕЙ ДЛЯ ДОБАВЛЕНИЯ");

        QVBoxLayout *layout  = new QVBoxLayout;
        QHBoxLayout* lay     = new QHBoxLayout;
        QPushButton* button1 = new QPushButton("Назад");
        addButton            = new QPushButton("Добавить");

        addButton->setEnabled(false);
        label1->setFixedHeight(20);
        label2->setFixedHeight(20);
        label3->setFixedHeight(20);
        label4->setFixedHeight(20);

        QList<QString> all_sizes;
        all_sizes<<"";
        stockDialogQuery.exec("select size from size_list");
        while (stockDialogQuery.next())
            all_sizes<<stockDialogQuery.value(0).toString();
        n_p_stock item;
        item.idx = index;
        stockDialogQuery.exec("select det_size, amount from not_pers_stock_list where id_stock = "+QString::number(index));
        while (stockDialogQuery.next())
        {
            item.size<<stockDialogQuery.value(0).toString();
            item.amount<<stockDialogQuery.value(1).toString();
        }
        list_n_p.clear();
        list_n_p<<item;

        addStockList->setText( allStocks.at(index));
        addStockList->setReadOnly(true);
        addSizes->addItems(all_sizes);
        addCurrAmount->setReadOnly(true);
        addAmount->setReadOnly(true);
        lay->addWidget(button1);
        lay->addSpacing(200);
        lay->addWidget(addButton);;
        wid1->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addSpacing(50);
        layout->addWidget(label1);
        layout->addSpacing(20);
        layout->addWidget(addStockList);
        layout->addSpacing(20);
        layout->addWidget(label2);
        layout->addSpacing(20);
        layout->addWidget(addSizes);
        layout->addSpacing(20);
        layout->addWidget(label3);
        layout->addSpacing(20);
        layout->addWidget(addCurrAmount);
        layout->addSpacing(20);
        layout->addWidget(label4);
        layout->addSpacing(20);
        layout->addWidget(addAmount);
        layout->addSpacing(20);
        layout->addWidget(wid1);

        connect(button1, SIGNAL(clicked()), this, SLOT(backPage()));
        connect(addButton, SIGNAL(clicked()), this, SLOT(addDetails()));
        connect(addSizes, SIGNAL(currentIndexChanged(int)), this, SLOT(setSize()));
        connect(addAmount, SIGNAL(textChanged(const QString )), this, SLOT(setAddButtEnabled()));

        addPage->setLayout(layout);
    }

    //создания виджета-удаления пользователя

    void createDeletePage(int index)
    {
        deletePage = new QWidget;
        QWidget     * wid1   = new QWidget;
        QPushButton* button1 = new QPushButton("Назад");
        delButton            = new QPushButton("Удалить");
        QHBoxLayout* lay     = new QHBoxLayout;
        QVBoxLayout *layout  = new QVBoxLayout;

        delButton->setEnabled(false);
        lay->addWidget(button1);
        lay->addSpacing(200);
        lay->addWidget(delButton);
        wid1->setLayout(lay);

        if(allStockPersonal.at(index)=="нет")
        {
            QList<QString> all_sizes;
            all_sizes<<"";
            stockDialogQuery.exec("select size from size_list");
            while (stockDialogQuery.next())
                all_sizes<<stockDialogQuery.value(0).toString();
            n_p_stock item;
            item.idx = index;
            stockDialogQuery.exec("select det_size, amount from not_pers_stock_list where id_stock = "+QString::number(index));
            while (stockDialogQuery.next())
            {
                item.size<<stockDialogQuery.value(0).toString();
                item.amount<<stockDialogQuery.value(1).toString();
            }
            list_n_p.clear();
            list_n_p<<item;
            QLabel      *label1  = new QLabel("СКЛАД");
            QLabel      *label2  = new QLabel("ВЫБЕРИТЕ РАЗМЕР");
            QLabel      *label3  = new QLabel("ТЕКУЩЕЕ КОЛИЧЕСТВО ДЕТАЛЕЙ");
            QLabel      *label4  = new QLabel("КОЛИЧЕСТВО ДЕТАЛЕЙ ДЛЯ УДАЛЕНИЯ");

            label1->setFixedHeight(20);
            label2->setFixedHeight(20);
            label3->setFixedHeight(20);
            label4->setFixedHeight(20);

            delStockList->setText(allStocks.at(index));
            delStockList->setReadOnly(true);
            delSizes->addItems(all_sizes);
            delCurrAmount->setReadOnly(true);
            delAmount->setReadOnly(true);

            layout->setAlignment(Qt::AlignTop);
            layout->addWidget(label1);
            layout->addSpacing(50);
            layout->addWidget(label1);
            layout->addSpacing(20);
            layout->addWidget(delStockList);
            layout->addSpacing(20);
            layout->addWidget(label2);
            layout->addSpacing(20);
            layout->addWidget(delSizes);
            layout->addSpacing(20);
            layout->addWidget(label3);
            layout->addSpacing(20);
            layout->addWidget(delCurrAmount);
            layout->addSpacing(20);
            layout->addWidget(label4);
            layout->addSpacing(20);
            layout->addWidget(delAmount);
            layout->addSpacing(20);

        }
        else
        {
            p_stock item;
            item.idx = index;
            stockDialogQuery.exec("select n_ord, amount from pers_stock_list where id_stock = "+QString::number(index));
            while (stockDialogQuery.next())
            {
                item.n_ord<<stockDialogQuery.value(0).toString();
                item.amount<<stockDialogQuery.value(1).toString();
            }
            list_p<<item;

            QLabel      *label1  = new QLabel("СКЛАД");
            QLabel      *label2  = new QLabel("ВЫБЕРИТЕ НОМЕР ЗАКАЗА");

            label1->setFixedHeight(20);
            label2->setFixedHeight(20);

            delStockList->setText(allStocks.at(index));
            delStockList->setReadOnly(true);
            QList<QString> orders;
            orders<<"";
            orders<<item.n_ord;
            delOrd->addItems(orders);

            layout->setAlignment(Qt::AlignTop);
            layout->addWidget(label1);
            layout->addSpacing(100);
            layout->addWidget(label1);
            layout->addSpacing(20);
            layout->addWidget(delStockList);
            layout->addSpacing(20);
            layout->addWidget(label2);
            layout->addSpacing(20);
            layout->addWidget(delOrd);
            layout->addSpacing(20);
            layout->addSpacing(100);

        }

        layout->addWidget(wid1);
        deletePage->setLayout(layout);

        connect(delButton, SIGNAL(clicked()), this, SLOT(deleteDetails()));
        connect(delSizes, SIGNAL(currentIndexChanged(int)), this, SLOT(setSize()));
        connect(delOrd, SIGNAL(currentIndexChanged(int)), this, SLOT(setDelButtEnabled()));
        connect(delAmount, SIGNAL(textChanged(const QString )), this, SLOT(setDelButtEnabled()));

        connect(button1, SIGNAL(clicked()), this, SLOT(backPage()));

    }


    void createUpdatePage()
    {
        updatePage           = new QWidget;
        QHBoxLayout* lay     = new QHBoxLayout;
        QVBoxLayout *layout  = new QVBoxLayout;
        QWidget     * wid1   = new QWidget;

        updateButton         = new QPushButton("Сохранить изменения");
        QLabel      *label1  = new QLabel("ВЫБЕРИТЕ СКЛАД ДЛЯ РЕДАКЦИИ");
        QLabel      *label2  = new QLabel("НАЗВАНИЕ");
        QLabel      *label3  = new QLabel("ПЕРСОНАЛИЗАЦИЯ");

        updatePersonalization->setEnabled(false);
        updateButton->setEnabled(false);
        newNameLine->setReadOnly(true);

        label1->setFixedHeight(20);
        label2->setFixedHeight(20);
        label3->setFixedHeight(20);


        lay->addSpacing(300);
        lay->addWidget(updateButton);;
        wid1->setLayout(lay);


        layout->setAlignment(Qt::AlignTop);
        layout->addSpacing(100);
        layout->addWidget(label1);
        layout->addSpacing(20);
        layout->addWidget(updateStockList);
        layout->addSpacing(20);
        layout->addWidget(label2);
        layout->addSpacing(20);
        layout->addWidget(newNameLine);
        layout->addSpacing(20);
        layout->addWidget(label3);
        layout->addSpacing(20);
        layout->addWidget(updatePersonalization);
        layout->addSpacing(100);
        layout->addWidget(wid1);

        connect(updateStockList, SIGNAL(currentIndexChanged(int)), this, SLOT(setUpdateEnabled(int)));
        connect(updateButton, SIGNAL(clicked()), this, SLOT(setUpdates()));
        updatePage->setLayout(layout);
    }

public slots:

    //присосаться к базе

    void setQuery(QSqlQuery newQuery)
    {
        stockDialogQuery = newQuery;

    }

    //перейти на выбранную вкладку

    void nextPage()
    {
        stackedLayout->setCurrentIndex(-(currIdx)-1);
    }

    //вернуться на вкладку назад

    void backPage()
    {
        stackedLayout->setCurrentIndex(0);

        if(delSizes->count()!=0)
            delSizes->setCurrentIndex(0);
        if(addSizes->count()!=0)
            addSizes->setCurrentIndex(0);

        if(delOrd->count()!=0)
            delOrd->setCurrentIndex(0);

        addCurrAmount->setText("");
        addAmount->setText("");
        delCurrAmount->setText("");
        delAmount->setText("");

    }

    //индекс кнопки выбора действия

    void setCurrIdx(int idx)
    {
        currIdx = idx;
        nextButton ->setEnabled(true);
    }



    //установка размера и информации о нем

    void setSize()
    {
        if(addSizes->currentIndex()!=0||delSizes->currentIndex()!=0)
        {
           addAmount->setReadOnly(false);
           delAmount->setReadOnly(false);
           int idx;
           if(delSizes->currentIndex()!=0)
               idx = list_n_p.at(0).size.indexOf(delSizes->currentText());
           else
               idx = list_n_p.at(0).size.indexOf(addSizes->currentText());
           addCurrAmount->setText(list_n_p.at(0).amount.at(idx));
           delCurrAmount->setText(list_n_p.at(0).amount.at(idx));
        }
        else
        {
            addAmount->setReadOnly(true);
            addAmount->setText("");
            addCurrAmount->setText("");
            delAmount->setReadOnly(true);
            delAmount->setText("");
            delCurrAmount->setText("");
        }
    }

    //сделать доступ к кнопке добавления

    void setAddButtEnabled()
    {
        if(addAmount->text()!="")
             addButton ->setEnabled(true);
        else
         addButton ->setEnabled(false);
    }

    //сделать доступ к кнопке удаления

    void setDelButtEnabled()
    {
        if(delOrd->currentText()!=""&&delOrd->count()!=0)
            delButton ->setEnabled(true);
       else
        delButton ->setEnabled(false);
        if(delAmount->text()!="")
             delButton ->setEnabled(true);
        else
         delButton ->setEnabled(false);


    }

    //добавить детали

    void addDetails()
    {
        stockDialogQuery.exec("update  not_pers_stock_list set amount = "
                              +QString:: number(addAmount->text().toInt() +addCurrAmount->text().toInt())
                              + "where stock_description = '"+addStockList->text()+"'  and det_size = '"
                              +addSizes->currentText()+"'");
//         user_and_order_list( id int , n_ord varchar, n_oper int, oper_desc varchar, n_stock int, stock_desc varchar, , det_amount int, primary key (id,n_ord,n_oper,n_stock,n_room,n_equipment))")
        stockDialogQuery.exec("update user_and_order_list set det_amount = (select sum(amount) from not_pers_stock_list where stock_description = '"+addStockList->text()+"'  )"

                              + "where stock_desc = '"+addStockList->text()+"'  ");
        emit  updateCurrStockTable();
        done(1);
    }

    void deleteDetails()
    {
        if(delOrd->count()==0)
        {
            int amount = -delAmount->text().toInt() +delCurrAmount->text().toInt();
            if(amount<0)
                amount = 0;
            stockDialogQuery.exec("update  not_pers_stock_list set amount = "
                                  +QString:: number(amount)
                                  + "where stock_description = '"+delStockList->text()
                                  +"'  and det_size = '"+delSizes->currentText()+"'");
            stockDialogQuery.exec("update user_and_order_list set det_amount = (select sum(amount) from not_pers_stock_list where stock_description = '"+addStockList->text()+"'  )"

                                  + "where stock_desc = '"+addStockList->text()+"'  ");
        }
        else
        {
            qDebug()<<"ghghghghghghg";
            stockDialogQuery.exec("update  pers_stock_list set amount = 0 where stock_description = '"+delStockList->text()
                                  +"'  and n_ord = '"+delOrd->currentText()+"'");
            stockDialogQuery.exec("update user_and_order_list set det_amount = 0 where stock_desc = '"+addStockList->text()+"'  and n_ord = '"+delOrd->currentText()+"'");
            qDebug()<<stockDialogQuery.lastError();
            stockDialogQuery.exec("select id_stock from stock_list where stock_list.stock_description = '"+delStockList->text()+"'");
            qDebug()<<stockDialogQuery.lastError();
            stockDialogQuery.next();
            QString id_del = stockDialogQuery.value(0).toString();
            setOpersNotdone(id_del,delOrd->currentText());
        }
        emit  updateCurrStockTable();
        done(1);
    }
    //удалить из базы  склад

    void deleteStock()
    {

    }

void setOpersNotdone(QString destStockIndex, QString delOrd )
{
    stockDialogQuery.exec("select id_source_stock, id_operation from operation_list where id_dest_stock = "+destStockIndex);
    QList<QString> oper_id;
    QList<QString> source_stock;
    while (stockDialogQuery.next())
    {
        oper_id<<stockDialogQuery.value(1).toString();
        qDebug()<<stockDialogQuery.value(1).toString();
        source_stock<<stockDialogQuery.value(0).toString();
        qDebug()<<stockDialogQuery.value(0).toString();
    }
    qDebug()<<"uiuiuiu";
    for(int i = 0; i<oper_id.size();++i)
    {
     stockDialogQuery.exec("update concr_order_operation_list set state  = 'не выполнена' where operNumber = "+
                           oper_id.at(i)+" and ordNumber = '"+delOrd+"'");
    }
    for(int i = 0; i<source_stock.size();++i)
    {
        stockDialogQuery.exec("select id_source_stock, id_operation from operation_list where id_dest_stock = "+source_stock.at(i));
        if(stockDialogQuery.size()!=0)
            setOpersNotdone(source_stock.at(i),delOrd);
    }
}
    void setUpdateEnabled(int idx)
    {
        if(idx==0)
        {
            updateButton->setEnabled(false);
            newNameLine->clear();
            newNameLine->setReadOnly(true);

            updatePersonalization->setCurrentIndex(0);
            updatePersonalization->setEnabled(false);
        }
        else
        {
            updateButton->setEnabled(true);
            newNameLine->setReadOnly(false);
            updatePersonalization->setEnabled(true);

            callQuery();

            QString command;
            QString personalize;

            personalize = "select personalize from stock_list where stock_description  = '"+ updateStockList->currentText()+"'";
            stockDialogQuery.exec(personalize);
            stockDialogQuery.next();
            personalize = stockDialogQuery.value(0).toString();


            newNameLine->setText(updateStockList->currentText());
            updatePersonalization->setCurrentIndex(updatePersonalization->findText(personalize));

            }


    }

    void setUpdates()
    {
       callQuery();
       QString id = allStockIndex.at(updateStockList->currentIndex());
       QString oldPers = allStockPersonal.at(updateStockList->currentIndex());
       stockDialogQuery.exec("update stock_list set stock_description = '"+newNameLine->text()+"' where id_stock = "+ id);
       stockDialogQuery.exec("update not_pers_stock_list set stock_description = '"+newNameLine->text()+"' where id_stock = "+ id);
       stockDialogQuery.exec("update pers_stock_list set stock_description = '"+newNameLine->text()+"' where id_stock = "+ id);
       stockDialogQuery.exec("update userPersSt set stock_description = '"+newNameLine->text()+"' where id_stock = "+ id);
       stockDialogQuery.exec("update user_and_order_list set stock_desc = '"+newNameLine->text()+"' where n_stock = "+ id);
       if(oldPers!=updatePersonalization->currentText())
       {
           if(oldPers=="да")
           {

               stockDialogQuery.exec("select id_stock, stock_description from (select id_source_stock as dest from operation_list where id_dest_stock = "
                                     + id+" group by id_source_stock) as foo , stock_list where stock_list.id_stock = foo.dest and stock_list.personalize= 'да' ");
               qDebug()<<stockDialogQuery.lastError();
               stockDialogQuery.next();
               if( stockDialogQuery.size()==0)
               {
                   QList<QString> id_user;
                   QList<QString> det_size;
                   stockDialogQuery.exec("update stock_list set personalize= 'нет' where id_stock = "+ id);
                   stockDialogQuery.exec("delete from pers_stock_list where id_stock = "+ id);
                   stockDialogQuery.exec("delete from user_and_order_list where id_stock = "+ id);
                   stockDialogQuery.exec("select id_user from userPersSt where id_stock = "+ id +" group by id_user");
                   while(stockDialogQuery.next())
                       id_user<< stockDialogQuery.value(0).toString();

                   stockDialogQuery.exec("select size from size_list");
                   while(stockDialogQuery.next())
                       det_size<< stockDialogQuery.value(0).toString();

                   stockDialogQuery.exec("delete from userPersSt where id_stock = "+ id);
                   for(int i = 0; i<det_size.size();++i)
                       stockDialogQuery.exec("INSERT INTO not_pers_stock_list (id_stock,stock_description, det_size, amount) values ("+
                                             id+",'"+newNameLine->text()+"','"+det_size.at(i)+"',0)");
                   for(int i = 0; i<id_user.size();++i)
                       stockDialogQuery.exec("insert into user_and_order_list( id, n_ord, n_oper,oper_desc,n_stock,stock_desc, n_room, n_equipment, personality, det_amount) values("+
                                             id_user.at(i)+", '',-1,'',"+ id+",'"+newNameLine->text()+"',-1,-1,'нет',0)");
               }
               else
               {
                   QMessageBox message;
                   message.setText("Невозможно сделать склад неперсонализированным. Деталь, из которой изготавливают то, что хранится на складе, персонализированная ");
                   message.exec();
               }
           }
           else
           {
               QList<QString> id_user;
               QString id_nextSt;
               QString id_nextDescr;
               stockDialogQuery.exec("update stock_list set personalize= 'да' where id_stock = "+ id);
               stockDialogQuery.exec("select id from user_and_order_list where n_stock = "+ id +" group by id");
               while(stockDialogQuery.next())
                   id_user<< stockDialogQuery.value(0).toString();
               stockDialogQuery.exec("delete from user_and_order_list where n_stock = "+ id);
               stockDialogQuery.exec("delete from not_pers_stock_list where id_stock = "+ id);
               for(int i = 0; i<id_user.size();++i)
                   stockDialogQuery.exec("insert into  userPersSt (id_user, id_stock,stock_description) values ("+id_user.at(i)+","+id+",'"+newNameLine->text()+"')");

               stockDialogQuery.exec("select id_stock, stock_description from (select id_dest_stock as dest from operation_list where id_source_stock = "
                                     + id+" group by id_dest_stock) as foo , stock_list where stock_list.id_stock = foo.dest");
               qDebug()<<stockDialogQuery.lastError();
               stockDialogQuery.next();
               if(stockDialogQuery.size()!=0)
                   setPersByIdx(stockDialogQuery.value(0).toString()
                                , stockDialogQuery.value(1).toString());

           }
       }
       emit updateAllStockTable();
       done(1);

    }

    void setPersByIdx(QString idx, QString name)
    {
        QList<QString> id_user;
        QString id_nextSt;
        QString id_nextDescr;
        stockDialogQuery.exec("update stock_list set personalize= 'да' where id_stock = "+ idx);
        stockDialogQuery.exec("select id from user_and_order_list where n_stock = "+ idx +" group by id");
        while(stockDialogQuery.next())
           id_user<< stockDialogQuery.value(0).toString();
        stockDialogQuery.exec("delete from user_and_order_list where n_stock = "+ idx);
        stockDialogQuery.exec("delete from not_pers_stock_list where id_stock = "+ idx);
        for(int i = 0; i<id_user.size();++i)
            stockDialogQuery.exec("insert into  userPersSt (id_user, id_stock,stock_description) values ("+id_user.at(i)+","+idx+",'"+name+"')");

        stockDialogQuery.exec("select id_stock, stock_description from (select id_dest_stock as dest from operation_list where id_source_stock = "
                              + idx+" group by id_dest_stock) as foo , stock_list where stock_list.id_stock = foo.dest");
         stockDialogQuery.next();
        if(stockDialogQuery.size()!=0)
            setPersByIdx(stockDialogQuery.value(0).toString()
                         , stockDialogQuery.value(1).toString());
    }
signals:
    void callQuery();
    void updateAllStockTable();
    void updateScheme();
    void updateCurrStockTable();
};

#endif // STOCKDIALOG_H
