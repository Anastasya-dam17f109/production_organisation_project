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

#include "graphicEdit/edge.h"
#include "graphicEdit/node.h"
#include "graphicEdit/graphwidget.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QDebug>



Node::Node(GraphWidget *graphWidget, QString Id)
    :  graph(graphWidget)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    setCacheMode(DeviceCoordinateCache);
    setFlag(ItemIsSelectable);
    setZValue(-1);
    sceneIndex  = graphWidget->indGraphWidg;
    personalize = "нет";
    id          = Id;
    gWidget     = new  GraphWidget(0);
    gWidget     = graphWidget;

    if(Id!="edgeNode")
    {
        destUSB     = new edgeNode(graphWidget, "edgeNode");
        sourseUSB   = new edgeNode(graphWidget, "edgeNode");
        destUSB->setParentItem(this);
        destUSB->setFlag(ItemIsMovable,false);
        sourseUSB ->setParentItem(this);
        sourseUSB ->setFlag(ItemIsMovable,false);
    }


    if(Id=="operation")
    {
        sourseUSB->setPos(calcWidth+40-15,30);
        destUSB->setPos(-10,30);
        //list_of_variants = names;
    }
    else
    {  if(Id=="stock")
        {
            destUSB->setPos(20,50);
            sourseUSB->setPos(stockBegWidth-25,50);
            //list_of_variants = names;
        }
        else
        {
            if(Id=="edge")
            {
                destUSB->setPos(10,20);
                sourseUSB->setPos(80,20);
                Edge* edgeOnPicture = new Edge(destUSB,sourseUSB);


                edgeOnPicture ->setParentItem(this);

                edgeOnPicture->setFlag(ItemIsMovable,false);
                edgeOnPicture->setFlag(QGraphicsItem::ItemIsFocusable,false);
                edgeOnPicture->setFlag(ItemIsSelectable,false);
            }

        }
    }
    if(Id!="edge"||Id!="edgeNode")
    {
        inscription = new QLineEdit;
        //inscription->setSizeAdjustPolicy (QComboBox::AdjustToMinimumContentsLengthWithIcon);
       // inscription->addItems(names);
        inscription->resize(50, 30);
        inscription->setStyleSheet("QLineEdit {background: yellow};");
        QFont font = inscription->font();
        font.setPixelSize(18);
        inscription->setFont(font);
        inscription->setFrame(false);
        QObject::connect(inscription,SIGNAL(textChanged(const QString )),this,SLOT(resizeRect()) );
    }

}


void Node::addEdge(Edge *edge)
{
    if(edge->destNode()==this)
        destEdgeList << edge;
    else
        sourseEdge  = edge;
    edge->adjust();
}


void Node::calculateForces()
{
    if (!scene() || scene()->mouseGrabberItem() == this) {
        newPos = pos();
        return;
    }
    // Sum up all forces pushing this item away
    qreal xvel = 0;
    qreal yvel = 0;
    foreach (QGraphicsItem *item, scene()->items()) {
        Node *node = qgraphicsitem_cast<Node *>(item);
        if (!node)
            continue;

        QPointF vec = mapToItem(node, 0, 0);
        qreal dx = vec.x();
        qreal dy = vec.y();
        double l = 2.0 * (dx * dx + dy * dy);
        if (l > 0) {
        }
    }
    if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
        xvel = yvel = 0;
//! [5]

//! [6]
    QRectF sceneRect = scene()->sceneRect();
    newPos = pos() + QPointF(xvel, yvel);
    newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
    newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
    scene()->update();
}



void Node::copy()
{
    emit copyedNode(this);
}


void Node:: mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    setPos( mapToScene(event->pos()));
}

bool Node::advancePosition()
{
    //setPos(newPos);
    scene()->update();
    return true;
}



QRectF Node::boundingRect() const //определение ограничивающего прямоугольника
{
    qreal adjust = 2;
    if(id=="operation")
        return QRectF( -10 - adjust, -10 - adjust, calcWidth + adjust + 40, 63 + adjust);
    else
    {
        if(id=="stock")
            return QRectF( -10 - adjust, -10 - adjust, stockBegWidth +20, 100 );
        else
            if(id=="edge")
            return QRectF( -10 - adjust, -10 - adjust, 80, 100 );
        else return QRectF( -1, -1, 5, 5);
    }
}


QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}



void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * )
{    
    painter->setPen(Qt::NoPen);
   // painter->setBrush(Qt::darkGray);
    if(id=="operation")
       {
        painter->setBrush(QBrush("#FF1493"));
        painter->drawRect(-10, -10,calcWidth +  42 , 500);
    }
    else
    {
        if(id=="stock")
        {
            QPolygon poly;
            painter->setBrush(QBrush("#00FF00"));
            poly<<QPoint(0,30)<<QPoint(stockBegWidth/2,0)<<QPoint(stockBegWidth,30)<<QPoint(stockBegWidth-20,30)<<QPoint(stockBegWidth -20,100)<<QPoint(20,100)<<QPoint(20,30);
            painter->drawPolygon(poly);
        }
        if(id=="edgeNode")
        {
             painter->setBrush(Qt::black);
             painter->drawRect(0, 0,10 , 10);
        }
    }

}


QList<Edge *> Node::edges() const
{
    return destEdgeList;
}



void Node:: RemoveEdges()  // удалить  все ребра узла, и все свободные прикрепленвыне edgenode
{
    foreach (Edge * edge, destEdgeList)
    {
        Node* curr =  edge->sourceNode();
        delete curr->sourseEdge;
        curr->sourseEdge = nullptr;
        if(curr->id=="edgeNode")
        {
            emit deletNode(curr);
            //delete curr;
        }
    }
    emit deletEdgesFromNode(this);
    destEdgeList.clear();
    if(sourseEdge!=nullptr)
    {
        Node* curr = sourseEdge->destNode();
        curr->destEdgeList.removeAt(curr->destEdgeList.indexOf(sourseEdge));

        if(curr->id=="edgeNode")
        {
            emit deletNode(curr);
            //delete curr;
        }
        delete sourseEdge;
        sourseEdge = nullptr;
    }
}

void Node::deleteNode()  // удалить   узел, и все свободные связанные edgenode
{

    RemoveEdges();
    emit deletNode(this);
}

void  Node:: resizeRect()//слот, который делает переопределение ограничивающего прямоугольника узла
{
    prepareGeometryChange ();
    int height = 0;
    if(id=="operation")
        height = 30;
    else
        height = 50;
    if(inscription->text().length()*11+20>calcWidth )
        calcWidth = inscription->text().length()*11+10;
    inscription->resize(calcWidth, 25);
    if(stockBegWidth<calcWidth+50)
        stockBegWidth = calcWidth+50;

    if(lastTextLength>inscription->text().length())
    {
        if(inscription->text().length()*11+20>100)
       {
            calcWidth = inscription->text().length()*11+10;
            stockBegWidth = calcWidth+60;
        }
    }
    sourseUSB->setPos(calcWidth+40-15,height);
    if(id=="stock")
    {
        sourseUSB->setPos(stockBegWidth-25,height);
    }
    lastTextLength = inscription->text().length();
    if( !destEdgeList.isEmpty()){
        foreach (Edge *edge, destEdgeList)
            edge->adjust();
    }

    if(sourseEdge != nullptr)
        sourseEdge->adjust();
    //personalize = gWidget->personalization(inscription->currentIndex());

}


QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)//перегрузка стандарта. нужна для переопределения положения элемента
{
    QList<QGraphicsItem *> list = collidingItems() ;
    Node*  secondEdgeNode;
    Edge*  betwenEdge;
    switch (change) {
    case ItemPositionHasChanged:
        graph->itemMoved();
        if( !destEdgeList.isEmpty()){
            foreach (Edge *edge, destEdgeList)
                edge->adjust();
        }

        if(sourseEdge != nullptr)
            sourseEdge->adjust();
       //перерисовка ребер
        if(id=="edgeNode")
        {
            foreach(QGraphicsItem * i , list)
            {
                edgeNode * item= dynamic_cast<edgeNode *>(i);
                if (item)
                {
                    if(sourseEdge != nullptr)
                    {
                        secondEdgeNode  = sourseEdge->destNode();
                        betwenEdge      = sourseEdge;
                    }
                    else
                    {
                        betwenEdge      = destEdgeList.at(0);
                        secondEdgeNode  = destEdgeList.at(0)->sourceNode() ;
                    }
                    Node * itemParent = dynamic_cast<Node *>(item->parentItem());
                    if(itemParent !=secondEdgeNode)
                    {
                        if(itemParent->destUSB==item)
                            emit paintNewEdge(secondEdgeNode, itemParent, this,betwenEdge);
                        else
                        {
                            if(itemParent->sourseEdge==nullptr)
                            emit paintNewEdge(itemParent, secondEdgeNode, this,betwenEdge);
                        }
                    }
                }
            }
        }
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}



void Node::createProxy()   //добавление  итему прокси-виджета
{
    QGraphicsProxyWidget* pMyProxy2 = new QGraphicsProxyWidget(this); // the proxy parent is pMyItem
    pMyProxy2->setWidget(this->inscription);
    if(id=="stock")
    {
        pMyProxy2->setX(25);
        pMyProxy2->setY(40);
    }

}


void Node:: contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if(id!="edgeNode")
    {
        QMenu menu;
        QAction *removeAction   = menu.addAction("Удалить все ребра", this, SLOT(RemoveEdges()));
        QAction *delAction      = menu.addAction("Удалить узел",this, SLOT(deleteNode()));
        QAction *copyAction     = menu.addAction("Копировать",this, SLOT(copy()));
        QAction *selectedAction = menu.exec(event->screenPos());
    }

}


void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    if(sceneIndex==1)
    {
        QMimeData * mimeData = new QMimeData;
        Node      * item     = this;
        QDrag     * drag     = new QDrag(event->widget());
        QByteArray byteArray(reinterpret_cast<char*>(&item),sizeof(Node*));
        mimeData->setData("Item",byteArray); 
        drag->setMimeData(mimeData);
        drag->start();
    }
    else
        QGraphicsItem::mousePressEvent(event);
}


void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
//! [12]
