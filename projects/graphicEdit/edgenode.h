#ifndef EDGENODE_H
#define EDGENODE_H
#include <QGraphicsObject>
#include <QList>
#include <QGraphicsProxyWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QDrag>
#include <QDebug>
#include <QMimeData>
#include <QGraphicsSceneMouseEvent>

class Edge;
class Node;
class GraphWidget;
QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;

class edgeNode : public QGraphicsObject
{
    Q_OBJECT
public:
    int         sceneIndex;
    QString     id;

    edgeNode(GraphWidget *graphWidget, QString);

    void addEdge(Edge *edge);

    QList<Edge *> edges() const;

    enum { Type = UserType + 3 };
    int type() const override { return Type; }

    void calculateForces();
    bool advancePosition();

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:

    QPointF newPos;
    GraphWidget *graph;

};

#endif // EDGENODE_H
