#ifndef ADDORDERDIALOG_H
#define ADDORDERDIALOG_H

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


class addOrderDialog:public QDialog
{
    Q_OBJECT
public:

    QSqlQuery orderDialogQuery;

    QComboBox *sizes;
    QComboBox *packs;
    QLineEdit *description;
    QLineEdit *adress;
    QLineEdit *LogServise;
    QLineEdit *name;
    QLineEdit *phone;
    QLineEdit *TechMak;
    QString respId;

    addOrderDialog(QString id)
    {
//        (ordNumber varchar, responsible varchar, order_description varchar,
//                size int, Adress varchar, LogServise varchar, Name_Phone varchar, TechMak varchar, startDate datetime,
//                endDate datetime, pack varchar, state varchar, PRIMARY KEY(OrdNumber))");
        respId = id;
        resize(400,500);
        setWindowTitle("Добавить заказ");
        callQuery();
        QVBoxLayout * layout = new QVBoxLayout;
        QList<QString> all_sizes;
        QList<QString> pack;
        pack<<""<<"да"<<"нет";
        all_sizes<<"";
        orderDialogQuery.exec("select size from size_list");
        while (orderDialogQuery.next())
            all_sizes<<orderDialogQuery.value(0).toString();

        sizes       = new QComboBox;
        packs       = new QComboBox;
        description = new QLineEdit;
        adress      = new QLineEdit;
        LogServise  = new QLineEdit;
        name        = new QLineEdit;
        phone       = new QLineEdit;
        TechMak     = new QLineEdit;

        sizes->addItems(all_sizes);
        packs->addItems(pack);

        QLabel * label1        = new QLabel("ОПИСАНИЕ ЗАКАЗА: ");
        QLabel * label2        = new QLabel("ВЫБЕРИТЕ РАЗМЕР ");
        QLabel * label3        = new QLabel("ИМЯ КЛИЕНТА");
        QLabel * label4        = new QLabel("ТЕЛЕФОН ");
        QLabel * label5        = new QLabel("АДРЕС ");
        QLabel * label6        = new QLabel("СЛУЖБА ЛОГИСТИКИ");
        QLabel * label7        = new QLabel("ТЕХ.МАКЕТ ");
        QLabel * label8        = new QLabel("УПАКОВКА");
        QPushButton* addButton = new QPushButton("Добавить заказ");

        orderDialogQuery.exec("select* from name_list");
        while (orderDialogQuery.next())
            description->setText(orderDialogQuery.value(0).toString());
        description->setReadOnly(true);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label1);
        layout->addWidget(description);
        layout->addWidget(label2);
        layout->addWidget(sizes);
        layout->addWidget(label3);
        layout->addWidget(name);
        layout->addWidget(label4);
        layout->addWidget(phone);
        layout->addWidget(label5);
        layout->addWidget(adress);
        layout->addWidget(label6);
        layout->addWidget(LogServise);
        layout->addWidget(label7);
        layout->addWidget(TechMak);
        layout->addWidget(label8);
        layout->addWidget(packs);
        layout->addWidget(addButton);
        setLayout(layout);
        connect(addButton,SIGNAL(clicked()),this,SLOT(addOder()));
    }

public slots:

    //присосаться к базе

    void setQuery(QSqlQuery newQuery)
    {
        orderDialogQuery = newQuery;

    }

    //добавить заказ

    void addOder()
    {
        if(sizes->currentIndex()!=0 && packs->currentIndex()!=0
                && name->text()!=""
                && phone->text()!="" &&
                phone->text()!="" &&
                adress->text()!="" &&
                LogServise->text()!="" &&
                TechMak->text()!="")
        {
            //            user_list(id int, name varchar, position varchar, sphere varchar, login varchar,  password varchar, primary key (id))");
            //                myQuery.exec("create table user_and_oper_list(id int , oper_id int, oper_desc varchar, order_amount int, stock_amount int, room_amount int, equipment_amount int, primary key (id,oper_id))");
            orderDialogQuery.exec("select Id from (select id as Id, order_amount as amount from user_and_oper_list where id IN (select id as ID from user_list where sphere = 'Услуги' ) )as foo1 where foo1.amount = (select min(amount) from (select id as Id, order_amount as  amount from user_and_oper_list  where id IN (select id as ID from user_list where sphere = 'Услуги') ) as foo1)");
            orderDialogQuery.next();
            QString responsible = orderDialogQuery.value(0).toString();
            qDebug()<<responsible;
            if( respId!="-1")
                responsible = respId;
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
                    + responsibleName +"','"
                    +description->text()
                    +"','" +sizes->currentText()
                    +"','" +adress->text()+"','"
                    +  LogServise->text()+"','"+name->text()+"','" +
                    phone->text()+"','" +
                    TechMak->text() +"',now(), null, null,'"+ packs->currentText()+ "','не готов','не собран','не доставлен') returning ordNumber";
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

            qDebug()<<orderDialogQuery.lastError();
            done(1);
            emit updateOrderList();
        }
        else
        {
            QMessageBox message;
            message.setText("Заполните все поля");
            message.exec();
        }
    }
signals:

    void callQuery();
    void updateOrderList();
};
#endif // ADDORDERDIALOG_H
