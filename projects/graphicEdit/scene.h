#ifndef SCENE_H
#define SCENE_H

#include <QWidget>
#include <QGraphicsScene>
#include "edge.h"
#include "node.h"
#include <QTransform>
class GraphWidget;

class Scene: public QGraphicsScene
{
    Q_OBJECT
public:
    Scene(int, int, int, int, int,GraphWidget*);

    int          currIdx;
    GraphWidget* widg;
    QAction *    pasteAction;
    bool         pasteFlag  = false;
    QPointF      pasteCoords;

protected:
    virtual void dragEnterEvent ( QGraphicsSceneDragDropEvent * event );
    virtual void dragLeaveEvent ( QGraphicsSceneDragDropEvent * event );
    virtual void dragMoveEvent ( QGraphicsSceneDragDropEvent * event );
    virtual void dropEvent ( QGraphicsSceneDragDropEvent * event );
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void     contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

public slots:
    void enabledMenu()
    {
        qDebug()<<"ggg";
       pasteFlag = (true);
    }
    void paste();
    void clearScene();
};

#endif // SCENE_H
