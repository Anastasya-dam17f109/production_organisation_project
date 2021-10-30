#ifndef USERSDIALOG_H
#define USERSDIALOG_H

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

class usersDialog: public QDialog// дайлог для изменения текущего именни продукции
{
    Q_OBJECT
public:
    QWidget* firstPage;
    QWidget* addPage;
    QWidget* deletePage;
    QWidget* updatePage;
    QWidget* scrollWidg;
    QWidget* scrollWidgUpd;

    QWidget* operationPage;
    QWidget* stockPage;
    QWidget* managerPage;

    QWidget* operationPageUpd;
    QWidget* stockPageUpd;

    QLineEdit *addLine1;
    QLineEdit *addLine2;
    QLineEdit *addLine3;

    QLineEdit *lineName;
    QLineEdit *lineLogin;
    QLineEdit *linePassword;

    QButtonGroup* addGroopOper;
    QButtonGroup* addGroopStock;
    QButtonGroup* updateGroopOper;
    QButtonGroup* updateGroopStock;

    QComboBox *pos;
    QComboBox *name;
    QComboBox *nameForUpdate;
    QComboBox *positionForUpdate;
    QComboBox *spereForUpdate;

    QScrollArea* scroll;
    QScrollArea* scrollUpdate;
    QStackedLayout *stackedLayout;
    QStackedLayout *stackedLayout1;
    QStackedLayout *stackedLUpdate;

    QSqlQuery userDialogQuery;
    QList<QString> posDescr;

    QPushButton* nextButton;
    QPushButton* delButton;
    QPushButton* updateButton;
    int currIdx = 0;
    int currIdxGrBox = 0;

    bool changeSphereFlag = false;

    usersDialog(QList<QString> positions,QList<QString> description)
    {
        resize(400,500);
        setWindowTitle("Редактор списка пользователей");
        callQuery();
        QList<QString> operations;
        QList<QString> stocks;
        QList<QString> user_names;
        QList<QString> spheres;

        userDialogQuery.exec("select id_operation , operation_description from operation_list"
                             "  group by  id_operation, operation_description  ");
        qDebug()<<userDialogQuery.lastError();
        while(userDialogQuery.next())
            operations<< userDialogQuery.value(1).toString();

        userDialogQuery.exec("select stock_description from stock_list");
        while(userDialogQuery.next())
            stocks<< userDialogQuery.value(0).toString();

        userDialogQuery.exec("select name from user_list");
        user_names<<"";
        while(userDialogQuery.next())
            user_names<< userDialogQuery.value(0).toString();


        spheres<<""<<"Выпуск"<<"Доставка"<<"Услуги"<<"Закупка";


        pos               = new QComboBox;
        name              = new QComboBox;
        nameForUpdate     = new QComboBox;
        positionForUpdate = new QComboBox;
        spereForUpdate    = new QComboBox;

        lineLogin         = new QLineEdit;
        lineName          = new QLineEdit;
        linePassword      = new QLineEdit;
        addLine1          = new QLineEdit;
        addLine2          = new QLineEdit;
        addLine3          = new QLineEdit;

        addGroopOper = new QButtonGroup;
        addGroopStock = new QButtonGroup;
        updateGroopOper = new QButtonGroup;
        updateGroopStock = new QButtonGroup;

        pos->addItems(positions);
        name->addItems(user_names);
        positionForUpdate->addItems(positions);
        nameForUpdate->addItems(user_names);
        spereForUpdate->addItems(spheres);

        operationPage    = createOperationPage(operations,addGroopOper);
        stockPage        = createStockPage(stocks,addGroopStock);
        operationPageUpd = createOperationPage(operations,updateGroopOper);
        stockPageUpd     = createStockPage(stocks,updateGroopStock);
        posDescr = description;

        scrollUpdate     = new QScrollArea;
        scroll           = new QScrollArea;
        scrollWidg       = new QWidget;
        scrollWidgUpd    = new QWidget;
        stackedLayout1   = new QStackedLayout;
        stackedLUpdate   = new QStackedLayout;

        scrollWidg ->setLayout(createScrollPart(stackedLayout1,createManagerPage(), operationPage,stockPage));
        scrollWidgUpd ->setLayout(createScrollPart(stackedLUpdate,createManagerPage(), operationPageUpd,stockPageUpd));
        createFirstPage();
        createAddPage();
        createDeletePage();
        createUpdatePage();
        //

        QVBoxLayout *layout = new QVBoxLayout;
        stackedLayout = new QStackedLayout;
        stackedLayout->addWidget(firstPage);
        stackedLayout->addWidget(addPage);
        stackedLayout->addWidget(deletePage);
        stackedLayout->addWidget(updatePage);
        layout->addLayout(stackedLayout);
        setLayout(layout);
    }

    void createFirstPage()
    {
        firstPage           = new QWidget;
        QWidget * wid       = new QWidget;
        QLabel      * label = new QLabel("ВЫБЕРИТЕ ДЕЙСТВИЕ");
        QVBoxLayout *layout = new QVBoxLayout;
        QHBoxLayout* lay    = new QHBoxLayout;
        nextButton          = new QPushButton("Далее");
        QRadioButton* b1    = new QRadioButton("Добавить пользователя");
        QRadioButton* b2    = new QRadioButton("Удалить пользователя");
        QRadioButton* b3    = new QRadioButton("Редактировать данные пользователя");
        QButtonGroup* groop = new QButtonGroup();
        nextButton ->setEnabled(false);
        label ->setFixedHeight(20);
        groop->setExclusive(true);
        groop->addButton(b1);
        groop->addButton(b2);
        groop->addButton(b3);

        lay->addSpacing(200);
        lay->addWidget(nextButton);;
        wid->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label);
        layout->addSpacing(100);
        layout->addWidget(b1);
        layout->addSpacing(40);
        layout->addWidget(b2);
        layout->addSpacing(40);
        layout->addWidget(b3);
        layout->addSpacing(100);
        layout->addWidget(wid);

        firstPage->setLayout(layout);

        connect(nextButton, SIGNAL(clicked()), this, SLOT(nextPage()));
        connect(groop, SIGNAL(buttonClicked(int )), this, SLOT(setCurrIdx(int )));
    }

    //создания виджета-добавления пользователя

    void createAddPage()
    {
        addPage              = new QWidget;
        QWidget     * wid1   = new QWidget;
        QLabel      *label1  = new QLabel("ВВЕДИТЕ ИМЯ ПОЛЬЗОВАТЕЛЯ");
        QLabel      *label2  = new QLabel("ВЫБЕРИТЕ ДОЛЖНОСТЬ");
        QLabel      *label3  = new QLabel("ВВЕДИТЕ ЛОГИН");
        QLabel      *label4  = new QLabel("ВВЕДИТЕ ПАРОЛЬ");
        QLabel      *label5  = new QLabel("ВЫБЕРИТЕ ПОЛЕ ДЕЯТЕЛЬНОСТИ");

        QVBoxLayout *layout  = new QVBoxLayout;
        QHBoxLayout* lay     = new QHBoxLayout;
        QPushButton* button1 = new QPushButton("Назад");
        QPushButton* button2 = new QPushButton("Добавить");

        label1->setFixedHeight(20);
        label2->setFixedHeight(20);
        label3->setFixedHeight(20);
        label4->setFixedHeight(20);
        label5->setFixedHeight(20);

        lay->addWidget(button1);
        lay->addSpacing(200);
        lay->addWidget(button2);;
        wid1->setLayout(lay);


        scroll->setWidgetResizable(true);
        scroll->setWidget(scrollWidg);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label1);
        layout->addWidget(addLine1);
        layout->addWidget(label3);
        layout->addWidget(addLine2);
        layout->addWidget(label4);
        layout->addWidget(addLine3);
        layout->addWidget(label2);
        layout->addWidget(pos);
        layout->addWidget(label5);
        layout->addWidget(scroll);
        layout->addWidget(wid1);

        connect(button1, SIGNAL(clicked()), this, SLOT(backPage()));
        connect(button2, SIGNAL(clicked()), this, SLOT(setInformation()));
        connect(pos, SIGNAL(currentIndexChanged(int)), this, SLOT(setChosenWidg(int)));

        addPage->setLayout(layout);
    }

    //создания виджета-удаления пользователя

    void createDeletePage()
    {
        deletePage = new QWidget;
        QWidget     * wid1   = new QWidget;
        QPushButton* button1 = new QPushButton("Назад");
        delButton            = new QPushButton("Удалить");
        QHBoxLayout* lay     = new QHBoxLayout;
        QVBoxLayout *layout  = new QVBoxLayout;
        QLabel      *label1  = new QLabel("ВЫБЕРИТЕ ПОЛЬЗОВАТЕЛЯ ДЛЯ УДАЛЕНИЯ ");

        delButton->setEnabled(false);
        label1->setFixedHeight(20);
        lay->addWidget(button1);
        lay->addSpacing(200);
        lay->addWidget(delButton);;
        wid1->setLayout(lay);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label1);
        layout->addSpacing(100);
        layout->addWidget(name);
        layout->addSpacing(250);
        layout->addWidget(wid1);
        deletePage->setLayout(layout);

        connect(name, SIGNAL(currentIndexChanged(int)), this, SLOT(setDelButtonEnabled(int)));
        connect(button1, SIGNAL(clicked()), this, SLOT(backPage()));
        connect(delButton, SIGNAL(clicked()), this, SLOT(deleteInformation()));
    }

    //создания виджета-редакции пользователя

    void createUpdatePage()
    {
        updatePage           = new QWidget;
        QHBoxLayout* lay     = new QHBoxLayout;
        QVBoxLayout *layout  = new QVBoxLayout;
        QWidget     * wid1   = new QWidget;
        QPushButton* button1 = new QPushButton("Назад");
        updateButton         = new QPushButton("Сохранить изменения");
        QLabel      *label1  = new QLabel("ВЫБЕРИТЕ ПОЛЬЗОВАТЕЛЯ ДЛЯ РЕДАКЦИИ");
        QLabel      *label2  = new QLabel("ИМЯ");
        QLabel      *label3  = new QLabel("ЛОГИН");
        QLabel      *label4  = new QLabel("ПАРОЛЬ");
        QLabel      *label5  = new QLabel("ПОЛЕ ДЕЯТЕЛЬНОСТИ");
        QLabel      *label6  = new QLabel("СФЕРА ДЕЯТЕЛЬНОСТИ");
        QLabel      *label7  = new QLabel("ДОЛЖНОСТЬ");

        positionForUpdate->setEnabled(false);
        spereForUpdate->setEnabled(false);
        lineLogin->setReadOnly(true);
        lineName->setReadOnly(true);
        linePassword->setReadOnly(true);

        label1->setFixedHeight(20);
        label2->setFixedHeight(20);
        label3->setFixedHeight(20);
        label4->setFixedHeight(20);
        label5->setFixedHeight(20);
        label6->setFixedHeight(20);
        label7->setFixedHeight(20);

        lay->addWidget(button1);
        lay->addSpacing(200);
        lay->addWidget(updateButton);;
        wid1->setLayout(lay);
        connect(button1, SIGNAL(clicked()), this, SLOT(backPage()));

        updateButton->setEnabled(false);
        scrollUpdate->setWidgetResizable(true);
        scrollUpdate->setWidget(scrollWidgUpd);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(label1);
        layout->addWidget(nameForUpdate);
        layout->addWidget(label2);
        layout->addWidget(lineName);
        layout->addWidget(label3);
        layout->addWidget(lineLogin);
        layout->addWidget(label4);
        layout->addWidget(linePassword);
        layout->addWidget(label7);
        layout->addWidget(positionForUpdate);
        layout->addWidget(label6);
        layout->addWidget(spereForUpdate);
        layout->addWidget(label5);
        layout->addWidget(scrollUpdate);
        layout->addWidget(wid1);

        connect(nameForUpdate, SIGNAL(currentIndexChanged(int)), this, SLOT(setUpdateEnabled(int)));
        connect(positionForUpdate, SIGNAL(currentIndexChanged(int)), this, SLOT(changePosition(int)));
        connect(spereForUpdate, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSphere(int)));
        connect(updateButton, SIGNAL(clicked()), this, SLOT(setUserChanges()));
        updatePage->setLayout(layout);
    }

    //создания виджета для кнопок с операциями

    QWidget* createOperationPage(QList<QString> operId,QButtonGroup* groopOper)
    {
        QWidget* operationPageNew = new QWidget;
        QVBoxLayout *lay1         = new QVBoxLayout;

        groopOper->setExclusive(false);
        for(auto i = operId.begin(); i!=operId.end();++i)
        {
            QCheckBox* b1 = new QCheckBox(*i);
            groopOper->addButton(b1);
            lay1->addWidget(b1);
        }
        operationPageNew->setLayout(lay1);
        return operationPageNew;
    }

    //создания виджета для кнопок со складами

    QWidget*  createStockPage(QList<QString> stock_id,QButtonGroup* groopStock)
    {
        QWidget* stockPageNew = new QWidget;
        QVBoxLayout *lay1     = new QVBoxLayout;

        groopStock->setExclusive(false);
        for(auto i = stock_id.begin(); i!=stock_id.end();++i)
        {
            QCheckBox* b1 = new QCheckBox(*i);
            groopStock->addButton(b1);
            lay1->addWidget(b1);
        }
        stockPageNew->setLayout(lay1);
        return stockPageNew;
    }

    //создания наполнения для прокручиваемого виджека с кнопками

    QVBoxLayout * createScrollPart(QStackedLayout* stack,QWidget* manag, QWidget*oper,QWidget*stock)
    {
        QVBoxLayout *lay = new QVBoxLayout;
        stack->addWidget(manag);
        stack->addWidget(oper);
        stack->addWidget(stock);
        lay->setAlignment(Qt::AlignTop);
        lay->addLayout(stack);
        return lay;
    }



    QWidget* createManagerPage()
    {
        return(new QWidget);
    }



    void setButtonsUnchecked(QButtonGroup* groopOper)
    {
        QList<QAbstractButton*> check1 = groopOper->buttons();
        for(auto i = check1.begin(); i!=check1.end();++i)
            (*i)->setChecked(false);
    }


public slots:

//перейти на выбранную вкладку

    void nextPage()
    {
        stackedLayout->setCurrentIndex(-(currIdx)-1);
    }

//вернуться на вкладку назад

    void backPage()
    {
        //для страницы добавления
        setButtonsUnchecked(addGroopOper);
        setButtonsUnchecked(addGroopStock);
        setButtonsUnchecked(updateGroopOper);
        setButtonsUnchecked(updateGroopStock);

        addLine1->clear();
        addLine2->clear();
        addLine3->clear();
        lineLogin->clear();
        lineName->clear();
        linePassword->clear();
        lineLogin->setReadOnly(true);
        lineName->setReadOnly(true);
        linePassword->setReadOnly(true);

        pos->setCurrentIndex(0);
        name->setCurrentIndex(0);
        nameForUpdate->setCurrentIndex(0);
        positionForUpdate->setCurrentIndex(0);
        spereForUpdate->setCurrentIndex(0);

        positionForUpdate->setEnabled(false);
        spereForUpdate->setEnabled(false);

        stackedLayout->setCurrentIndex(0);
        stackedLayout1->setCurrentIndex(0);
        stackedLUpdate->setCurrentIndex(0);
    }

//индекс кнопки выбора действия

    void setCurrIdx(int idx)
    {
        currIdx = idx;
        nextButton ->setEnabled(true);
    }

//открыть-закрыть возможность удалить пользователя

    void setDelButtonEnabled(int idx)
    {
        if(idx==0)
             delButton->setEnabled(false);
        else
             delButton->setEnabled(true);
    }

 //для виджета редактирования установка данных в зависимоси от выбранного пользователя

    void setUpdateEnabled(int idx)
    {
        if(idx==0)
        {
            updateButton->setEnabled(false);
            lineLogin->clear();
            lineName->clear();
            linePassword->clear();
            lineLogin->setReadOnly(true);
            lineName->setReadOnly(true);
            linePassword->setReadOnly(true);

            setButtonsUnchecked(updateGroopOper);
            setButtonsUnchecked(updateGroopStock);
            positionForUpdate->setCurrentIndex(0);
            spereForUpdate->setCurrentIndex(0);
            positionForUpdate->setEnabled(false);
            spereForUpdate->setEnabled(false);

            stackedLUpdate->setCurrentIndex(0);
        }
        else
        {
            updateButton->setEnabled(true);
            lineLogin->setReadOnly(false);
            lineName->setReadOnly(false);
            linePassword->setReadOnly(false);
            positionForUpdate->setEnabled(true);
            spereForUpdate->setEnabled(true);
            setButtonsUnchecked(updateGroopOper);
            setButtonsUnchecked(updateGroopStock);
            callQuery();
            QList<QString> name_id;
            QString        command;

            userDialogQuery.exec("select id from user_list");
            name_id<<"";
            while(userDialogQuery.next())
                name_id<< userDialogQuery.value(0).toString();

            QString sphere = "select sphere from user_list where id ="+ name_id.at(nameForUpdate->currentIndex());
            userDialogQuery.exec(sphere);
            userDialogQuery.next();
            sphere = userDialogQuery.value(0).toString();

            command = "select name, position, login, sphere, password from user_list where id = "+ name_id.at(nameForUpdate->currentIndex());
            userDialogQuery.exec(command);
            userDialogQuery.next();
            lineName->setText(userDialogQuery.value(0).toString());
            lineLogin->setText(userDialogQuery.value(2).toString());

            linePassword->setText(userDialogQuery.value(4).toString());
            if(positionForUpdate->findText(userDialogQuery.value(1).toString())!=-1)
                positionForUpdate->setCurrentIndex(positionForUpdate->findText(userDialogQuery.value(1).toString()));
            else
                 positionForUpdate->setCurrentIndex(0);
            spereForUpdate->setCurrentIndex(spereForUpdate->findText(sphere));
            if(userDialogQuery.value(1).toString()=="Админ")
            {
                spereForUpdate->setEnabled(false);
                positionForUpdate->setEnabled(false);
            }
            else
            {
                spereForUpdate->setEnabled(true);
                positionForUpdate->setEnabled(true);
            }
            if(sphere =="Выпуск")
            {
                stackedLUpdate->setCurrentIndex(1);
                command = "select oper_desc from user_and_oper_list where id = "+ name_id.at(nameForUpdate->currentIndex());
                userDialogQuery.exec(command);
                qDebug()<<userDialogQuery.lastError();
                QList<QAbstractButton*> check1 = updateGroopOper->buttons();
                userDialogQuery.next();

                for(int i = 0; i<check1.size(); ++i)
                {
                    if(check1.at(i)->text()==userDialogQuery.value(0).toString())
                    {

                        check1.at(i)->setChecked(true);
                        userDialogQuery.next();
                    }
                }


            }
            else
            {
                if(sphere =="Закупка")
                {
                    stackedLUpdate->setCurrentIndex(2);
                    command = "select stock_desc from user_and_order_list where id = "+ name_id.at(nameForUpdate->currentIndex());
                    userDialogQuery.exec(command);
                    QList<QAbstractButton*> check1 = updateGroopStock->buttons();
                    userDialogQuery.next();
                    for(int i = 0; i<check1.size(); ++i)
                    {

                        if(check1.at(i)->text()==userDialogQuery.value(0).toString())
                        {
                            check1.at(i)->setChecked(true);
                            userDialogQuery.next();
                        }
                    }

                    command = "select stock_description from userPersSt  where id_user = "+ name_id.at(nameForUpdate->currentIndex());
                    userDialogQuery.exec(command);
                    userDialogQuery.next();
                    for(int i = 0; i<check1.size(); ++i)
                    {
                        if(check1.at(i)->text()==userDialogQuery.value(0).toString())
                        {
                            check1.at(i)->setChecked(true);
                            userDialogQuery.next();
                        }
                    }

                }
                else
                    stackedLUpdate->setCurrentIndex(0);
            }

        }
    }

//для странички добавления - отправка записаной информации о новом пользователе

    void setInformation()
    {
        if(addLine1->text()!=""&&addLine2->text()!="" && addLine3->text()!=""&&pos->currentIndex()!=0)
        {
            QList<int> numbers;
            if(currIdxGrBox==1)
            {
                int count = 1;
                QList<QString> operationDescription;
                QList<QAbstractButton*> check = addGroopOper->buttons();
                for(auto i = check.begin(); i!=check.end();++i)
                {
                    if((*i)->isChecked())
                        operationDescription<<(*i)->text();
                    qDebug()<<(*i)->text();
                    count++;
                }
               qDebug()<< operationDescription;
                for(int i = 0; i< operationDescription.size(); ++i)
                {
                    userDialogQuery.exec("select id_operation from operation_list where operation_description = '"
                                         +operationDescription.at(i)+"' group by id_operation");
                    userDialogQuery.next();
                    numbers<<userDialogQuery.value(0).toInt();
                }
            }
            if(currIdxGrBox==2)
            {
                int count = 1;
                QList<QString> operationDescription;
                QList<QAbstractButton*> check = addGroopStock->buttons();
                for(auto i = check.begin(); i!=check.end();++i)
                {
                    if((*i)->isChecked())
                        operationDescription<<(*i)->text();
                    qDebug()<<(*i)->text();
                    count++;
                }
                for(int i = 0; i< operationDescription.size(); ++i)
                {
                    userDialogQuery.exec("select id_stock from stock_list where stock_description = '"
                                         +operationDescription.at(i)+"' group by id_stock");
                    userDialogQuery.next();
                    numbers<<userDialogQuery.value(0).toInt();
                }
            }
            emit createNewUser(addLine1->text(),addLine2->text(),addLine3->text(),pos->currentText(),numbers);
            done(1);
        }
        else
        {
            QMessageBox message;
            message.setText("Заполните все поля");
            message.exec();
        }
    }

//непосредственная редакция  информации о новом пользователе

     void setUserChanges()
    {
         if(lineLogin->text()!="" && lineName->text()!="" &&
                 linePassword->text()!="" &&
                 positionForUpdate->currentIndex()!=0 &&
                 spereForUpdate->currentIndex()!=0)
         {
             callQuery();
             QList<QString> name_id;
             QString        command;

             userDialogQuery.exec("select id from user_list");
             name_id<<"";
             while(userDialogQuery.next())
                 name_id<< userDialogQuery.value(0).toString();

             QString id = name_id.at(nameForUpdate->currentIndex());
             QString sphere = "select sphere from user_list where id ="+ id;
             userDialogQuery.exec(sphere);
             userDialogQuery.next();
             sphere = userDialogQuery.value(0).toString();

             command = "select name, position, login, sphere, password from user_list where id = "+ id;
             userDialogQuery.exec(command);
             userDialogQuery.next();
             QString oldName = userDialogQuery.value(0).toString();
             QString oldlogin = userDialogQuery.value(2).toString();
             QString oldPassword = userDialogQuery.value(4).toString();
             QString oldPosition = userDialogQuery.value(1).toString();

             if(oldName!=lineName->text())
             {
                 command = "update user_list set name = '"+lineName->text()+"' where id = "+ id;
                 userDialogQuery.exec(command);
             }
             if(oldlogin!=lineLogin->text())
             {
                 command = "update user_list set login = '"+lineLogin->text()+"' where id = "+ id;
                 userDialogQuery.exec(command);
             }
             if(oldPassword!=linePassword->text())
             {
                 command = "update user_list set password = '"+linePassword->text()+"' where id = "+ id;
                 userDialogQuery.exec(command);
             }
             if(oldPosition !=positionForUpdate->currentText())
             {
                 if(oldPosition !="Админ")
                 {
                     command = "update user_list set position = '"+positionForUpdate->currentText()+"' where id = "+ id;
                 userDialogQuery.exec(command);
                 }
             }
             if(sphere !=spereForUpdate->currentText())
             {
                 userDialogQuery.exec("select sphere, name from user_list where id = " + id);
                 userDialogQuery.next();
                 QString sphere = userDialogQuery.value(0).toString();
                 QString name   = userDialogQuery.value(1).toString();
                 QList<QString> orders;
                 userDialogQuery.exec("select n_ord from user_and_order_list where id = " + id+" group by n_ord");
                 while(userDialogQuery.next())
                     orders<<userDialogQuery.value(0).toString();
                 command = "update user_list set sphere = '"+spereForUpdate->currentText()+"' where id = "+ id;
                 userDialogQuery.exec(command);
                 emit updateUserTable();

                 //если изменилась полностью сфера деятельности работника, то нужно или поставить неопределенность в заказах и операциях,
                 //иди удалить ответственность за деталь, если он кладовщик

                 userDialogQuery.exec("delete from  user_and_oper_list  where id = "+ id);
                 userDialogQuery.exec( "delete from  userPersSt   where id_user = "+ id);

                 if(sphere == "Выпуск")
                 {
                     userDialogQuery.exec("update user_and_order_list set id = null  where id = " + id);
                     userDialogQuery.exec("update concr_order_operation_list  set  worker = null  where  worker = " + id);
                 }
                 else
                 {
                     if(sphere == "Услуги")
                     {
                         for(int i = 0; i<orders.size();++i)
                         {
                             userDialogQuery.exec("update order_list  set  responsible = 'не определен'  where  ordNumber = '" +orders.at(i)+"'");
                             userDialogQuery.exec("update order_list  set  resp_id = null  where  ordNumber = '" +orders.at(i)+"'");
                         }
                         userDialogQuery.exec("update user_and_order_list set id = null  where id = " + id);
                     }
                     if(sphere == "Закупка")
                     {
                         userDialogQuery.exec("delete from user_and_order_list where id = " + id);
                     }
                     if(sphere=="Доставка")
                     {

                         userDialogQuery.exec("update delivery_list set id_deliver = -1 where id_deliver = "+ id);
                     }
                     //разобраться с курьером
                 }

                 //окончание этого блока
                 if(spereForUpdate->currentText() == "Выпуск"||spereForUpdate->currentText() == "Закупка")
                 {
                     QList<int> numbers;
                     if(spereForUpdate->currentText() == "Выпуск")
                     {
                         int count = 1;
                         QList<QAbstractButton*> check = updateGroopOper->buttons();
                         QList<QString> operationDescription;
                         for(auto i = check.begin(); i!=check.end();++i)
                         {
                             if((*i)->isChecked())
                                 operationDescription<<(*i)->text();
                             qDebug()<<(*i)->text();
                             count++;
                         }

                         for(int i = 0; i< operationDescription.size(); ++i)
                         {
                             userDialogQuery.exec("select id_operation from operation_list where operation_description = '"
                                                  +operationDescription.at(i)+"' group by id_operation");
                             userDialogQuery.next();
                             numbers<<userDialogQuery.value(0).toInt();
                         }
                     }
                     if(spereForUpdate->currentText() == "Закупка")
                     {
                         int count = 1;
                         QList<QAbstractButton*> check = updateGroopStock->buttons();
                         QList<QString> operationDescription;
                         for(auto i = check.begin(); i!=check.end();++i)
                         {
                             if((*i)->isChecked())
                                 operationDescription<<(*i)->text();
                             qDebug()<<(*i)->text();
                             count++;
                         }
                         for(int i = 0; i< operationDescription.size(); ++i)
                         {
                             userDialogQuery.exec("select id_stock from stock_list where stock_description = '"
                                                  +operationDescription.at(i)+"' group by id_stock");
                             userDialogQuery.next();
                             numbers<<userDialogQuery.value(0).toInt();
                         }
                     }
                     emit updateUser(id, spereForUpdate->currentText(), numbers);
                 }
                 if(spereForUpdate->currentText() == "Услуги"|| spereForUpdate->currentText() =="Доставка")
                 {
                     userDialogQuery.exec("select order_amount from  user_and_oper_list where id = "+ id);
                     if(userDialogQuery.size() ==0)
                         userDialogQuery.exec("insert into user_and_oper_list (id,oper_id,oper_desc,order_amount,stock_amount,room_amount, equipment_amount) values ("+ id+",-1,'',0,-1,-1,-1)" );
                 }

                 done(1);
             }
             else
             {
                 QList<QString> numbers;
                 QList<QString> operationDescription;
                 if(spereForUpdate->currentText() == "Выпуск")
                 {
                     int count = 1;
                     QList<QAbstractButton*> check = updateGroopOper->buttons();
                     // создание списка всех отмеченных операций
                     for(auto i = check.begin(); i!=check.end();++i)
                     {
                         if((*i)->isChecked())
                             operationDescription<<(*i)->text();
                         qDebug()<<(*i)->text();
                         count++;
                     }
                    qDebug()<< operationDescription;
                     for(int i = 0; i< operationDescription.size(); ++i)
                     {
                         userDialogQuery.exec("select id_operation from operation_list where operation_description = '"
                                              +operationDescription.at(i)+"' group by id_operation");
                         userDialogQuery.next();
                         numbers<<userDialogQuery.value(0).toString();
                     }
                     // добавление тех, которых не было до этого
                     for(auto i = numbers.begin(); i!=numbers.end();++i)
                     {
                         command = "select oper_desc from   user_and_oper_list  where id = "+ id+" and oper_id = " + (*i);
                         userDialogQuery.exec(command);
                         if(userDialogQuery.size()==0)
                         {
                             command = "select operation_description from operation_list where id_operation = "  + (*i);
                             userDialogQuery.exec(command);
                             userDialogQuery.next();
                             command = "insert into user_and_oper_list(id , oper_id, oper_desc,order_amount, stock_amount, room_amount, equipment_amount) values("+
                                     id+","+ (*i)+",'"+userDialogQuery.value(0).toString()+"',0,-1,-1,-1)" ;
                             userDialogQuery.exec(command);
                         }
                     }
                     // создание списка всех  операций которые есть в базе
                     QList<QString> existsNumb;

                     userDialogQuery.exec("select oper_id  from   user_and_oper_list  where id = "+ id);
                     while(userDialogQuery.next())
                     {
                        existsNumb<<userDialogQuery.value(0).toString();
                     }
                     // создание списка всех  операций которые есть в базеудаление тех, которе не отмечены
                     for(auto i = existsNumb.begin(); i!=existsNumb.end();++i)
                     {
                         if(numbers.indexOf((*i))==-1)
                         {
                             userDialogQuery.exec("delete from  user_and_oper_list where id = "+ id+" and oper_id = "+ (*i));
                             userDialogQuery.exec("update  user_and_order_list set id = null where id = "+ id+" and n_oper = "+ (*i));
                             userDialogQuery.exec("update  concr_order_operation_list set  worker = null where worker = "+ id+" and operNumber = "+ (*i));

                         }
                     }
                     // апгрейд таблиц заказов
                 }

                 if(spereForUpdate->currentText() == "Закупка")
                 {
                     int count = 1;
                     QList<QAbstractButton*> check = updateGroopStock->buttons();
                     for(auto i = check.begin(); i!=check.end();++i)
                     {
                         if((*i)->isChecked())
                         {
                             userDialogQuery.exec("select id_stock from stock_list where stock_description  = '"+ (*i)->text()+"'" );
                             userDialogQuery.next();
                             numbers<<userDialogQuery.value(0).toString();
                         }
                         count++;
                     }
                     userDialogQuery.exec("select stock_amount from user_and_oper_list where id = "+ id );
                     if(userDialogQuery.size()==0)
                         userDialogQuery.exec("insert into  user_and_oper_list (id, oper_id, oper_desc, order_amount, stock_amount, room_amount, equipment_amount) values ("+ id+",-1,'',-1,0,-1,-1)" );
                     command = "update  user_and_oper_list  set  stock_amount  = "+ QString::number(numbers.size())+"  where id = "+ id;
                     userDialogQuery.exec(command);
                     command = "update  user_and_oper_list  set  oper_id =(-1) where id = "+ id;
                     userDialogQuery.exec(command);
                     command = "update  user_and_order_list  set  n_oper =(-1) where id = "+ id;
                     userDialogQuery.exec(command);
                     for(auto i = numbers.begin(); i!=numbers.end();++i)
                     {
                         command = "select oper_desc from   user_and_order_list  where id = "+ id+" and n_stock = " + (*i)+ "group by oper_desc";
                         userDialogQuery.exec(command);
                         if(userDialogQuery.size()==0)
                         {
                             command = "select  personalize from stock_list where id_stock = "  + (*i);
                             userDialogQuery.exec(command);

                             userDialogQuery.next();
                             QString person = userDialogQuery.value(0).toString();
                             command = "select stock_description  from stock_list where id_stock = "  + (*i);
                             userDialogQuery.exec(command);
                             userDialogQuery.next();
                            qDebug()<<person;
                             QString decr   = userDialogQuery.value(0).toString();
                             qDebug()<<decr;
                             if(person=="нет")
                             {
                                 command = "select amount from not_pers_stock_list where id_stock = "  + (*i);
                                 userDialogQuery.exec(command);
                                 userDialogQuery.next();
                                 command = "insert into user_and_order_list( id, n_ord, n_oper,oper_desc,n_stock,stock_desc, n_room, n_equipment, personality, det_amount) values("+
                                         id+", '',-1,'',"+ (*i)+",'"+decr+"',-1,-1,'нет',"+userDialogQuery.value(0).toString()+")";
                                 userDialogQuery.exec(command);
                                  qDebug()<<"ggvgfvgf";
                                 qDebug()<<userDialogQuery.lastError();
                             }
                             else
                             {
                                 QList<QString> ords;
                                 QList<QString> amounts;
                                 userDialogQuery.exec("insert into userPersSt (id_user, id_stock, stock_description) values ("+ id+","+(*i)+",'"+decr+"')");
                                 qDebug()<<userDialogQuery.lastError();
                                 command = "select n_ord, amount  from  pers_stock_list  where id_stock = "+ (*i);
                                 userDialogQuery.exec(command);
                                 while(userDialogQuery.next())
                                 {
                                    ords<<userDialogQuery.value(0).toString();
                                    amounts<<userDialogQuery.value(1).toString();
                                 }
                                 for(int j = 0; j<ords.size();++j)
                                 {
                                     command = "insert into user_and_order_list( id, n_ord, n_oper,oper_desc,n_stock,stock_desc, n_room, n_equipment, personality, det_amount) values("+
                                             id+", '"+ords.at(j)+"',-1,'',"+ (*i)+",'"+decr+"',-1,-1,'да',"+amounts.at(j)+")";
                                     userDialogQuery.exec(command);
                                 }
                             }

                         }
                     }

                     QList<QString> existsNumb;
                     command = "select n_stock  from   user_and_order_list  where id = "+ id;
                     userDialogQuery.exec(command);
                     while(userDialogQuery.next())
                     {
                        existsNumb<<userDialogQuery.value(0).toString();
                     }
                     for(auto i = existsNumb.begin(); i!=existsNumb.end();++i)
                     {
                         if(numbers.indexOf((*i))==-1)
                         {
                             command = "delete from  user_and_order_list where id = "+ id+" and n_stock = "+ (*i);
                             userDialogQuery.exec(command);

                         }
                     }

                     existsNumb.clear();
                     command = "select id_stock  from  userPersSt  where id_user = "+ id;
                     userDialogQuery.exec(command);
                     while(userDialogQuery.next())
                     {
                        existsNumb<<userDialogQuery.value(0).toString();
                     }
                     for(auto i = existsNumb.begin(); i!=existsNumb.end();++i)
                     {
                         if(numbers.indexOf((*i))==-1)
                         {
                             command = "delete from userPersSt where id_user = " + id;
                             userDialogQuery.exec(command);
                         }
                     }

                 }
//                 if(spereForUpdate->currentText() == "Услуги"|| spereForUpdate->currentText() =="Доставка")
//                 {
//                      userDialogQuery.exec("select order_amount from  user_and_oper_list where id = "+ id);
//                             if(userDialogQuery.size() ==0)
//                             userDialogQuery.exec("insert into user_and_oper_list (id,oper_id,oper_desc,order_amount,stock_amount,room_amount, equipment_amount) values ("+ id+",-1,'',0,-1,-1,-1)" );
//                 }
                 done(1);
             }

         }
         else
         {

             callQuery();
             QList<QString> name_id;
             QString        command;

             userDialogQuery.exec("select id from user_list");
             name_id<<"";
             while(userDialogQuery.next())
                 name_id<< userDialogQuery.value(0).toString();

             QString id = name_id.at(nameForUpdate->currentIndex());
             QString sphere = "select sphere from user_list where id ="+ id;
             userDialogQuery.exec(sphere);
             userDialogQuery.next();
             sphere = userDialogQuery.value(0).toString();

             command = "select name, position, login, sphere, password from user_list where id = "+ id;
             userDialogQuery.exec(command);
             userDialogQuery.next();
             QString oldName = userDialogQuery.value(0).toString();
             QString oldlogin = userDialogQuery.value(2).toString();
             QString oldPassword = userDialogQuery.value(4).toString();
             QString oldPosition = userDialogQuery.value(1).toString();
             if(oldPosition =="Админ")
             {
                 if(oldName!=lineName->text())
                 {
                     command = "update user_list set name = '"+lineName->text()+"' where id = "+ id;
                     userDialogQuery.exec(command);
                 }
                 if(oldlogin!=lineLogin->text())
                 {
                     command = "update user_list set login = '"+lineLogin->text()+"' where id = "+ id;
                     userDialogQuery.exec(command);
                 }
                 if(oldPassword!=linePassword->text())
                 {
                     command = "update user_list set password = '"+linePassword->text()+"' where id = "+ id;
                     userDialogQuery.exec(command);
                 }
                  done(1);
                  emit updateUserTable();
             }
else{
         QMessageBox message;
         message.setText("Заполните все поля");
         message.exec();
             }
     }
}

//для странички удаления - отправка записаной информации о удаляемом пользователе

   void deleteInformation()
   {
       callQuery();
       QList<QString> name_id;
       userDialogQuery.exec("select id from user_list");
       name_id<<"";
       while(userDialogQuery.next())
           name_id<< userDialogQuery.value(0).toString();
       emit deleteChosenUser(name_id.at(name->currentIndex()));
       done(1);
   }

//для странички добавления изменение списка кнопок выбора

    void setChosenWidg(int idx)
    {

        if(idx==0)
            stackedLayout1->setCurrentIndex(0);
        else
        {
            currIdxGrBox = idx-1;
            if(posDescr.at(idx-1)=="Выпуск")
            {
                currIdxGrBox = 1;
                stackedLayout1->setCurrentIndex(1);
            }
            else
            {
                currIdxGrBox = 2;
                if(posDescr.at(idx-1)=="Закупка")
                    stackedLayout1->setCurrentIndex(2);
                else
                    stackedLayout1->setCurrentIndex(0);
            }
        }
    }

//для странички редакции - изменение текущей должности

    void changePosition(int idx)
    {
        if(changeSphereFlag == false)
        {
            callQuery();
            QString        command = "select sphere from position_list where position = '" + positionForUpdate->currentText()+"'";
            userDialogQuery.exec(command);
            userDialogQuery.next();
            if(userDialogQuery.value(0).toString()!=spereForUpdate->currentText())
            {
                spereForUpdate->setCurrentIndex(spereForUpdate->findText(userDialogQuery.value(0).toString()));
                if(userDialogQuery.value(0).toString()=="Выпуск")
                    stackedLUpdate->setCurrentIndex(1);
                else
                {
                    if(userDialogQuery.value(0).toString()=="Закупка")
                        stackedLUpdate->setCurrentIndex(2);
                    else
                        stackedLUpdate->setCurrentIndex(0);
                }
            }
        }
    }

 //для странички редакции - изменение текущей сферы деяельности

    void changeSphere(int)
    {
        changeSphereFlag = true;
        callQuery();
        QString        command = "select sphere from position_list where position = '" + positionForUpdate->currentText()+"'";
        userDialogQuery.exec(command);
        userDialogQuery.next();
        if(userDialogQuery.value(0).toString()!=spereForUpdate->currentText())
        {
            positionForUpdate->setCurrentIndex(0);
            if(spereForUpdate->currentText()=="Выпуск")
                stackedLUpdate->setCurrentIndex(1);
            else
            {
                if(spereForUpdate->currentText()=="Закупка")
                    stackedLUpdate->setCurrentIndex(2);
                else
                    stackedLUpdate->setCurrentIndex(0);
            }
        }
         changeSphereFlag = false;
    }



    void setQuery(QSqlQuery newQuery)
    {
        userDialogQuery = newQuery;

    }



signals:

    void createNewUser(QString name, QString login, QString password, QString position, QList<int> operations);
    void callQuery();
    void deleteChosenUser(QString id);
    void updateUser(QString id, QString sphere, QList<int> list);
    void updateUserTable();
};
#endif // USERSDIALOG_H
