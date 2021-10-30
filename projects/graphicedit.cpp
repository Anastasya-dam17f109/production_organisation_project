
#include "graphicEdit/graphicedit.h"
#include "graphicEdit/paintwidget.h"
#include <QMenuBar>
graphicEdit::graphicEdit()
{
    resize(2000,1000);
    PaintWidget *widget   = new PaintWidget;
    QMenu *menu           = new QMenu("&Сцена");
    QAction *saveAction   = menu->addAction("Сохранить изменения", widget, SLOT(saveSceneFromMenu()));
    QAction *clearAction  = menu->addAction("Очистить сцену",widget, SLOT(clearSceneFromMenu()));
    QMenu    *menu1       = new QMenu("&Элементы сцены");
    QAction *opAction     = menu1->addAction("Добавить склад", widget, SLOT(addStock()));
    QAction *stAction     = menu1->addAction("Добавить операцию",widget, SLOT(addOperation()));
    setCentralWidget(widget);
    QMenuBar *menuBar     = new QMenuBar(0);
    menuBar->addMenu(menu);
    menuBar->addMenu(menu1);
    setMenuBar( menuBar);
    connect(widget, SIGNAL(updating()), this, SLOT(sendUS()));
setWindowModality ( Qt::ApplicationModal);
}

graphicEdit::graphicEdit(int idx)
{
    resize(2000,1000);
    PaintWidget *widget   = new PaintWidget(idx);
    QMenu *menu           = new QMenu("&Сцена");
    QAction *browseAction = menu->addAction("загрузить схему из базы", widget, SLOT(browseScene()));

    setCentralWidget(widget);
    QMenuBar *menuBar     = new QMenuBar(0);
    menuBar->addMenu(menu);
    setMenuBar( menuBar);

}
