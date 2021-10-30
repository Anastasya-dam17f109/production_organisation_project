#include "graphicEdit/edgenode.h"
#include "graphicEdit/edge.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include "graphicEdit/graphwidget.h"

edgeNode::edgeNode(GraphWidget *graphWidget, QString Id)
    : graph(graphWidget)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
    sceneIndex  = graphWidget->indGraphWidg;
    id          = Id;
}



void edgeNode::calculateForces()
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
   // double weight = (edgeList.size() + 1) * 10;
//    foreach (Edge *edge, edgeList) {
//        QPointF vec;
//        if (edge->sourceNode() == this)
//            vec = mapToItem(edge->destNode(), 0, 0);
//        else
//            vec = mapToItem(edge->sourceNode(), 0, 0);

//    }

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

bool edgeNode::advancePosition()
{
    if (newPos == pos())
        return false;

    setPos(newPos);
    return true;
}

QRectF edgeNode::boundingRect() const
{
    qreal adjust = 2;
    return QRectF( -1, -1, 5, 5);
}

QPainterPath edgeNode::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void edgeNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::black);
    painter->drawRect(0, 0,10 , 10);


}

QVariant edgeNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    QList<QGraphicsItem *> list = collidingItems() ;
    edgeNode*  secondEdgeNode;
    switch (change) {
    case ItemPositionHasChanged:

        break;
        case ItemScenePositionHasChanged:
        qDebug()<<"nnnnnnn";
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void edgeNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setActive(true);
    update();
    QGraphicsItem::mousePressEvent(event);
}

void edgeNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
