#ifndef OPERATIONDIALOG_H
#define OPERATIONDIALOG_H

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


class operationDialog:public QDialog
{
    Q_OBJECT
public:

    QComboBox *updateName;
    QLineEdit *newNameLine;

    QWidget*   updatePage;

    QSqlQuery operDialogQuery;
    QPushButton* updateButton;

    QList<QString> allOperIndex;

//создание диалога-добавления склада

    operationDialog()
    {

        resize(400,300);
        setWindowTitle("Переименовать операцию");
        callQuery();
        QList<QString> operations;
        operations<<"";
        allOperIndex<<"";
        operDialogQuery.exec("select id_operation, operation_description from operation_list group by id_operation, operation_description");
        while(operDialogQuery.next())
        {
            operations<< operDialogQuery.value(1).toString();
            allOperIndex<< operDialogQuery.value(0).toString();
        }

        updateName = new QComboBox;
        updateName->addItems(operations);
        newNameLine = new QLineEdit;

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
        QLabel      *label1    = new QLabel("ВЫБЕРИТЕ ОПЕРАЦИЮ");
        QLabel      *label2    = new QLabel("ТЕКУЩЕЕ НАЗВАНИЕ ОПЕРАЦИИ");

        updateButton->setEnabled(false);
        newNameLine->setReadOnly(true);

        label1->setFixedHeight(20);
        label2->setFixedHeight(20);

        lay->addSpacing(300);
        lay->addWidget(updateButton);;
        wid1->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addSpacing(50);
        layout->addWidget(label1);
        layout->addSpacing(20);
        layout->addWidget(updateName);
        layout->addSpacing(20);
        layout->addSpacing(20);
        layout->addWidget(label2);
        layout->addSpacing(20);
        layout->addWidget(newNameLine);
        layout->addSpacing(50);
        layout->addWidget(wid1);

        connect(updateName, SIGNAL(currentIndexChanged(int)), this, SLOT(setUpdateEnabled(int)));
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
        {
            updateButton->setEnabled(false);
            newNameLine->clear();
            newNameLine->setReadOnly(true);
        }
        else
        {
            updateButton->setEnabled(true);
            newNameLine->setReadOnly(false);
            callQuery();
            newNameLine->setText(updateName->currentText());
        }
    }

    void setUpdates()
    {
        callQuery();
        QString id = allOperIndex.at(updateName->currentIndex());

        operDialogQuery.exec("update operation_list set operation_description = '"+newNameLine->text()+"' where id_operation = "+ id);
        operDialogQuery.exec("update user_and_oper_list set oper_desc = '"+newNameLine->text()+"' where oper_id = "+ id);
        operDialogQuery.exec("update user_and_order_list set oper_desc = '"+newNameLine->text()+"' where n_oper = "+ id);

        emit updateScheme();
        emit updateOperTable();
        done(1);
    }
signals:
    void callQuery();
    void updateOperTable();
    void updateScheme();
};


#endif // OPERATIONDIALOG_H
