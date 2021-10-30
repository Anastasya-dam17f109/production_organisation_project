#include "adminTabWidget.h"
#include "graphicEdit/graphicedit.h"
#include "graphicEdit/paintwidget.h"

adminTabWidget::adminTabWidget()
{
    resize(2000,1000);
   // graphicEdit * w = new  graphicEdit;
    PaintWidget* stocks = new PaintWidget(0,0);
    PaintWidget* orders = new PaintWidget("");
    PaintWidget* scheme = new PaintWidget(0);
    PaintWidget* users  = new PaintWidget("",0);

    connect(stocks,SIGNAL(updateScheme()),scheme,SLOT(updateShemeAfterSaving()));
    connect(scheme,SIGNAL(updateStockList()),stocks,SLOT(usedOnSchemeStocks()));
    connect(scheme,SIGNAL(updateStockList()),stocks,SLOT(usedOnSchemeOperations()));
    connect(scheme,SIGNAL(updateStockList()),stocks,SLOT(setCurrStockList()));
    connect(users,SIGNAL(updateOrderList()),orders,SLOT(updateOrderTable()));

    setStyleSheet(" QTabBar::tab:selected {background: #E6E6FA;border: 1px solid #C4C4C3;border-bottom-color: #E6E6FA; };QTabBar::tab:!selected {background: #D8BFD8;border: 1px solid #C4C4C3;border-bottom-color: #D8BFD8; };");
    addTab(orders,"    ЗАКАЗЫ    ");
    addTab(scheme,"  СХЕМА ПРОИЗВОДСТВА  ");
    addTab(users,"  ПЕРСОНАЛ  ");
    addTab(stocks,"  СКЛАДЫ И ОПЕРАЦИИ  ");
    tabBar()->setFixedHeight(50);
    tabBar()->setShape(QTabBar::RoundedNorth);
    setWindowTitle("Производство - Администратор");

}
void adminTabWidget::paintEvent(QPaintEvent *event){;}

