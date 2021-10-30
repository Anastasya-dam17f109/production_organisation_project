#ifndef DELETEORDERDIALOG_H
#define DELETEORDERDIALOG_H


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


class deleteOrderDialog:public QDialog
{
    Q_OBJECT
public:
    QSqlQuery orderDialogQuery;

    QComboBox  * ordNumbers;
    QPushButton* deleteButton;

    deleteOrderDialog(QString id)
    {
        emit callQuery();
        resize(400,200);
        setWindowTitle("Удалить заказ");
        QVBoxLayout * layout = new QVBoxLayout;
        QList<QString> all_ords;
        all_ords<<"";
        if(id=="-1")
            orderDialogQuery.exec("select ordNumber from order_list group by ordNumber");
        else
            orderDialogQuery.exec("select ordNumber from order_list where resp_id = "+id+" group by ordNumber");
        while(orderDialogQuery.next())
            all_ords<<orderDialogQuery.value(0).toString();
        ordNumbers = new QComboBox;
        ordNumbers->addItems(all_ords);
        deleteButton  = new QPushButton("Удалить заказ");
        deleteButton->setEnabled(false);

        QLabel* label = new QLabel("ВЫБЕРИТЕ ЗАКАЗ ДЛЯ УДАЛЕНИЯ");
        label->setFixedHeight(20);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label);
        layout->addSpacing(40);
        layout->addWidget(ordNumbers);
        layout->addSpacing(40);
        layout->addWidget(deleteButton);
        setLayout(layout);

        connect(ordNumbers,SIGNAL(currentIndexChanged(int)),this,SLOT(makeDelButEnabled(int)));
        connect(deleteButton,SIGNAL(clicked()),this,SLOT(sendDelOrd()));
    }


public slots:

    //присосаться к базе

    void setQuery(QSqlQuery newQuery)
    {
        orderDialogQuery = newQuery;

    }

 //открыть возможность нажимать на кнопку удаления

    void makeDelButEnabled(int idx)
    {
        if(idx!=0)
        {
           deleteButton->setEnabled(true);
        }
        else
        {
           deleteButton->setEnabled(false);
        }
    }

//отправить номер заказа для удаления

    void sendDelOrd()
    {
        emit deleteThisOrd(ordNumbers->currentText());
        done(1);
    }
signals:
    void callQuery();
    void deleteThisOrd(QString ordNumb);
};
#endif // DELETEORDERDIALOG_H
