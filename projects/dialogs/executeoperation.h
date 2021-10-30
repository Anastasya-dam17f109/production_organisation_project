#ifndef EXECUTEOPERATION_H
#define EXECUTEOPERATION_H

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


class executeOperDialog:public QDialog
{
    Q_OBJECT
public:

    QSqlQuery exOpDialogQuery;

    QComboBox   * nOrd;
    QComboBox   * nOper;
    QPushButton * button;
    QString       id;
    QString       specify;

    QString createBigCommand(QString userId)
    {

        QString bigCommand = "";
        QList<QString> operations;
        QList<QString> orders;
        QList<QString> sizes;

        exOpDialogQuery.exec("select concr_order_operation_list.ordNumber as ORD,concr_order_operation_list.operNumber as OPER ,size as SIZE from concr_order_operation_list,order_list"
                     " where concr_order_operation_list.state = 'не выполнена' and concr_order_operation_list.ordNumber = order_list.ordNumber and worker = "
                      +userId);
        while(exOpDialogQuery.next())
        {
            operations<<exOpDialogQuery.value(1).toString();
            orders<<exOpDialogQuery.value(0).toString();
            sizes<<exOpDialogQuery.value(2).toString();
        }

        for(int i = 0; i<operations.size(); ++i)
        {
            exOpDialogQuery.exec("select id_source_stock from operation_list where id_operation = "+operations.at(i));
            QList<QString> s_stocks;
            while(exOpDialogQuery.next())
               s_stocks<<exOpDialogQuery.value(0).toString();
            int count = 0;
            qDebug()<<exOpDialogQuery.lastError();
            for(int j =0; j<s_stocks.size();++j  )
             {
                exOpDialogQuery.exec("select personalize from stock_list where id_stock = "+s_stocks.at(j));
                exOpDialogQuery.next();
                if(exOpDialogQuery.value(0).toString()=="нет")
                {
                    exOpDialogQuery.exec("select amount from not_pers_stock_list where id_stock = "+s_stocks.at(j)+ " and det_size = '"+ sizes.at(i)+"'");
                    exOpDialogQuery.next();
                    qDebug()<<exOpDialogQuery.lastError();
                    if(exOpDialogQuery.value(0).toString()!="0")
                        count++;
                }
                else
                {
                    exOpDialogQuery.exec("select amount from pers_stock_list where id_stock = "+s_stocks.at(j)+ " and n_ord = '"+ orders.at(i)+"'");
                    exOpDialogQuery.next();
                    if(exOpDialogQuery.value(0).toString()!="0")
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


    executeOperDialog(QString userId, QString spec)
    {

        resize(400,200);
        specify = spec;
        if(specify=="выполнить")
            setWindowTitle("Выполнить операцию");
        else
             setWindowTitle("Забраковать деталь");
        callQuery();
        QVBoxLayout * layout = new QVBoxLayout;
        if(specify=="выполнить")
        exOpDialogQuery.exec("select ORD from ("+createBigCommand(userId)+")as foo group by ORD");
        else
            exOpDialogQuery.exec("select concr_order_operation_list.ordNumber as ORD,operation_description as DESK, size, 'выполнена' from operation_list, order_list,concr_order_operation_list "
                                " where concr_order_operation_list.ordNumber = order_list.ordNumber and concr_order_operation_list.worker = "+userId+
                            " and concr_order_operation_list.operNumber = operation_list.id_operation and concr_order_operation_list.state = 'выполнена'"
                            " group by concr_order_operation_list.ordNumber,operation_description, size");
        id = userId;
        QList<QString> ordsForChoose;
        ordsForChoose<<"";

        while (exOpDialogQuery.next())
            ordsForChoose<<exOpDialogQuery.value(0).toString();

        nOrd       = new QComboBox;
        nOper      = new QComboBox;
        nOrd->addItems(ordsForChoose);
        nOper->setEnabled(false);

        QLabel * label1  = new QLabel("ВЫБЕРИТЕ ЗАКАЗ: ");
        QLabel * label2  = new QLabel("ВЫБЕРИТЕ ОПЕРАЦИЮ ");
        button           = new QPushButton("Выполнить операцию");
        button->setEnabled(false);

        if(specify!="выполнить")
        {
            label1->setText("ВЫБЕРИТЕ ЗАКАЗ: ");
            label2->setText("ВЫБЕРИТЕ ОПЕРАЦИЮ ДЛЯ ОТМЕНЫ");
        }

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label1);
        layout->addWidget( nOrd);
        layout->addWidget(label2);
        layout->addWidget(nOper);
        layout->addSpacing(20);
        layout->addWidget(button);
        setLayout(layout);

        connect(button,SIGNAL(clicked()),this,SLOT(execute()));
        connect(nOrd, SIGNAL(currentIndexChanged(int)), this, SLOT(setOperEnabled()));
        connect(nOper, SIGNAL(currentIndexChanged(int)), this, SLOT(setButEnabled()));
    }

public slots:

    //присосаться к базе

    void setQuery(QSqlQuery newQuery)
    {
        exOpDialogQuery = newQuery;

    }

    //включение операции

    void setOperEnabled()
    {
        if(nOrd->currentIndex()==0)
        {
            nOper->setCurrentIndex(0);
            nOper->setEnabled(false);
        }
        else
        {
            nOper->setEnabled(true);
            if(nOper->count()!=0)
                nOper->clear();
            if(specify=="выполнить")
            exOpDialogQuery.exec("select DESK from ("+createBigCommand(id)+")as foo where ORD = '"+nOrd->currentText()+"'");
            else
                exOpDialogQuery.exec("select DESK from (select concr_order_operation_list.ordNumber as ORD,operation_description as DESK, size, 'выполнена' from operation_list, order_list,concr_order_operation_list "
                                     " where concr_order_operation_list.ordNumber = order_list.ordNumber and concr_order_operation_list.worker = "+id+
                                 " and concr_order_operation_list.operNumber = operation_list.id_operation and concr_order_operation_list.state = 'выполнена'"
                                 " group by concr_order_operation_list.ordNumber,operation_description, size)as foo where ORD = '"+nOrd->currentText()+"'");
           qDebug()<<exOpDialogQuery.lastError();
            QList<QString> opers;
            opers<<"";
            while(exOpDialogQuery.next())
                opers<<exOpDialogQuery.value(0).toString();
            nOper->addItems(opers);
        }
    }

   //включение кнопки

   void setButEnabled()
   {
       if(nOper->currentIndex()==0)
           button->setEnabled(false);
       else
           button->setEnabled(true);
   }

   //выполнить операцию

   void execute()
   {
       if(specify=="выполнить")
       {
           exOpDialogQuery.exec("select ORD,operation_description, SIZE from("+createBigCommand(id)+") as foo6 where foo6.ORD = '"
                                +nOrd->currentText()+"'");
           exOpDialogQuery.next();
           if( exOpDialogQuery.size()!=0)
           {
               exOpDialogQuery.exec("select id_operation from operation_list where operation_description = '"
                                    +nOper->currentText()+"'");
               exOpDialogQuery.next();

               emit executeIt(nOrd->currentText(), exOpDialogQuery.value(0).toString());
               done(1);
           }
           else
           {
               QMessageBox message;
               message.setText("Невозможно выполнить. Нет необходимого материала.");
               message.exec();

           }
       }
       else
       {

           exOpDialogQuery.exec("select ORD,operation_description, SIZE from(( select concr_order_operation_list.ordNumber as ORD,operation_description, size, 'выполнена' from operation_list, order_list,concr_order_operation_list "
                                " where concr_order_operation_list.ordNumber = order_list.ordNumber and concr_order_operation_list.worker = "+id+
                                " and concr_order_operation_list.operNumber = operation_list.id_operation and concr_order_operation_list.state = 'выполнена'"
                                " group by concr_order_operation_list.ordNumber,operation_description, size )) as foo6 where foo6.ORD = '"
                                +nOrd->currentText()+"'");
           exOpDialogQuery.next();
           if( exOpDialogQuery.size()!=0)
           {
               exOpDialogQuery.exec("select id_operation from operation_list where operation_description = '"
                                    +nOper->currentText()+"'");
               exOpDialogQuery.next();

                emit regectIt(nOrd->currentText(), exOpDialogQuery.value(0).toString());
               done(1);
           }
           else
           {
               QMessageBox message;
               message.setText("Невозможно выполнить. Уже забраковали.");
               message.exec();

           }
       }
   }

signals:

    void callQuery();
    void executeIt(QString ord, QString oper);
    void regectIt(QString ord, QString oper);
};
#endif // EXECUTEOPERATION_H
