/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "graphicEdit/graphwidget.h"
#include "graphicEdit/edge.h"
#include "graphicEdit/node.h"
#include "graphicEdit/edgenode.h"
#include <QGraphicsSceneMouseEvent>
#include <math.h>
#include <QDebug>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QGraphicsSceneDragDropEvent>
#include <QTextEdit>
#include <QObject>
class Node;
//! [0]
//!
GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), timerId(0)
{
    Scene* s = new Scene (-100,-100, 1750, 900, 0,this);
    connect(this, SIGNAL(graphClearSceneSig()),s,SLOT(clearScene()));
    setScene(s);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setFixedHeight(880);
    setWindowTitle(tr("Редактирование производственной схемы"));
    indGraphWidg = 0;

    emit callQuery();
    myQuery.exec("select id_operation from operation_list");
    while(myQuery.next())
         operations<<myQuery.value(0).toInt();
    myQuery.exec("select id_stock from stock_list");
    while(myQuery.next())   
        stocks<<myQuery.value(0).toInt();
}
//! [1]


GraphWidget::GraphWidget(QWidget *parent, int idx)
    : QGraphicsView(parent), timerId(0)
{
    emit callQuery();
    while(myQuery.next())
         operations<<myQuery.value(0).toInt();
    myQuery.exec("select id_stock from stock_list");
    while(myQuery.next())
        stocks<<myQuery.value(0).toInt();
    Scene* s = new Scene (0, 0, 250, 600, 1, this);
    setScene(s);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(150, 600);
    setFixedHeight(600);
    setFixedWidth(150);
    indGraphWidg = 1;
    Node *node1 = new Node(this,"operation");    // блок операций
    node1->setPos(90, 0);
    node1->createProxy();
    node1->index_for_base = findMaxId(operations);
    operations.append(node1->index_for_base);
    s->addItem(node1);
    // блок операций - добавление на сцену

    QGraphicsSimpleTextItem *operText  =new  QGraphicsSimpleTextItem;  // подпись блока операций
    operText->setText("Блок операций");
    QFont font = operText->font();
    font.setPixelSize(16);
    operText->setFont(font);
    operText->setPos(80, 80);
    s->addItem(operText);
    // подпись блока операций  - добавление на сцену

    Node *node2 = new Node(this,"stock");   //  склад
    node2->setPos(90, 250);
    node2->createProxy();
    s->addItem(node2);
    connect(node1, SIGNAL(deletNode(Node* )), this, SLOT(removeNode(Node* )));
    connect(node2, SIGNAL(deletNode(Node* )), this, SLOT(removeNode(Node* )));
    connect(node1, SIGNAL(deletEdgesFromNode(Node* )), this, SLOT(removeEdges(Node* )));
    connect(node2, SIGNAL(deletEdgesFromNode(Node* )), this, SLOT(removeEdges(Node* )));
    connect(node1, SIGNAL(copyedNode(Node* )), this, SLOT(setCopyItem(Node* )));
    connect(node1, SIGNAL(paintNewEdge(Node*, Node*,Node*,Edge*)), this, SLOT(newEdgeOnScene(Node* , Node*, Node*,Edge*)));
    connect(node2, SIGNAL(copyedNode(Node* )), this, SLOT(setCopyItem(Node* )));
    connect(node2, SIGNAL(paintNewEdge(Node*, Node*,Node*,Edge*)), this, SLOT(newEdgeOnScene(Node* , Node*, Node*,Edge*)));
    node1->inscription->setEnabled(false);
    node2->inscription->setEnabled(false);
    node2->index_for_base = findMaxId(stocks);
    stocks.append(node2->index_for_base);
    QGraphicsSimpleTextItem *stockText  =new  QGraphicsSimpleTextItem;  // подпись блока операций
    stockText->setText("       Склад");
    font.setPixelSize(16);
    stockText->setFont(font);
    stockText->setPos(80, 350);
    s->addItem(stockText);    // добавление подписи склада


    Node *node3 = new Node(this,"edge");
    node3->setPos(80, 520);
    s->addItem(node3);

    QGraphicsSimpleTextItem *edgeText  =new  QGraphicsSimpleTextItem;  // подпись блока операций
    edgeText->setText("       Ребро");
    font.setPixelSize(16);
    edgeText->setFont(font);
    edgeText->setPos(80, 540);
    s->addItem(edgeText);    // добавление подписи склада

}

//для движения

void GraphWidget::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);

}

//добавить query

void GraphWidget::catchQuery(QSqlQuery q)
{
    myQuery = q;
}

//пересылка сигнала меню

void GraphWidget::menuClearScene()
{
    emit graphClearSceneSig();
}

//максимальный индекс из индексов базы

int GraphWidget::findMaxId(QList<int> container)
{
    int max = 0;

    for (int  i = 0; i<container.size(); ++i)
    {
        if(container.at(i)>max)
            max = container.at(i);
    }

    return max+1;
}

//функция для распределения событий между контекстными менюшками - возвращает узел в текущей позиции курсора

QGraphicsItem* GraphWidget:: isThereItem(QPointF point)
{
    return itemAt(mapFromScene (point));
}

//найти склад по индексу

Node* GraphWidget:: findStock(int index)
{
    Node* findedStock = nullptr;
    foreach(Node* node, stockList)
    {
        if(node->index_for_base==index) 
            return node;
    }
    return findedStock;
}


Node* GraphWidget:: findOperation(int index)
{
    Node* findedStock = nullptr;
    foreach(Node* node, operationList)
    {
        if(node->index_for_base==index)
            return node;
    }
    return findedStock;
}
//проверка на то, есть ли уже такой ид операции в списке

bool GraphWidget::isOperExists( int idx)
{
    int count = 0;
    foreach(Node* node, operationList)
    {
        if(node->index_for_base==idx)
            count++;

    }
    if(count!=0)
        return true;
    else
        return false;
}

//добавить склад

void GraphWidget:: menuAddStock()
{
    Node *node2 = new Node(this,"stock");   //  склад
    node2->setPos(0, 250);
    node2->createProxy();
    node2->index_for_base = findMaxId(stocks);
    stocks<<node2->index_for_base;
    scene()->addItem(node2);

    connect(node2, SIGNAL(deletNode(Node* )), this, SLOT(removeNode(Node* )));
    connect(node2, SIGNAL(deletEdgesFromNode(Node* )), this, SLOT(removeEdges(Node* )));
    connect(node2, SIGNAL(copyedNode(Node* )), this, SLOT(setCopyItem(Node* )));
    connect(node2, SIGNAL(paintNewEdge(Node*, Node*,Node*,Edge*)), this, SLOT(newEdgeOnScene(Node* , Node*, Node*,Edge*)));
    stockList.append(node2);

}

//сохранить  схему

void GraphWidget:: menuSave()
{
    if(edgeNodeList.isEmpty())
    {
        Node* badConnectionOP = nullptr;
        Node* badConnectionST = nullptr;
        Node* aloneOper       = nullptr;
        Node* aloneStock      = nullptr;

        foreach(Node* node, operationList)
        {
            if(node->sourseEdge!=nullptr)
            {   if(node->sourseEdge->destNode()->id=="operation")
                badConnectionOP = node;
            }
            if(node->sourseEdge==nullptr ||node->destEdgeList.isEmpty())
                aloneOper = node;
        }
        if(badConnectionOP==nullptr)
        {
            if(aloneOper==nullptr)
            {
                int not_unic_stock =  0;
                foreach(Node* node, stockList)
                {
                    if(node->sourseEdge!=nullptr &&node->sourseEdge->destNode()->id=="stock")
                        badConnectionST = node;
                    if(node->sourseEdge==nullptr &&node->destEdgeList.isEmpty())
                        aloneStock = node;
                    for( auto i = stockList.begin(); i!= stockList.end();++i)
                    {
                        if((*i)!=node&&(*i)->inscription->text()==node->inscription->text())
                           not_unic_stock++;
                    }
                }

                int not_unic_oper =  0;
                foreach(Node* node, operationList)
                {

                    for( auto i = operationList.begin(); i!= operationList.end();++i)
                    {
                        if((*i)!=node&&(*i)->inscription->text()==node->inscription->text())
                            not_unic_oper++;
                    }
                }

                if(badConnectionST==nullptr)
                {
                    if(aloneStock==nullptr)
                    {
                        if(not_unic_stock==0)
                        {
                            if(not_unic_oper==0)
                            {
                                int count = 1;
                                emit  sendCommand("CREATE TABLE stock_list (id_stock int, det_size int, stock_description varchar, personalize  varchar,  x float, y float, PRIMARY KEY(id_stock,det_size))");
                                emit  sendCommand("CREATE TABLE not_pers_stock_list (id_stock int,stock_description varchar, det_size varchar, amount int, PRIMARY KEY(id_stock,det_size))");
                                emit  sendCommand("CREATE TABLE pers_stock_list (id_stock int,stock_description varchar, n_ord varchar, amount int, PRIMARY KEY(id_stock))");
                                emit  sendCommand("CREATE TABLE id_stock_list (id int)");
                                emit  sendCommand("CREATE TABLE id_stock_list (id int)");
                                emit  sendCommand("CREATE TABLE id_oper_list (id int)");
                                emit  sendCommand("delete from stock_list");
                                emit  sendCommand("delete from id_stock_list");
                                emit  sendCommand("delete from id_oper_list");

                                QList<QString> sizes;
                                QList<QString> all_sizes;
                                QList<QString> order;
                                QList<QString> n_p_stock_descr;
                                QList<QString> p_stock_descr;
                                QList<QString> n_p_stock_amount;
                                QList<QString> p_stock_amount;

                                //////////////////////блок получения информции о персонализированных и неперсонализированныхскладах
                                emit callQuery();
                                myQuery.exec("select id_stock, amount ,det_size from not_pers_stock_list where det_size = (select size from size_list)");
                                while (myQuery.next())
                                {
                                    sizes<<myQuery.value(2).toString();
                                    n_p_stock_descr<<myQuery.value(0).toString();
                                    n_p_stock_amount<<myQuery.value(1).toString();
                                }

                                myQuery.exec("select id_stock, amount , n_ord from pers_stock_list ");
                                while (myQuery.next())
                                {
                                    order<<myQuery.value(2).toString();
                                    p_stock_descr<<myQuery.value(0).toString();
                                    p_stock_amount<<myQuery.value(1).toString();
                                }

                                myQuery.exec("select size from size_list");
                                while (myQuery.next())
                                {
                                    all_sizes<<myQuery.value(0).toString();
                                }
                                myQuery.exec("delete from not_pers_stock_list");
                                myQuery.exec("delete from pers_stock_list");
                                //////////////////////конец блока
                                qDebug()<<"fiiiiiiiiiiiiiiire";
                                foreach(Node* node, stockList)
                                {
                                    QString command = "INSERT INTO stock_list (id_stock, det_size, stock_description, personalize, x,y) VALUES (" + QString::number(node->index_for_base)
                                            + ",1,'"+node->inscription->text()
                                            + "', '" +node->personalize+"', " +QString::number(node->x())
                                            +","+QString::number(node->y())+")";
                                    qDebug()<<node->personalize;
                                    QString command1 ="INSERT INTO id_stock_list (id) VALUES (" + QString::number(node->index_for_base)+")";

                                    myQuery.exec("update userPersSt set stock_description = '"+node->inscription->text()+"' where id_stock = "+ QString::number(node->index_for_base));
                                    //////////////////////блок добавления полученных данных - это нужно для того, чтобы в каждом обновлении схемы не терять ифу о материалах. но если переименовать склад, все исчезнет
                                    if(node->personalize=="нет")
                                    {
                                        for(auto i = all_sizes.begin(); i!=all_sizes.end(); ++i)
                                        {
                                            QString command2 = "INSERT INTO not_pers_stock_list (id_stock,stock_description, det_size, amount) values ("+
                                                    QString::number(node->index_for_base)+",'"+node->inscription->text()+"','"+(*i)+"',0)";
                                            myQuery.exec(command2);
                                        }

                                        for(int i = 0; i< n_p_stock_descr.size(); i++)
                                        {
                                            QString command3 = "update not_pers_stock_list set amount = "+n_p_stock_amount.at(i)
                                                    +"where id_stock ="+ QString::number(node->index_for_base)+
                                                    " and det_size = '"+ sizes.at(i)+"'";
                                            myQuery.exec(command3);
                                        }
                                    }
                                    else
                                    {
                                        for(int i = 0; i< p_stock_descr.size(); i++)
                                        {
                                            if(p_stock_descr.at(i).toInt()==node->index_for_base)
                                            {
                                                QString command4 = "INSERT INTO pers_stock_list(id_stock,stock_description, n_ord, amount) values ("+
                                                        QString::number(node->index_for_base)+",'"+node->inscription->text()+"','"+ order.at(i)+"'," +p_stock_amount.at(i)+")";
                                                myQuery.exec(command4);
                                            }
                                        }
                                    }
                                    //////////////////////конец блока

                                    myQuery.exec(command);
                                    myQuery.exec(command1);
                                    QString command2 = "update user_and_order_list set  stock_desc = '"+node->inscription->text()+"' where n_stock = "+QString::number(node->index_for_base);
                                    myQuery.exec(command2);
                                    qDebug()<<myQuery.lastError();
                                    node->itemIndex = count;
                                    count++;
                                }
                                count = 1;
                                myQuery.exec("CREATE TABLE operation_list (id_operation int, operation_description varchar, id_source_stock int, id_dest_stock  int, x float, y float, PRIMARY KEY(id_operation, id_source_stock ))");
                                myQuery.exec("delete from operation_list ");
                                foreach(Node* node, operationList)
                                {
                                    int persFlag = 0;
                                    for(auto i = node->destEdgeList.begin(); i!=node->destEdgeList.end();++i)
                                    {
                                        QString command = "INSERT INTO operation_list (id_operation, operation_description, id_source_stock, id_dest_stock ,x,y) VALUES (" + QString::number(node->index_for_base)
                                                + ",'"+node->inscription->text() + "',"
                                                +QString::number((*i)->sourceNode()->index_for_base)+","
                                                +QString::number(node->sourseEdge->destNode()->index_for_base)+","
                                                +QString::number(node->x())
                                                +","+QString::number(node->y())+")";
                                        myQuery.exec(command);
                                        if((*i)->sourceNode()->personalize=="да")
                                            persFlag++;
                                    }

                                    if(persFlag!=0)
                                    {
                                        QList<QString> id_user;
                                        myQuery.exec("update stock_list set personalize = 'да' where id_stock = "
                                                     + QString::number(node->sourseEdge->destNode()->index_for_base));
                                        myQuery.exec("select id from user_and_order_list where n_stock = "
                                                     +  QString::number(node->sourseEdge->destNode()->index_for_base) +" group by id");
                                        while(myQuery.next())
                                            id_user<< myQuery.value(0).toString();
                                        myQuery.exec("delete from user_and_order_list where n_stock = "
                                                     +  QString::number(node->sourseEdge->destNode()->index_for_base));
                                        myQuery.exec("delete from not_pers_stock_list where id_stock = "
                                                     +  QString::number(node->sourseEdge->destNode()->index_for_base));
                                        for(int i = 0; i<id_user.size();++i)
                                            myQuery.exec("insert into  userPersSt (id_user, id_stock,stock_description) values ("+id_user.at(i)+","
                                                         + QString::number(node->sourseEdge->destNode()->index_for_base)
                                                         +",'"+ node->sourseEdge->destNode()->inscription->text()+"')");
                                    }

                                    QString command1 ="INSERT INTO id_oper_list (id) VALUES (" + QString::number(count)+")";
                                    emit  sendCommand(command1);
                                    QString command2 = "update user_and_oper_list oper_desc = '"
                                            +node->inscription->text()+"' where oper_id = "+QString::number(node->index_for_base) ;
                                    myQuery.exec(command2);
                                    command2 = "update user_and_order_list  oper_desc = '"
                                            +node->inscription->text()+"' where n_oper = "+QString::number(node->index_for_base);
                                    myQuery.exec(command2);
                                    count++;


                                }
                                myQuery.exec("delete from user_and_oper_list where  NOT EXISTS (select operation_description from operation_list where oper_id =id_operation ) and oper_id!=(-1)");
                                myQuery.exec("delete from user_and_order_list where  NOT EXISTS (select operation_description from operation_list where n_oper =id_operation  ) and n_oper!=(-1)");
                                myQuery.exec("delete from user_and_order_list where  NOT EXISTS (select stock_description from stock_list where n_stock =id_stock ) and n_stock!=(-1)");
                                myQuery.exec("delete from userPersSt where  NOT EXISTS (select stock_description  from stock_list where userPersSt.id_stock =id_stock ) ");

                                QList<QString> ids;
                                myQuery.exec("select id from user_list");
                                while(myQuery.next())
                                    ids<< myQuery.value(0).toString();
                                for(auto i = ids.begin(); i!=ids.end();++i)
                                {
                                    QString command4 = "select n_stock from user_and_order_list where id ="+(*i)+ " and n_oper = (-1) group by n_stock";
                                    myQuery.exec(command4);
                                    if(myQuery.size()!=0)
                                        command4 = "update user_and_oper_list set stock_amount = "+ QString::number(myQuery.size())+" where id = "+ (*i);
//                                    else
//                                        command4 = "delete from user_and_oper_list  where id = "+ (*i)+" and oper_id = (-1)";
                                    myQuery.exec(command4);
                                    myQuery.exec("update user_and_oper_list set oper_desc = (select operation_description from operation_list where oper_id =id_operation) where  id ="+(*i)+ " and oper_id != (-1)");
                                     myQuery.exec("update user_and_order_list set stock_desc = (select stock_description from stock_list where n_stock =id_stock) where  id ="+(*i)+ " and n_oper = (-1)");
                                }

                                foreach(Node* node, stockList)
                                {
                                    if(node->personalize=="да")
                                    {
                                        QString id =  QString::number(node->index_for_base);
                                        QList<QString> id_user;
                                        QString id_nextSt;
                                        QString id_nextDescr;
                                        myQuery.exec("update stock_list set personalize= 'да' where id_stock = "+ id);
                                        myQuery.exec("select id from user_and_order_list where n_stock = "+ id +" group by id");
                                        while(myQuery.next())
                                            id_user<< myQuery.value(0).toString();
                                        myQuery.exec("delete from user_and_order_list where n_stock = "+ id);
                                        myQuery.exec("delete from not_pers_stock_list where id_stock = "+ id);
                                        for(int i = 0; i<id_user.size();++i)
                                            myQuery.exec("insert into  userPersSt (id_user, id_stock,stock_description) values ("+id_user.at(i)
                                                         +","+id+",'"+node->inscription->text()+"')");

                                        myQuery.exec("select id_stock, stock_description from (select id_dest_stock as dest from operation_list where id_source_stock = "
                                                     + id+" group by id_dest_stock) as foo , stock_list where stock_list.id_stock = foo.dest");
                                        qDebug()<<myQuery.lastError();
                                        myQuery.next();
                                        if(myQuery.size()!=0)
                                            setPersByIdx(myQuery.value(0).toString()
                                                         , myQuery.value(1).toString());
                                    }

                                }

                            emit updateBrowsedSheme();
                            }
                            else
                            {
                                QMessageBox message;
                                message.setText( "Ошибка построения схемы. несколько операций имеют одинаковое название");
                                message.exec();
                            }

                        }
                        else
                        {
                            QMessageBox message;
                            message.setText( "Ошибка построения схемы. несколько складов имеют одинаковое название");
                            message.exec();
                        }
                    }
                    else
                    {
                        QMessageBox message;
                        message.setText( "Ошибка построения схемы. Одиночный склад");
                        message.exec();
                    }
                }
                else
                {
                    QMessageBox message;
                    message.setText( "Ошибка построения схемы. Неправильное соединение складов");
                    message.exec();
                }
            }
            else
            {
                QMessageBox message;
                message.setText( "Ошибка построения схемы. Одиночный блок операций");
                message.exec();
            }
        }
        else
        {
            QMessageBox message;
            message.setText( "Ошибка построения схемы. Неправильное соединение блоков операций");
            message.exec();
        }
    }
    else
    {
        QMessageBox message;
        message.setText( "Ошибка построения схемы. Пустое ребро");
        message.exec();
    }

}

//персональные склады

void GraphWidget::setPersByIdx(QString idx, QString name)
{
    QList<QString> id_user;
    QString id_nextSt;
    QString id_nextDescr;
    myQuery.exec("update stock_list set personalize= 'да' where id_stock = "+ idx);
    myQuery.exec("select id from user_and_order_list where n_stock = "+ idx +" group by id");
    while(myQuery.next())
       id_user<< myQuery.value(0).toString();
    myQuery.exec("delete from user_and_order_list where n_stock = "+ idx);
    myQuery.exec("delete from not_pers_stock_list where id_stock = "+ idx);
    for(int i = 0; i<id_user.size();++i)
        myQuery.exec("insert into  userPersSt (id_user, id_stock,stock_description) values ("+id_user.at(i)+","+idx+",'"+name+"')");

    myQuery.exec("select id_stock, stock_description from (select id_dest_stock as dest from operation_list where id_source_stock = "
                          + idx+" group by id_dest_stock) as foo , stock_list where stock_list.id_stock = foo.dest");
    myQuery.next();
    if(myQuery.size()!=0)
        setPersByIdx(myQuery.value(0).toString()
                     , myQuery.value(1).toString());
}

//загрузить схему

void GraphWidget::browseScheme()
{
    emit callQuery();
    emit  sendCommand("CREATE TABLE stock_list (id_stock int, det_size int, stock_description varchar, personalize  varchar,  x float, y float, PRIMARY KEY(id_stock,det_size))");
    myQuery.exec("select* from stock_list");
    qDebug()<<"called q";

    while (myQuery.next())
    {

        qDebug()<<"called у";
        Node *node2 = new Node(this,"stock");
        node2->itemIndex = myQuery.value(0).toInt();
        node2->inscription->setText(myQuery.value(2).toString());
        node2->personalize = myQuery.value(3).toString();
        node2->setPos(myQuery.value(4).toDouble(), myQuery.value(5).toDouble());
        node2->createProxy();
        node2->index_for_base =  myQuery.value(0).toInt();
        stocks<<node2->index_for_base;

        scene()->addItem(node2);
        connect(node2, SIGNAL(deletNode(Node* )), this, SLOT(removeNode(Node* )));
        connect(node2, SIGNAL(deletEdgesFromNode(Node* )), this, SLOT(removeEdges(Node* )));
        connect(node2, SIGNAL(copyedNode(Node* )), this, SLOT(setCopyItem(Node* )));
        connect(node2, SIGNAL(paintNewEdge(Node*, Node*,Node*,Edge*)), this, SLOT(newEdgeOnScene(Node* , Node*, Node*,Edge*)));

        stockList.append(node2);

        if(editable==false)
        {
            node2->setFlag(QGraphicsItem::ItemIsMovable, false);
            node2->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
            node2->setFlag(QGraphicsItem::ItemIsFocusable, false);
            node2->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, false);
            node2->setFlag(QGraphicsItem::ItemIsSelectable, false);
            node2->inscription->setReadOnly(true);
        }

    }
    myQuery.exec("select* from operation_list");
    while ( myQuery.next())
    {
        Node *node2;
        if(!isOperExists(myQuery.value(0).toInt()))
        {
            node2 = new Node(this,"operation");
            node2->itemIndex = myQuery.value(0).toInt();

            node2->inscription->setText(myQuery.value(1).toString());
            node2->setPos(myQuery.value(4).toDouble(), myQuery.value(5).toDouble());
            node2->createProxy();
            scene()->addItem(node2);
            node2->index_for_base =  myQuery.value(0).toInt();
            operations<<node2->index_for_base;

            Edge* source      = new Edge(findStock(myQuery.value(2).toInt()), node2,0);
            Edge* destination = new Edge( node2,findStock(myQuery.value(3).toInt()),0);
            scene()->addItem(source);
            scene()->addItem(destination);
            connect(node2, SIGNAL(deletNode(Node* )), this, SLOT(removeNode(Node* )));
            connect(node2, SIGNAL(deletEdgesFromNode(Node* )), this, SLOT(removeEdges(Node* )));
            connect(node2, SIGNAL(copyedNode(Node* )), this, SLOT(setCopyItem(Node* )));
            connect(node2, SIGNAL(paintNewEdge(Node*, Node*,Node*,Edge*)), this, SLOT(newEdgeOnScene(Node* , Node*, Node*,Edge*)));

            operationList.append(node2);
            edgeList.append(source);
            edgeList.append(destination);

            if(editable==false)
            {
                node2->setFlag(QGraphicsItem::ItemIsMovable, false);
                node2->setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);
                node2->setFlag(QGraphicsItem::ItemIsFocusable, false);
                node2->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, false);
                node2->setFlag(QGraphicsItem::ItemIsSelectable, false);
                node2->inscription->setReadOnly(true);
            }
            else
            {
                connect(source, SIGNAL(removingEdge(Edge*)), this, SLOT(removeOneEdge(Edge*)));
                connect(destination, SIGNAL(removingEdge(Edge*)), this, SLOT(removeOneEdge(Edge*)));
            }
        }
        else
        {
            Edge* source      = new Edge(findStock(myQuery.value(2).toInt()),findOperation(myQuery.value(0).toInt()),0);
            scene()->addItem(source);
            edgeList.append(source);
            if(editable==true)
                connect(source, SIGNAL(removingEdge(Edge*)), this, SLOT(removeOneEdge(Edge*)));

        }
    }

    qDebug()<<"called p";
}



void  GraphWidget:: menuAddOperation()
{
    emit callQuery();

    Node *node1 = new Node(this,"operation");    // блок операций
    node1->setPos(-60, 0);
    node1->createProxy();
    node1->index_for_base = findMaxId(operations);
    operations<<node1->index_for_base;
    connect(node1, SIGNAL(deletNode(Node*)),          this, SLOT(removeNode(Node*)));
    connect(node1, SIGNAL(deletEdgesFromNode(Node*)), this, SLOT(removeEdges(Node*)));
    connect(node1, SIGNAL(copyedNode(Node* )), this, SLOT(setCopyItem(Node* )));
    connect(node1, SIGNAL(paintNewEdge(Node*, Node*,Node*,Edge*)), this, SLOT(newEdgeOnScene(Node* , Node*, Node*,Edge*)));

    operationList.append(node1);

    scene()->addItem(node1);
}



void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}



 void GraphWidget::dragEnterEvent ( QDragEnterEvent * event )
{
    QGraphicsView::dragEnterEvent(event);
}



void GraphWidget::dragLeaveEvent ( QDragLeaveEvent * event )
{
    QGraphicsView::dragLeaveEvent(event);
}



void GraphWidget::dragMoveEvent ( QDragMoveEvent * event )
{
    QGraphicsView::dragMoveEvent(event);
}



void GraphWidget::dropEvent ( QDropEvent * event )
{
    QByteArray byteArray;
    byteArray   = event->mimeData()->data("Item");
    Node * item = *reinterpret_cast<Node**>(byteArray.data());
    QPointF currPos = mapToScene(event->pos());
    if(indGraphWidg != 1)
    {
        if(item->id!="edge")
        {

            item->setPos(currPos);
            item->gWidget = this;
            connect(item, SIGNAL(copyedNode(Node* )), item->gWidget, SLOT(setCopyItem(Node* )));
            connect(item, SIGNAL(deletNode(Node* )), this, SLOT(removeNode(Node* )));
            connect(item, SIGNAL(paintNewEdge(Node*, Node*,Node*,Edge*)), this, SLOT(newEdgeOnScene(Node* , Node*, Node*,Edge*)));
            connect(item, SIGNAL(deletEdgesFromNode(Node* )), this, SLOT(removeEdges(Node* )));
            item->inscription->setEnabled(true);
            item->sceneIndex = indGraphWidg;
            scene()-> addItem(item);
            if(item->id=="operation")
            {
                item->itemIndex = operationList.size() + 1;
                operationList.append(item);
                item->index_for_base = findMaxId(operations);
                operations<<item->index_for_base;
            }
            else
            {
                item->itemIndex = stockList.size() + 1;
                stockList.append(item);
                item->index_for_base = findMaxId(stocks);
                stocks<<item->index_for_base;
            }
        }
        else
        {
            QList<QString> list;
            Node* edgeN1 = new Node(this, "edgeNode");
            edgeN1->setPos(currPos);
            Node* edgeN2 = new Node(this, "edgeNode");
            edgeN2->setPos(currPos.x()-30, currPos.y()+20);
            Edge* connection = new Edge(edgeN1, edgeN2,0);

            edgeList.append(connection);
            edgeNodeList.append(edgeN1);
            edgeNodeList.append(edgeN2);

            scene()->addItem(edgeN1);
            scene()->addItem(edgeN2);
            scene()->addItem(connection);

            connect(edgeN1, SIGNAL(paintNewEdge(Node*, Node*,Node*,Edge*)), this, SLOT(newEdgeOnScene(Node* , Node*, Node*,Edge*)));
            connect(edgeN1, SIGNAL(deletNode(Node* )), this, SLOT(removeNode(Node* )));
            connect(edgeN2, SIGNAL(deletNode(Node* )), this, SLOT(removeNode(Node* )));
            connect(edgeN1, SIGNAL(deletEdgesFromNode(Node* )), this, SLOT(removeEdges(Node* )));
            connect(edgeN2, SIGNAL(deletEdgesFromNode(Node* )), this, SLOT(removeEdges(Node* )));
            connect(edgeN2, SIGNAL(paintNewEdge(Node*, Node*, Node*,Edge*)), this, SLOT(newEdgeOnScene(Node* , Node*, Node*,Edge*)));
            connect(connection, SIGNAL(removingEdge(Edge*)), this, SLOT(removeOneEdge(Edge*)));
        }
    }
    QGraphicsView::dropEvent(event);
}

//удалить узел

void GraphWidget::removeNode(Node* delNode)
{
    qDebug()<<delNode->gWidget->operationList.size();
    if(delNode->id=="operation")
    {
        delNode->gWidget->operationList.removeAt(delNode->gWidget->operationList.indexOf(delNode));
        delNode->gWidget->operations.removeAt(delNode->gWidget->operations.indexOf(delNode->index_for_base));
        qDebug()<<delNode->gWidget->operationList.size();
    }
    else
    {
        if(delNode->id=="stock")
        {
            delNode->gWidget->stockList.removeAt(delNode->gWidget->stockList.indexOf(delNode));
            delNode->gWidget->stocks.removeAt(delNode->gWidget->stocks.indexOf(delNode->index_for_base));
        }
        else
            delNode->gWidget->edgeNodeList.removeAt(delNode->gWidget->edgeNodeList.indexOf(delNode));
    }
    delete delNode;
}

// удаление ребер, связанных с передаваемым узлом

void  GraphWidget::removeEdges(Node* delNode)
{
    foreach (Edge * edge, edgeList)
    {
        if(edge->sourceNode()==delNode||edge->destNode()==delNode)
             edgeList.removeAt(edgeList.indexOf(edge));

    }
}

// удаление какого-то одного ребра

void  GraphWidget:: removeOneEdge(Edge* delEdge)
{

   Node* first  = delEdge->sourceNode();
   Node* second = delEdge->destNode();
   edgeList.removeAt(edgeList.indexOf(delEdge));
   delete delEdge;
   //если узел-источник - это уже какой-то итем
   if(first->id!="edgeNode")
       first->sourseEdge  = nullptr;
   else
   {
       edgeNodeList.removeAt(edgeNodeList.indexOf(first));
       delete first;

   }
    //если узел-приемник - это уже какой-то итем
   if(second ->id!="edgeNode")
       second->destEdgeList.removeAt(second->destEdgeList.indexOf(delEdge));
   else
   {
       edgeNodeList.removeAt(edgeNodeList.indexOf(second));
       delete second;
   }
}

//записать узел в буфер обмена

void GraphWidget::setCopyItem(Node* copNode)
{
    if(copyedNode!=nullptr)
         copyedNode = nullptr;
    copyedNode = new Node(copNode->gWidget, copNode->id);
    copyedNode->createProxy();
    copyedNode->inscription->setText(copNode->inscription->text());
    connect(copyedNode, SIGNAL(deletNode(Node* )), this, SLOT(removeNode(Node* )));
    connect(copyedNode, SIGNAL(deletEdgesFromNode(Node* )), this, SLOT(removeEdges(Node* )));
    connect(copyedNode, SIGNAL(copyedNode(Node* )), this, SLOT(setCopyItem(Node* )));
    connect(copyedNode, SIGNAL(paintNewEdge(Node*, Node*,Node*,Edge*)), this, SLOT(newEdgeOnScene(Node* , Node*, Node*,Edge*)));

    emit  makeEnabled();

}

//создание звязки между двумя узлами - по сути добавление нового ребра

void GraphWidget:: newEdgeOnScene(Node* source, Node*dest, Node* delNode, Edge* delEdge)
{
    if(source!=dest)
    {
        QPointF coordS = source->pos();
        QPointF coordD = dest->pos();
        // чистка сцены
        scene()->removeItem(delNode );
        scene()->removeItem(delEdge);
        // чистка памяти в удаляемом узле. ну, на всякий. хуже не стало
        if(delNode->sourseEdge!=nullptr)
            delete delNode->sourseEdge;
        else
        {
            delNode->destEdgeList.clear();
            delNode->sourseEdge=nullptr;
        }
        scene()->items().removeAt(scene()->items().indexOf(delNode));
        // чистка запомненных узлов в виджете
        edgeList.removeAt(edgeList.indexOf(delEdge));
        edgeNodeList.removeAt(edgeNodeList.indexOf(delNode));
        delete delNode;
        // чистка ребер источника
        if(source->destEdgeList.indexOf(delEdge)!=-1)
            source->destEdgeList.removeAt(source->destEdgeList.indexOf(delEdge));
        else
        {
            if(delEdge==source->sourseEdge)
            {
                delete source->sourseEdge;
                source->sourseEdge = nullptr;
            }
        }
       // чистка ребер приемника. не чистить все нельзя - падает
        if(dest->destEdgeList.indexOf(delEdge)!=-1)
            dest->destEdgeList.removeAt(dest->destEdgeList.indexOf(delEdge));
        else
        {
            if(delEdge==dest->sourseEdge)
            {
                delete dest->sourseEdge;
                dest->sourseEdge= nullptr;
            }
        }
        // добавление нового узла
        Edge * h = new Edge(source,dest ,1);
        edgeList.append(h);
        scene()->addItem(h);
        dest->setPos(coordD);
        source->setPos(coordS);
           update();
        connect(h, SIGNAL(removingEdge(Edge*)), this, SLOT(removeOneEdge(Edge*)));
    }
}



void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    foreach (Node *node, nodes)
        node->calculateForces();

    bool itemsMoved = false;
    foreach (Node *node, nodes) {
        if (node->advancePosition())
            itemsMoved = true;
    }
    scene()->update();
    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}


#if QT_CONFIG(wheelevent)
//! [5]
void GraphWidget::wheelEvent(QWheelEvent *event)
{
    //if(indGraphWidg != 1)
    scaleView(pow((double)2, -event->delta() / 240.0));
}
//! [5]
#endif


void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect){}



void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}



void GraphWidget::shuffle()
{
    foreach (QGraphicsItem *item, scene()->items()) {
        if (qgraphicsitem_cast<Node *>(item))
            item->setPos(-150 + QRandomGenerator::global()->bounded(300), -150 + QRandomGenerator::global()->bounded(300));
    }
}



void GraphWidget::zoomIn()
{
    //if(indGraphWidg != 1)
    scaleView(qreal(1.2));
}



void GraphWidget::zoomOut()
{
    //if(indGraphWidg != 1)
    scaleView(1 / qreal(1.2));
}
