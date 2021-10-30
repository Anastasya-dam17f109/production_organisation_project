#ifndef USERSELECTDIALOG_H
#define USERSELECTDIALOG_H

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

class userSelectDialog : public QDialog
{
    Q_OBJECT
public:
    QWidget* firstPage;
    QWidget* namePage;
    QWidget* operationPage;
    QWidget* stockPage;
    QWidget* orderPage;
    QWidget* positionPage;
    QWidget* spherePage;


    QComboBox *position;
    QComboBox *name;
    QComboBox *operation;
    QComboBox *stock;
    QComboBox *spere;
    QComboBox *order;

    QStackedLayout *stackedLayout;

    QSqlQuery selectDialogQuery;

    QList<QString> id;
    QList<QString> sphereUser;

    QPushButton* nextButton;

    int currIdx = 0;
    userSelectDialog()
    {
        resize(400,500);
        setWindowTitle("Создание выборки из списка пользователей");
        callQuery();
        QList<QString> operations;
        QList<QString> stocks;
        QList<QString> user_names;
        QList<QString> positions;
        QList<QString> orders;
        QList<QString> spheres;

        operations<<"";
        stocks<<"";
        user_names<<"";
        positions<<"";
        orders<<"";
        id<<"";
        sphereUser<<"";
        spheres<<"";

        selectDialogQuery.exec("select id, operation_description from operation_list, id_oper_list where   id_oper_list.id  = operation_list.id_operation  group by  id, operation_description  order by id");
        while(selectDialogQuery.next())
            operations<< selectDialogQuery.value(1).toString();

        selectDialogQuery.exec("select stock_description from stock_list");
        while(selectDialogQuery.next())
            stocks<< selectDialogQuery.value(0).toString();

        selectDialogQuery.exec("select id, name, sphere from user_list");
        while(selectDialogQuery.next())
        {
            user_names<< selectDialogQuery.value(1).toString();
            id<< selectDialogQuery.value(0).toString();
            sphereUser<< selectDialogQuery.value(2).toString();
        }

        selectDialogQuery.exec("select sphere , position from position_list");
        while(selectDialogQuery.next())
        {
            spheres<< selectDialogQuery.value(0).toString();
            positions<< selectDialogQuery.value(1).toString();
        }

        selectDialogQuery.exec("select ordNumber from order_list");
        while(selectDialogQuery.next())
        {
            orders<< selectDialogQuery.value(0).toString();

        }
        // здесь будет такой же блок, но со списком заказов

        position  = new QComboBox;
        name      = new QComboBox;
        operation = new QComboBox;
        stock     = new QComboBox;
        spere     = new QComboBox;
        order     = new QComboBox;

        position->addItems(positions);
        name->addItems(user_names);
        operation ->addItems(operations);
        stock->addItems(stocks);
        spere->addItems(spheres);
        order->addItems(orders);

        createFirstPage();
        createNamePage();
        createOperationPage();
        createStockPage();
        createPositionPage();
        createSpherePage();
        createOrderPage();

        QVBoxLayout *layout = new QVBoxLayout;
        stackedLayout = new QStackedLayout;
        stackedLayout->addWidget(firstPage);
        stackedLayout->addWidget(namePage);
        stackedLayout->addWidget(operationPage);
        stackedLayout->addWidget(stockPage);
        stackedLayout->addWidget(positionPage);
        stackedLayout->addWidget(spherePage);
        stackedLayout->addWidget(orderPage);

        layout->addLayout(stackedLayout);
        setLayout(layout);
    }



    void createFirstPage()
    {
        firstPage           = new QWidget;
        QWidget * wid       = new QWidget;
        QLabel      * label = new QLabel("ВЫБЕРИТЕ УСЛОВИЕ ДЛЯ ПОИСКА:");
        QVBoxLayout *layout = new QVBoxLayout;
        QHBoxLayout* lay    = new QHBoxLayout;
        nextButton          = new QPushButton("Далее");
        QRadioButton* b1    = new QRadioButton("Информация о конкретном пользователе");
        QRadioButton* b2    = new QRadioButton("Информация о конкретной операции");
        QRadioButton* b3    = new QRadioButton("Информация о конкретном складе");
        QRadioButton* b4    = new QRadioButton("Информация о конкретной должности");
        QRadioButton* b5    = new QRadioButton("Информация о конкретной сфере деятельности");
        QRadioButton* b6    = new QRadioButton("Информация о конкретном заказе");
        QButtonGroup* groop = new QButtonGroup();
        nextButton ->setEnabled(false);
        label ->setFixedHeight(20);
        groop->setExclusive(true);
        groop->addButton(b1);
        groop->addButton(b2);
        groop->addButton(b3);
        groop->addButton(b4);
        groop->addButton(b5);
        groop->addButton(b6);


        lay->addSpacing(200);
        lay->addWidget(nextButton);;
        wid->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label);
        layout->addSpacing(50);
        layout->addWidget(b1);
        layout->addSpacing(20);
        layout->addWidget(b2);
        layout->addSpacing(20);
        layout->addWidget(b3);
        layout->addSpacing(20);
        layout->addWidget(b4);
        layout->addSpacing(20);
        layout->addWidget(b5);
        layout->addSpacing(20);
        layout->addWidget(b6);
        layout->addSpacing(100);
        layout->addWidget(wid);

        firstPage->setLayout(layout);

        connect(nextButton, SIGNAL(clicked()), this, SLOT(nextPage()));
        connect(groop, SIGNAL(buttonClicked(int )), this, SLOT(setCurrIdx(int )));
    }

   //создания виджета выбора имени

    void createNamePage()
    {
        namePage                = new QWidget;
        QWidget     * wid1      = new QWidget;
        QPushButton* button1    = new QPushButton("Назад");
        QPushButton* findButton = new QPushButton("Выбрать");
        QHBoxLayout* lay        = new QHBoxLayout;
        QVBoxLayout *layout     = new QVBoxLayout;
        QLabel      *label1     = new QLabel("ВЫБЕРИТЕ ПОЛЬЗОВАТЕЛЯ  ");

        label1->setFixedHeight(20);
        lay->addWidget(button1);
        lay->addSpacing(200);
        lay->addWidget(findButton);;
        wid1->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label1);
        layout->addSpacing(100);
        layout->addWidget(name);
        layout->addSpacing(250);
        layout->addWidget(wid1);
        namePage->setLayout(layout);

        //connect(name, SIGNAL(currentIndexChanged(int)), this, SLOT(setDelButtonEnabled(int)));
        connect(button1, SIGNAL(clicked()), this, SLOT(backPage()));
        connect(findButton, SIGNAL(clicked()), this, SLOT(makeNameSelection()));
    }

     //создания виджета выбора операции

    void createOperationPage()
    {
        operationPage           = new QWidget;
        QWidget     * wid1      = new QWidget;
        QPushButton* button1    = new QPushButton("Назад");
        QPushButton* findButton = new QPushButton("Выбрать");
        QHBoxLayout* lay        = new QHBoxLayout;
        QVBoxLayout *layout     = new QVBoxLayout;
        QLabel      *label1     = new QLabel("ВЫБЕРИТЕ ОПЕРАЦИЮ: ");

        label1->setFixedHeight(20);
        lay->addWidget(button1);
        lay->addSpacing(200);
        lay->addWidget(findButton);;
        wid1->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label1);
        layout->addSpacing(100);
        layout->addWidget(operation);
        layout->addSpacing(250);
        layout->addWidget(wid1);
        operationPage->setLayout(layout);

        //connect(operation, SIGNAL(currentIndexChanged(int)), this, SLOT(setDelButtonEnabled(int)));
        connect(button1, SIGNAL(clicked()), this, SLOT(backPage()));
        connect(findButton, SIGNAL(clicked()), this, SLOT(makeOperationSelection()));
    }

     //создания виджета выбора склада

    void createStockPage()
    {
        stockPage               = new QWidget;
        QWidget     * wid1      = new QWidget;
        QPushButton* button1    = new QPushButton("Назад");
        QPushButton* findButton = new QPushButton("Выбрать");
        QHBoxLayout* lay        = new QHBoxLayout;
        QVBoxLayout *layout     = new QVBoxLayout;
        QLabel      *label1     = new QLabel("ВЫБЕРИТЕ СКЛАД: ");

        label1->setFixedHeight(20);
        lay->addWidget(button1);
        lay->addSpacing(200);
        lay->addWidget(findButton);;
        wid1->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label1);
        layout->addSpacing(100);
        layout->addWidget(stock);
        layout->addSpacing(250);
        layout->addWidget(wid1);
        stockPage->setLayout(layout);

        connect(button1, SIGNAL(clicked()), this, SLOT(backPage()));
        connect(findButton, SIGNAL(clicked()), this, SLOT(makeStockSelection()));
    }

    //создания виджета выбора должности

    void createPositionPage()
    {
        positionPage               = new QWidget;
        QWidget     * wid1      = new QWidget;
        QPushButton* button1    = new QPushButton("Назад");
        QPushButton* findButton = new QPushButton("Выбрать");
        QHBoxLayout* lay        = new QHBoxLayout;
        QVBoxLayout *layout     = new QVBoxLayout;
        QLabel      *label1     = new QLabel("ВЫБЕРИТЕ ДОЛЖНОСТЬ: ");

        label1->setFixedHeight(20);
        lay->addWidget(button1);
        lay->addSpacing(200);
        lay->addWidget(findButton);;
        wid1->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label1);
        layout->addSpacing(100);
        layout->addWidget(position);
        layout->addSpacing(250);
        layout->addWidget(wid1);
        positionPage->setLayout(layout);

        //connect(position, SIGNAL(currentIndexChanged(int)), this, SLOT(setDelButtonEnabled(int)));
        connect(button1, SIGNAL(clicked()), this, SLOT(backPage()));
        connect(findButton, SIGNAL(clicked()), this, SLOT(makePositionSelection()));
    }

    void createSpherePage()
    {
        spherePage               = new QWidget;
        QWidget     * wid1      = new QWidget;
        QPushButton* button1    = new QPushButton("Назад");
        QPushButton* findButton = new QPushButton("Выбрать");
        QHBoxLayout* lay        = new QHBoxLayout;
        QVBoxLayout *layout     = new QVBoxLayout;
        QLabel      *label1     = new QLabel("ВЫБЕРИТЕ СФЕРУ ДЕЯТЕЛЬНОСТИ: ");

        label1->setFixedHeight(20);
        lay->addWidget(button1);
        lay->addSpacing(200);
        lay->addWidget(findButton);;
        wid1->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label1);
        layout->addSpacing(100);
        layout->addWidget(spere);
        layout->addSpacing(250);
        layout->addWidget(wid1);
        spherePage->setLayout(layout);

       // connect(spere, SIGNAL(currentIndexChanged(int)), this, SLOT(setDelButtonEnabled(int)));
        connect(button1, SIGNAL(clicked()), this, SLOT(backPage()));
        connect(findButton, SIGNAL(clicked()), this, SLOT(makeSphereSelection()));
    }

    void createOrderPage()
    {
        orderPage               = new QWidget;
        QWidget     * wid1      = new QWidget;
        QPushButton* button1    = new QPushButton("Назад");
        QPushButton* findButton = new QPushButton("Выбрать");
        QHBoxLayout* lay        = new QHBoxLayout;
        QVBoxLayout *layout     = new QVBoxLayout;
        QLabel      *label1     = new QLabel("ВЫБЕРИТЕ ЗАКАЗ: ");

        label1->setFixedHeight(20);
        lay->addWidget(button1);
        lay->addSpacing(200);
        lay->addWidget(findButton);;
        wid1->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label1);
        layout->addSpacing(100);
        layout->addWidget(order);
        layout->addSpacing(250);
        layout->addWidget(wid1);
        orderPage->setLayout(layout);

        //connect(order, SIGNAL(currentIndexChanged(int)), this, SLOT(setDelButtonEnabled(int)));
        connect(button1, SIGNAL(clicked()), this, SLOT(backPage()));
        connect(findButton, SIGNAL(clicked()), this, SLOT(makeOrderSelection()));
    }
public slots:

    //присосаться к базе

    void setQuery(QSqlQuery newQuery)
    {
        selectDialogQuery = newQuery;

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
    }

    //индекс кнопки выбора действия

    void setCurrIdx(int idx)
    {
        currIdx = idx;
        nextButton ->setEnabled(true);
    }

    //выборка по текущему имени пользователя

    void makeNameSelection()
    {
        if(name->currentIndex()!=0)
        {
            QList<QString> header1;
            QList<QString> header2;

            emit sendName(id.at(name->currentIndex()).toInt());
            emit sendCommands("","",header1,header2);
            done(1);
        }
        else
        {
            QMessageBox message;
            message.setText("Невозможно сделать выборку. Пользователь не выбран");
            message.exec();
        }
    }

    //выборка по текущей операции

    void makeOperationSelection()
    {
        if(operation->currentIndex()!=0)
        {
            selectDialogQuery.exec("select id_operation from operation_list where operation_description = '"+operation->currentText()+"'");
            selectDialogQuery.next();
            QString id_oper = selectDialogQuery.value(0).toString();
             QString nSpCommand =" select name, position, oper_desc,  order_amount from user_and_oper_list,user_list where user_list.id = user_and_oper_list.id and  user_and_oper_list.oper_id =  "
                    +id_oper;
            QString spCommand ="select name,operation_description, ordNumber, state from user_list, operation_list,concr_order_operation_list"
                    " where user_list.id = concr_order_operation_list.worker and operation_list.id_operation = concr_order_operation_list.operNumber"
                    " and operation_list.id_operation ="+id_oper;

            QList<QString> header1;
            QList<QString> header2;
            header1<<"Имя"<<"Должность"<<"Oперация"<<"Количество заказов";
            header2<<"Имя"<<"Операция"<<"Номер Заказа"<<"Статус";
            emit sendName(0);
            emit sendCommands(nSpCommand,spCommand,header1,header2);
            done(1);
        }
        else
        {
            QMessageBox message;
            message.setText("Невозможно сделать выборку. Операция не выбрана");
            message.exec();
        }
    }

    //выборка по текущему складу

    void makeStockSelection()
    {
        if(stock->currentIndex()!=0)
        {
            selectDialogQuery.exec("select id_stock from stock_list where stock_description = '"+stock->currentText()+"'");
            selectDialogQuery.next();
            QString id_st = selectDialogQuery.value(0).toString();
            qDebug()<<id_st;
           QString nSpCommand = "(select name, position  , stock_description, personalize from (select name AS name, position AS position, n_stock as n_stock from user_list, user_and_order_list where user_list.id = user_and_order_list.id "
                    " and user_and_order_list.n_stock = "
                    +id_st
                    +" group by name, position, n_stock ) as foo,stock_list  where foo.n_stock = stock_list.id_stock)"+
                    " union (select name, position  , stock_description , personalize from (select name AS name, position AS position, id_stock as n_stock  from user_list, userPersSt where user_list.id = userPersSt.id_user"
                    +" and userPersSt.id_stock = "
                    +id_st
                    +" group by name, position, id_stock ) as foo1,stock_list  where foo1.n_stock = stock_list.id_stock)";
            QString spCommand =
                    "(select name, stock_desc, personality,n_ord, amount from user_and_order_list,user_list,not_pers_stock_list where user_list.id = user_and_order_list.id"
                    " and n_ord='' and user_and_order_list.n_stock = "
                    +id_st+") union (select name, stock_desc, personality,user_and_order_list.n_ord, amount from user_and_order_list,user_list,pers_stock_list where user_list.id = user_and_order_list.id"
                                                           " and n_stock=pers_stock_list.id_stock and user_and_order_list.n_ord =pers_stock_list.n_ord and user_and_order_list.n_stock = "
                                                           +id_st+")" ;
  spCommand="select name, stock_desc, personality,n_ord, det_amount from user_and_order_list,user_list where user_list.id = user_and_order_list.id and user_and_order_list.n_stock = "
          +id_st+"" ;
            QList<QString> header1;
            QList<QString> header2;
            header1<<"Имя"<<"Должность"<<"Склад"<<"Персонализированный";
            header2<<"Имя"<<"Склад"<<"Персонализированный"<<"Номер Заказа"<<"Количество деталей";
            emit sendName(0);
            emit sendCommands(nSpCommand,spCommand,header1,header2);
            done(1);
        }
        else
        {
            QMessageBox message;
            message.setText("Невозможно сделать выборку.  Склад не выбран");
            message.exec();
        }
    }


    //выборка по текущей должности

    void makePositionSelection()
    {
        if(position->currentIndex()!=0)
        {

            QString nSpCommand =" select name, position, login, password from user_list where position=  '"
                    +position->currentText()+"'";
            QList<QString> header1;
            header1<<"Имя"<<"Должность"<<"Логин"<<"Пароль";
            emit sendName(0);
            emit sendCommands(nSpCommand,nSpCommand,header1,header1);
            done(1);
        }
        else
        {
            QMessageBox message;
            message.setText("Невозможно сделать выборку. должность не выбрана");
            message.exec();
        }
    }

    //выборка по текущей сфере деятельности

    void makeSphereSelection()
    {
        if(spere->currentIndex()!=0)
        {

            QString nSpCommand =" select name, position, login, password from user_list where sphere=  '"
                    +spere->currentText()+"'";
            QList<QString> header1;
            header1<<"Имя"<<"Должность"<<"Логин"<<"Пароль";

            emit sendName(0);
            emit sendCommands(nSpCommand,nSpCommand,header1,header1);
            done(1);
        }
        else
        {
            QMessageBox message;
            message.setText("Невозможно сделать выборку. Сфера деятельности не выбрана");
            message.exec();
        }
    }

    void makeOrderSelection()
    {
        if(order->currentIndex()!=0)
        {


            QString spCommand ="select name,operation_description, ordNumber, state from user_list, operation_list,concr_order_operation_list"
                    " where user_list.id = concr_order_operation_list.worker and operation_list.id_operation = concr_order_operation_list.operNumber"
                    " and concr_order_operation_list.ordNumber ='"+order->currentText()+"'";

            QList<QString> header1;
            QList<QString> header2;
            //header1<<"Имя"<<"Должность"<<"Oперация"<<"Количество заказов";
            header2<<"Имя"<<"Операция"<<"Номер Заказа"<<"Статус";
            emit sendName(0);
            emit sendCommands(spCommand,spCommand,header2,header2);
            done(1);
        }
        else
        {
            QMessageBox message;
            message.setText("Невозможно сделать выборку. заказ не выбран");
            message.exec();
        }
    }
signals:
    void callQuery();
    void sendCommands(QString unSp,QString Sp, QList<QString> nspHead,  QList<QString> spHead);
    void sendName(int name);
};

#endif // USERSELECTDIALOG_H
