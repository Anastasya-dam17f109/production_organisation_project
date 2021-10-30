#ifndef RESPONSIBLEDIALOG_H
#define RESPONSIBLEDIALOG_H

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


class responsibleDialog:public QDialog
{
    Q_OBJECT
public:

    QComboBox *responsibleName;


    QWidget*   updatePage;
    QList<QString> ordNumber;
    QSqlQuery operDialogQuery;
    QPushButton* updateButton;

    QList<QString> allRespIndex;

//создание диалога-добавления склада

    responsibleDialog(QList<QString> Number)
    {

        resize(400,300);
        setWindowTitle("Изменить ответственного за заказ");
        callQuery();
        QList<QString> operations;
        ordNumber = Number;
        operations<<"";
        allRespIndex<<"";
        operDialogQuery.exec("select id, name from user_list where sphere = 'Услуги'");
        while(operDialogQuery.next())
        {
            operations<< operDialogQuery.value(1).toString();
            allRespIndex<< operDialogQuery.value(0).toString();
        }

        responsibleName = new QComboBox;
        responsibleName->addItems(operations);

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
        QLabel      *label1    = new QLabel("ВЫБЕРИТЕ ОТВЕТСТВЕННОГО");

        updateButton->setEnabled(false);
        label1->setFixedHeight(20);

        lay->addSpacing(300);
        lay->addWidget(updateButton);
        wid1->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addSpacing(50);
        layout->addWidget(label1);
        layout->addSpacing(100);
        layout->addWidget(responsibleName);
        layout->addSpacing(50);
        layout->addWidget(wid1);

        connect(responsibleName, SIGNAL(currentIndexChanged(int)), this, SLOT(setUpdateEnabled(int)));
        connect(updateButton, SIGNAL(clicked()), this, SLOT(setUpdates()));
        updatePage->setLayout(layout);
    }

public slots:

    //присосаться к базе

    void setQuery(QSqlQuery newQuery)
    {
        operDialogQuery = newQuery;
    }

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

    void setUpdates()
    {
        callQuery();
        QString id = allRespIndex.at(responsibleName->currentIndex());
        QString last_id;
        for(int i=0; i<ordNumber.size();++i)
        {
            operDialogQuery.exec("select resp_id from order_list where ordNumber = '"+ordNumber.at(i) +"'") ;
            operDialogQuery.next();
            last_id = operDialogQuery.value(0).toString();
            if(last_id!=id)
            {
                qDebug()<<"sdsdsdasd";
                operDialogQuery.exec("update user_and_order_list set id = "+id+" where id = "+last_id+" and n_ord = '"+ordNumber.at(i)+"'");
                operDialogQuery.exec("select order_amount from user_and_oper_list where id = "+last_id);
                operDialogQuery.next();
                QString number = operDialogQuery.value(0).toString();
                operDialogQuery.exec("update  user_and_oper_list set order_amount  = " +QString::number(number.toInt()-1)+" where id = "+last_id);
                operDialogQuery.exec("select name from user_list where id = "+id);
                operDialogQuery.next();
                QString name = operDialogQuery.value(0).toString();
                operDialogQuery.exec("update  order_list set resp_id = " +id+" where ordNumber = '"+ordNumber.at(i) +"'");
                operDialogQuery.exec("update  order_list set responsible  = '" +name+"' where ordNumber = '"+ordNumber.at(i) +"'");
                operDialogQuery.exec("select order_amount from user_and_oper_list where id = "+id);
                operDialogQuery.next();
                number = operDialogQuery.value(0).toString();
                operDialogQuery.exec("update  user_and_oper_list set order_amount  = " +QString::number(number.toInt()+1)+" where id = "+id);
            }
        }
        emit updateOrderList();
        done(1);
    }
signals:
    void callQuery();
    void updateOrderList();
};

#endif // RESPONSIBLEDIALOG_H
