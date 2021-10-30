#ifndef OPENDIALOG_H
#define OPENDIALOG_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QComboBox>
#include <QTextStream>
#include <QFile>
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QMessageBox>
using namespace std;

class openDatabaseDialog:public QDialog
{
    Q_OBJECT
public:

    openDatabaseDialog()
    {
        setWindowTitle("Производство(вход)");
        QVBoxLayout* layout  = new QVBoxLayout;
        QLabel* label6       = new QLabel("Общие параметры подключения:");
        QLabel* label1       = new QLabel("Имя хоста:");
        QLabel* label2       = new QLabel("Порт:");
        QLabel* label3       = new QLabel("Имя базы для подключения:");
        QLabel* label4       = new QLabel("Роль в базе:");
        QLabel* label5       = new QLabel("Пароль для подключения к базе данных:");
        QLabel* label7       = new QLabel("Локальные параметры подключения:");
        QLabel* label8       = new QLabel("Логин:");
        QLabel* label9       = new QLabel("Пароль:");

        QString host   = "";
        QString port   = "";
        QString dbName = "";
        QString usName = "";
        QString login  = "";

        if(QFile::exists("enterParams.txt"))
        {
            QFile file("enterParams.txt");
            file.open(QIODevice::ReadOnly);
            QTextStream in(&file);
            in>>host;
            in>>port;
            in>>dbName;
            in>>usName;
            in>>login;
            file.close();
        }

        HostName           = new QLineEdit(host);
        Port               = new QLineEdit(port);
        DatabaseName       = new QLineEdit(dbName);
        UserName           = new QLineEdit(usName);
        Login              = new QLineEdit(login);
        basePassword       = new QLineEdit;
        Password           = new QLineEdit;
        basePassword->setEchoMode(QLineEdit::Password);
        Password->setEchoMode(QLineEdit::Password);

        newParams          = new QPushButton("Открыть базу данных");

        layout->addWidget(label6);
        layout->addWidget(label1);
        layout->addWidget(HostName);
        layout->addWidget(label2);
        layout->addWidget(Port);
        layout->addWidget(label3);
        layout->addWidget(DatabaseName);
        layout->addWidget(label4);
        layout->addWidget( UserName);
        layout->addWidget(label5);
        layout->addWidget(basePassword);
        layout->addSpacing(30);
        layout->addWidget(label7);
        layout->addWidget(label8);
        layout->addWidget(Login);
        layout->addWidget(label9);
        layout->addWidget(Password);
        layout->addWidget(newParams);
        setLayout(layout);
        connect( newParams , SIGNAL(clicked()), this , SLOT(opNewBase()));
    }

private:
    QLineEdit* HostName;
    QLineEdit* Port;
    QLineEdit* DatabaseName;
    QLineEdit* UserName;
    QLineEdit* basePassword;
    QLineEdit* Password;
    QLineEdit* Login;
    QPushButton *newParams;

public slots:

    void opNewBase()
    {
        QFile file("enterParams.txt");
        file.open(QIODevice::WriteOnly);
        QTextStream out(&file);
        out << HostName->text();
        out << " ";
        out << Port->text();
        out << " ";
        out << DatabaseName->text();
        out << " ";
        out << UserName->text();
        out << " ";
        out << Login->text();
        out << " ";
        out << basePassword->text();
        out << " ";
        out << Password->text();
        file.close();

        QSqlDatabase dbg = QSqlDatabase::addDatabase("QPSQL","mainconnection");
        dbg.setHostName(HostName->text());
        dbg.setPort(Port->text().toInt());
        dbg.setDatabaseName(DatabaseName->text());
        dbg.setUserName(UserName->text());
        dbg.setPassword(basePassword->text());
        dbg.open();
        if(dbg.isOpen())
        {
            QSqlQuery myQuery(dbg);

            myQuery.exec("select id, position, sphere from user_list where login = '"+Login->text()+"' and password = '"+Password->text()+"'");
            qDebug()<<myQuery.lastError();
            myQuery.next();
            if(myQuery.isValid())
            {
                if(myQuery.size()!=0)
                {
                    qDebug()<<myQuery.value(2).toString();
                    if(myQuery.value(1).toString()=="Админ")
                        done(1);
                    else {
                        if(myQuery.value(2).toString()=="Выпуск")
                        {
                            QFile file1("workerId.txt");
                            file1.open(QIODevice::WriteOnly);
                            QTextStream out1(&file1);
                            out1<<myQuery.value(0).toString();
                            file1.close();
                            done(2);
                        }
                        else {
                            if(myQuery.value(2).toString()=="Услуги")
                            {
                                QFile file1("workerId.txt");
                                file1.open(QIODevice::WriteOnly);
                                QTextStream out1(&file1);
                                out1<<myQuery.value(0).toString();
                                file1.close();
                                qDebug()<<myQuery.value(2).toString();
                                done(3);
                            }
                            else
                            {
                                if(myQuery.value(2).toString()=="Закупка")
                                {
                                    QFile file1("workerId.txt");
                                    file1.open(QIODevice::WriteOnly);
                                    QTextStream out1(&file1);
                                    out1<<myQuery.value(0).toString();
                                    file1.close();
                                    qDebug()<<myQuery.value(2).toString();
                                    done(4);
                                }
                                else
                                {
                                    if(myQuery.value(2).toString()=="Доставка")
                                    {
                                        QFile file1("workerId.txt");
                                        file1.open(QIODevice::WriteOnly);
                                        QTextStream out1(&file1);
                                        out1<<myQuery.value(0).toString();
                                        file1.close();
                                        qDebug()<<myQuery.value(2).toString();
                                        done(5);
                                    }
                                    else
                                    done(6);
                                }
                            }
                        }
                    }
                    QSqlDatabase :: removeDatabase ( "mainconnection" );
                }
                else
                {
                    QMessageBox mes;
                    mes.setText("неверное имя пользователя или пароль ");
                    mes.exec();
                    Password->setText("");
                }
            }
            else
            {
                myQuery.exec("select id, position, sphere from user_list");
                myQuery.next();
                qDebug()<<myQuery.lastError();
                if(!myQuery.isValid())
                {
                    QMessageBox mes;
                    mes.setText("данная база не участвует в процессе производства. Хотите начать новое производство?");

                    mes.setStandardButtons(QMessageBox::Yes);
                    mes.addButton(QMessageBox::No);
                    mes.setDefaultButton(QMessageBox::No);
                    mes.setButtonText(QMessageBox::Yes, tr("Да"));
                    mes.setButtonText(QMessageBox::No, tr("Нет"));
                    if(mes.exec() == QMessageBox::Yes)
                    {
                        myQuery.exec("drop table  user_list");
                        myQuery.exec("drop table  size_list");
                        myQuery.exec("drop table  name_list");
                        myQuery.exec("drop table  order_list");
                        myQuery.exec("drop table  concr_order_operation_list");
                        myQuery.exec("drop table  position_list");
                        myQuery.exec("drop table  user_and_oper_list");
                        myQuery.exec("drop table  user_and_order_list");
                        myQuery.exec("drop table  stock_list ");
                        myQuery.exec("drop table  not_pers_stock_list");
                        myQuery.exec("drop table  pers_stock_list");
                        myQuery.exec("drop table  id_stock_list");
                        myQuery.exec("drop table  id_stock_list");
                        myQuery.exec("drop table  id_oper_list");
                        myQuery.exec("drop table  operation_list");
                        myQuery.exec("drop table  userPersSt");
                        myQuery.exec("drop table  minimalAmount");
                        myQuery.exec("drop table  delivery_list");
                        myQuery.exec("create table user_list(id int, name varchar, position varchar, sphere varchar, login varchar,  password varchar, primary key (id))");
                        myQuery.exec("insert into user_list(id, name, position, sphere, login,  password) values (1, 'стд.имя','Админ','','"+Login->text()+"','"+Password->text()+"')");

                        QMessageBox mesage;
                        mesage.setText("Начато новое производство в выбранной вами базе данных. Вы - администратор,"
                                       " для входа используйте введенный вами логин и пароль."
                                       " Eсли по какой-то причине они вас не устраивают - вы можете изменить их программно");
                        mesage.exec();
                        done(1);
                    }
                    else {
                      // do something else
                    }
                    //mes.exec();

                }


            }

        }

        else
        {
            QMessageBox mes;
            mes.setText("Невозможно подключиться к базе данных. вероятно, в параметрах подключения допущена ошибка");
            mes.exec();
        }

    }




};
#endif // OPENDIALOG_H
