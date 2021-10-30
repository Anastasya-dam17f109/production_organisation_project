#ifndef STOCKERDETDIALOG_H
#define STOCKERDETDIALOG_H
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
using namespace std;

class stockerDetDialog:public QDialog
{
    Q_OBJECT
public:

    QLineEdit * line;
    QPushButton * deleteButton;
    QSqlQuery orderDialogQuery;
    int flagAdd;
    QString m_id;
    QString m_size;

    stockerDetDialog(int flag, QString id, QString size)
    {
        callQuery();
        resize(400,200);
        flagAdd = flag;
        m_id = id;
        m_size = size;
        if(flag == 1)
        setWindowTitle("Удалить детали");
        else
            setWindowTitle("Добавить детали");
        QVBoxLayout * layout = new QVBoxLayout;
        deleteButton = new QPushButton("Выполнить");
        line = new QLineEdit ;
        QLabel* label = new QLabel("ВВЕДИТЕ КОЛИЧЕСТВО ДЕТАЛЕЙ");
        label->setFixedHeight(20);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label);
        layout->addSpacing(40);
        layout->addWidget(line);
        layout->addSpacing(40);
        layout->addWidget(deleteButton);
        setLayout(layout);

        connect(line,SIGNAL(textChanged(const QString)),this,SLOT(makeDelButEnabled(const QString)));
        connect(deleteButton,SIGNAL(clicked()),this,SLOT(addOrDelete()));
    }
public slots:
    //присосаться к базе

    void setQuery(QSqlQuery newQuery)
    {
        orderDialogQuery = newQuery;

    }

 //открыть возможность нажимать на кнопку удаления

    void makeDelButEnabled(QString idx)
    {
        if(idx!="")
        {
           deleteButton->setEnabled(true);
        }
        else
        {
           deleteButton->setEnabled(false);
        }
    }


    void addOrDelete()
    {
        if(flagAdd!=1)
        {

            orderDialogQuery.exec("select amount from not_pers_stock_list where id_stock = "
                                  +m_id+" and det_size = '"+m_size+"'");
            orderDialogQuery.next();
            int currAmount = orderDialogQuery.value(0).toInt();
            orderDialogQuery.exec("update  not_pers_stock_list set amount = "
                                  +QString:: number(line->text().toInt() +currAmount)
                                  + "where id_stock = "+m_id+"  and det_size = '"
                                  +m_size+"'");
    //         user_and_order_list( id int , n_ord varchar, n_oper int, oper_desc varchar, n_stock int, stock_desc varchar, , det_amount int, primary key (id,n_ord,n_oper,n_stock,n_room,n_equipment))")
            orderDialogQuery.exec("update user_and_order_list set det_amount = (select sum(amount) from not_pers_stock_list where  id_stock = "+m_id+"  )"

                                  + "where n_stock = "+m_id);
            emit  updateCurrStockTable();
            done(1);
        }
        else
        {
            orderDialogQuery.exec("select amount from not_pers_stock_list where id_stock = "
                                  +m_id+" and det_size = '"+m_size+"'");
            orderDialogQuery.next();
            int currAmount = orderDialogQuery.value(0).toInt();
            int amount = -line->text().toInt() +currAmount;
            if(amount<0)
                amount = 0;
            orderDialogQuery.exec("update  not_pers_stock_list set amount = "
                                  +QString:: number(amount)
                                  + "where id_stock = "+m_id+"  and det_size = '"
                                  +m_size+"'");
    //         user_and_order_list( id int , n_ord varchar, n_oper int, oper_desc varchar, n_stock int, stock_desc varchar, , det_amount int, primary key (id,n_ord,n_oper,n_stock,n_room,n_equipment))")
            orderDialogQuery.exec("update user_and_order_list set det_amount = (select sum(amount) from not_pers_stock_list where  id_stock = "+m_id+"  )"

                                  + "where n_stock = "+m_id);
            emit  updateCurrStockTable();
            done(1);
        }
    }
signals:
    void callQuery();
    void updateCurrStockTable();
};

#endif // STOCKERDETDIALOG_H
