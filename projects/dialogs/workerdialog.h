#ifndef WORKERDIALOG_H
#define WORKERDIALOG_H
#include <QDialog>
#include <QList>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>


class workerDialog:public QDialog
{
    Q_OBJECT
public:

    QComboBox *workerName;


    QWidget*   updatePage;
    QString ordNumber;
    QString operNumber;
    QSqlQuery operDialogQuery;
    QPushButton* updateButton;

    QList<QString> allWorkIndex;

//создание диалога-добавления склада

    workerDialog(QString Number, QString operation)
    {
//create table user_and_oper_list(id int , oper_id int, oper_desc varchar, order_amount int, stock_amount int, room_amount int, equipment_amount int,
        resize(400,300);
        setWindowTitle("Изменить исполнителя операции");
        callQuery();
        QList<QString> wNames;
        ordNumber = Number;
        operNumber = operation;
        wNames<<"";
        allWorkIndex<<"";
        operDialogQuery.exec("select user_and_oper_list.id, name from user_list,user_and_oper_list where user_and_oper_list.id = user_list.id and oper_id ="+ operation);
        qDebug()<<operDialogQuery.lastError();
        while(operDialogQuery.next())
        {
            wNames<< operDialogQuery.value(1).toString();
            allWorkIndex<< operDialogQuery.value(0).toString();
        }

        workerName = new QComboBox;
        workerName->addItems(wNames);

        createUpdatePage();
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(updatePage);
        setLayout(layout);
    }



    void createUpdatePage()
    {
        updatePage             = new QWidget;
        QHBoxLayout* lay       = new QHBoxLayout;
        QVBoxLayout *layout    = new QVBoxLayout;
        QWidget     * wid1     = new QWidget;
        updateButton           = new QPushButton("Сохранить изменения");
        QLabel      *label1    = new QLabel("ВЫБЕРИТЕ РАБОТНИКА");

        updateButton->setEnabled(false);
        label1->setFixedHeight(20);

        lay->addSpacing(300);
        lay->addWidget(updateButton);
        wid1->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addSpacing(50);
        layout->addWidget(label1);
        layout->addSpacing(100);
        layout->addWidget(workerName);
        layout->addSpacing(50);
        layout->addWidget(wid1);

        connect(workerName, SIGNAL(currentIndexChanged(int)), this, SLOT(setUpdateEnabled(int)));
        connect(updateButton, SIGNAL(clicked()), this, SLOT(setUpdates()));
        updatePage->setLayout(layout);
    }

public slots:

    //присосаться к базе

    void setQuery(QSqlQuery newQuery)
    {
        operDialogQuery = newQuery;
    }

    //разблокировка кнопки

    void setUpdateEnabled(int idx)
    {
        if(idx==0)
            updateButton->setEnabled(false);
        else
        {
            updateButton->setEnabled(true);
            callQuery();
        }
    }

     //смена ответственного

    void setUpdates()
    {

        callQuery();
        QString id = allWorkIndex.at(workerName->currentIndex());
        QString last_id;
        operDialogQuery.exec("select worker from concr_order_operation_list where ordNumber = '"+ordNumber +"'" + " and operNumber = "+ operNumber) ;
        operDialogQuery.next();
        last_id = operDialogQuery.value(0).toString();
        if(last_id!=id)
        {

            operDialogQuery.exec("update user_and_order_list set id = "+id+" where id = "+last_id+" and n_ord = '"+ordNumber+"' and n_oper = "+ operNumber);
            operDialogQuery.exec("select order_amount from user_and_oper_list where id = "+last_id+" and oper_id = "+ operNumber);
            operDialogQuery.next();
            QString number = operDialogQuery.value(0).toString();
            operDialogQuery.exec("update  user_and_oper_list set order_amount  = " +QString::number(number.toInt()-1)+" where id = "+last_id+" and oper_id = "+ operNumber);
            operDialogQuery.exec("select name from user_list where id = "+id);
            operDialogQuery.next();
            QString name = operDialogQuery.value(0).toString();
            operDialogQuery.exec("update  concr_order_operation_list set worker = " +id+" where ordNumber = '"+ordNumber +"'"+ " and operNumber = "+ operNumber);
//+" and oper_id = "+ operations.at(i)
            operDialogQuery.exec("select order_amount from user_and_oper_list where id = "+id+" and oper_id = "+ operNumber);
            operDialogQuery.next();
             number = operDialogQuery.value(0).toString();
            operDialogQuery.exec("update  user_and_oper_list set order_amount  = " +QString::number(number.toInt()+1)+" where id = "+id+" and oper_id = "+ operNumber);
        }
        emit updateCurOrdView();
        done(1);
    }
signals:
    void callQuery();
    void updateCurOrdView();
};
#endif // WORKERDIALOG_H
