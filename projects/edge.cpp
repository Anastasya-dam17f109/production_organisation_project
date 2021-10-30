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
#include "graphicEdit/edgenode.h"
#include "graphicEdit/node.h"

#include <qmath.h>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOption>
#include <QDebug>

//! [0]
Edge::Edge(Node *sourceNode, Node *destNode, int idx)
    : arrowSize(10)
{
    setFlag(ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setCacheMode(DeviceCoordinateCache);
    setFlag(ItemIsSelectable);
    source = sourceNode;
    dest = destNode;
    sceneIndex = idx;
    source->addEdge(this);
    dest->addEdge(this);
    adjust();
}


Edge::Edge(edgeNode *sourceNode, edgeNode *destNode)
    : arrowSize(10)
{
    setFlag(ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setCacheMode(DeviceCoordinateCache);
    setFlag(ItemIsSelectable);
    repSourse  = sourceNode;
    repDest    = destNode;
    repAdjust();
}


Node * Edge::sourceNode() const
{
    return source;
}

Node *Edge::destNode() const
{
    return dest;
}


void Edge::adjust()
{
    if (!source || !dest)
        return;
    QPointF xCoord;
    QPointF yCoord;
    if(source->id=="edgeNode"&&dest->id=="edgeNode")
    {
        xCoord = mapFromItem(source, 0, 0);
        yCoord  = mapFromItem(dest, 0, 0);
    }
    if(source->id=="edgeNode"&&dest->id!="edgeNode")
    {
        xCoord = mapFromItem(source, 0, 0);
        yCoord  = mapFromItem(dest->destUSB, 0, 0);
    }
    if(source->id!="edgeNode"&&dest->id=="edgeNode")
    {
        xCoord = mapFromItem(source->sourseUSB, 0, 0);
        yCoord  = mapFromItem(dest, 0, 0);
    }
    if(source->id!="edgeNode"&&dest->id!="edgeNode")
    {
        xCoord = mapFromItem(source->sourseUSB, 0, 0);
        yCoord  = mapFromItem(dest->destUSB, 0, 0);
    }
    QLineF line(xCoord, yCoord);

    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else {
        sourcePoint = destPoint = line.p1();
    }
}


void Edge::repAdjust()
{
    if (!repSourse || !repDest)
        return;
 qDebug()<<"ok";
    QLineF line(mapFromItem(repSourse, 0, 0), mapFromItem(repDest, 0, 0));
    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else {
        sourcePoint = destPoint = line.p1();
        qDebug()<<length ;
    }
}



QRectF Edge::boundingRect() const
{
//    if (!source || !dest)
//        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth + arrowSize) / 2.0;

    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
            .normalized()
            .adjusted(-extra, -extra, extra, extra);
}

void Edge::removeMe()
{
    emit removingEdge(this);
}


void Edge:: contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *removeAction   = menu.addAction("Удалить  ребро", this, SLOT(removeMe()));

    QAction *selectedAction = menu.exec(event->screenPos());

}


void Edge::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    begMovingPos  = this->pos();
    moveFinished  = false;
    QGraphicsItem::mousePressEvent(event);
}


void Edge::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}


QPainterPath Edge::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}


void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QLineF line(sourcePoint, destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);

}
//! [6]
