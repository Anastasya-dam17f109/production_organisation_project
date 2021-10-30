#ifndef DELIVERDIALOG_H
#define DELIVERDIALOG_H

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


class deliverDialog:public QDialog
{
    Q_OBJECT
public:

    QComboBox *    deliverName;
    QString        ordNumber;
    QSqlQuery      operDialogQuery;
    QList<QString> allDeliverIndex;
    bool flag;
//создание диалога про курьера

    deliverDialog(QString order, bool managerFlag)
    {
        if(!managerFlag)
            resize(400,300);
        else
            resize(400,150);
        setWindowTitle("Ответственный за доставку ");
        callQuery();

        ordNumber = order;
        flag = managerFlag;
        QList<QString> deliverNames;
        allDeliverIndex<<"";
        deliverNames<<"";
        operDialogQuery.exec("select id, name from user_list where sphere = 'Доставка'");
        while(operDialogQuery.next())
        {
            allDeliverIndex<< operDialogQuery.value(0).toString();
            deliverNames<< operDialogQuery.value(1).toString();
        }

        deliverName = new QComboBox;
        deliverName->addItems(deliverNames);

        operDialogQuery.exec("select id_deliver from delivery_list where n_ord = '"+ordNumber+"'");
        operDialogQuery.next();
        QString id = operDialogQuery.value(0).toString();
        QString name;
        if(id!="-1")
        {
            operDialogQuery.exec("select name from user_list where id = "+id);
            operDialogQuery.next();
            name =  operDialogQuery.value(0).toString();
        }
        else
            name =  "не определен";
        QLabel *label1       = new QLabel("ИМЯ  :  ");
        QLabel *label2       = new QLabel(name);
        QWidget *widget1     = new QWidget;
        QHBoxLayout* layout1 = new QHBoxLayout;

        label1->setFixedHeight(50);
        label2->setFixedHeight(50);
        label1->setFixedWidth(60);
        label2->setFixedWidth(200);
        widget1->setFixedHeight(50);
        layout1->addWidget(label1);
        layout1->addWidget(label2);
        layout1->addSpacing(140);
        widget1->setLayout(layout1);

        QPushButton *button = new QPushButton("ОК");

        QVBoxLayout * layout = new QVBoxLayout;
        layout->addWidget(widget1);
        if(!managerFlag)
        {
            QLabel * label3 = new QLabel("ВЫБРАТЬ НОВОГО ОТВЕТСТВЕННОГО ЗА ДОСТАВКУ: ");
            layout->addWidget(label3);
            layout->addWidget(deliverName);

        }
        layout->addWidget(button);
        setLayout(layout);
        connect(button,SIGNAL(clicked()),this,SLOT(changeOrClose()));
    }

public slots:

    //присосаться к базе

    void setQuery(QSqlQuery newQuery)
    {
        operDialogQuery = newQuery;
    }

    //закрыть окно или сменить курьера

    void changeOrClose()
    {
        operDialogQuery.exec("select id_deliver from delivery_list where n_ord = '"+ordNumber+"'");
        operDialogQuery.next();
        QString id = operDialogQuery.value(0).toString();
        if(flag)
        {
            done(1);
        }
        else
        {

            QString chosenIdx = allDeliverIndex.at(deliverName->currentIndex());
            if(deliverName->currentIndex()==0||chosenIdx==id)
                done(1);
            else
            {
                if(id != "-1")
                {
                    operDialogQuery.exec("select  order_amount from user_and_oper_list where id =  " + id);
                    operDialogQuery.next();
                    QString curOrdAmount = operDialogQuery.value(0).toString();
                    operDialogQuery.exec("update user_and_oper_list set order_amount = "
                                          +QString::number(curOrdAmount.toInt() -1)+" where id =  " + id);

                    operDialogQuery.exec("select  order_amount from user_and_oper_list where id =  " + chosenIdx);
                    operDialogQuery.next();
                    curOrdAmount = operDialogQuery.value(0).toString();
                    operDialogQuery.exec("update user_and_oper_list set order_amount = "
                                          +QString::number(curOrdAmount.toInt() +1)+" where id =  " + chosenIdx);

                    operDialogQuery.exec("update delivery_list set id_deliver = "
                                          +chosenIdx+" where where n_ord = '"+ordNumber+"'");
                    done(1);
                }
            }
        }

    }

signals:
    void callQuery();

};
#endif // DELIVERDIALOG_H
