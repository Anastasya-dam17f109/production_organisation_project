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

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsSimpleTextItem>
#include "scene.h"
#include <QPainter>
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QTextStream>
#include <QtSql/QSqlTableModel>
#include <QMessageBox>
class Node;
class scene;

//! [0]
class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:

    int            indGraphWidg;
    QList<Node*>   operationList;
    QList<Node*>   stockList;
    QList<Node*>   edgeNodeList;
    QList<Edge*>   edgeList;
    Node*          copyedNode = nullptr;
    bool           editable   = true;
    QSqlQuery      myQuery;
    QList<int>     operations;
    QList<int>     stocks;
    QList<QString> personalised;

    GraphWidget(QWidget *parent );
    GraphWidget(QWidget *parent , int );

    void           browseScheme();
    Node*          findStock(int index);
    Node*          findOperation(int index);
    bool           isOperExists(int idx);
    void           itemMoved();
    QGraphicsItem* isThereItem(QPointF point);
    int            findMaxId(QList<int> container);
    void           setPersByIdx(QString idx, QString name);
public slots:

    void shuffle();
    void zoomIn();
    void zoomOut();

    void newEdgeOnScene(Node* source, Node*dest, Node* delNode, Edge* delEdge);
    void removeEdges(Node* delNode);  // удаление ребер, связанных с передаваемым узлом
    void removeOneEdge(Edge* delEdge);  // удаление одного ребра
    void removeNode(Node* delNode);
    void setCopyItem(Node* copNode);

    void menuClearScene();
    void menuSave();
    void menuAddStock();
    void menuAddOperation();
    void catchQuery(QSqlQuery q);


protected:

    void         keyPressEvent(QKeyEvent *event) override;
    void         timerEvent(QTimerEvent *event) override;
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dragLeaveEvent(QDragLeaveEvent * event);
    virtual void dragMoveEvent(QDragMoveEvent * event);
    virtual void dropEvent (QDropEvent * event);


#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void scaleView(qreal scaleFactor);

private:

    int   timerId;
    Node *centerNode;

signals:

    void makeEnabled();
    void graphClearSceneSig();
    void sendCommand(QString comm);
    void callQuery();
    void updateBrowsedSheme();
};
//! [0]

#endif // GRAPHWIDGET_H
