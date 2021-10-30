#include "graphicEdit/scene.h"
#include <QDebug>
#include <QGraphicsSceneDragDropEvent >
#include "graphicEdit/graphwidget.h"

Scene::Scene(int x,int y, int w, int h, int idx, GraphWidget* rect)
{
    setItemIndexMethod(QGraphicsScene::NoIndex);
    setSceneRect(x, y, w, h);
    currIdx = idx;
    widg = rect;
    connect(widg, SIGNAL( makeEnabled()),this, SLOT(enabledMenu()));
}

void Scene::dragEnterEvent ( QGraphicsSceneDragDropEvent * event ){}


void Scene:: paste()
{
   widg->copyedNode->setPos(pasteCoords );
   if(widg->copyedNode->id=="operation")
       widg->operationList.append(widg->copyedNode);
   else
       widg->stockList.append(widg->copyedNode);
   addItem(widg->copyedNode) ;
}


void Scene:: contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if(widg->editable==true)
    {
        if ( widg->isThereItem(event->scenePos ()))
        {
            qDebug()<<"nnn";
            Node * item= dynamic_cast<Node *>(widg->isThereItem(event->scenePos ()));
            if(item)
                item->contextMenuEvent(event);
            else
            {
                Edge * item1= dynamic_cast<Edge  *>(widg->isThereItem(event->scenePos ()));
                if(item1)
                    item1->contextMenuEvent(event);
            }
        }
        else
        {
            QMenu menu;
            QAction *clearAction    = menu.addAction("Очистить сцену",this, SLOT(clearScene()));
            pasteAction             = menu.addAction("Вставить элемент",this, SLOT(paste()));
            pasteAction->setEnabled(pasteFlag);
            pasteCoords = event->screenPos();
            QAction *selectedAction = menu.exec(event->screenPos());
        }
    }
}


void Scene:: clearScene()
{
    qDebug()<<"wywywy";
    if(items().size()!=0)
    {
        widg->operationList.clear();
        widg->stockList.clear();
        widg->edgeNodeList.clear();
        widg->edgeList.clear();
        qDebug()<<"wywywy";

            widg->operations.clear();
            widg->stocks.clear();

        clear ();
    }
}

void Scene:: mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsScene::mousePressEvent(event);
}


void Scene::dragLeaveEvent ( QGraphicsSceneDragDropEvent * event )
{
    QByteArray byteArray = event->mimeData()->data("Item");
    Node * item = *reinterpret_cast<Node**>(byteArray.data());
    item->sceneIndex = currIdx;
    Node *  node1 = new Node(item->gWidget,item->id);
    node1->sceneIndex = currIdx;
    node1->inscription->setEnabled(false);
    if(node1->id!="edge")
    {
        QGraphicsProxyWidget* pMyProxy = new QGraphicsProxyWidget(node1); // the proxy parent is pMyItem
        pMyProxy->setWidget(node1->inscription);
        if(node1->id=="stock")
        {
            pMyProxy->setX(25);
            pMyProxy->setY(40);
        }
    }
    node1->setPos(item->pos());
    addItem(node1);

}


void Scene::dragMoveEvent ( QGraphicsSceneDragDropEvent * event ){}


void Scene::dropEvent ( QGraphicsSceneDragDropEvent * event ){}
