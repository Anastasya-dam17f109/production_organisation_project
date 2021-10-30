#ifndef WORKERWIDGET_H
#define WORKERWIDGET_H
#include <QWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMenu>
#include <QDebug>
#include <QComboBox>
#include <QTextStream>
#include <QTableView>
#include <QTreeView>
#include <QFile>
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QContextMenuEvent>
#include "dialogs/executeoperation.h"

using namespace std;

class workerWidget:public QWidget
{
 Q_OBJECT
public:
    QString id;
    QSqlDatabase dbg;
    QList<QString> ordTableHead;
    QTableView  * ordTableView;
    QTableView  * operTableView;
    QTreeView* opers;
    int currentPage = 0;
    QLabel* currentHeader;
    QMenu*  contextMenu;
    QAction *deleteAction;
    QAction *updateAction;

    QModelIndex HighlightIndex;

    virtual void contextMenuEvent(QContextMenuEvent* pe)
    {
        contextMenu->exec(pe->globalPos());
    }

    workerWidget(QString);
    void setBase();
    void updateView();
    QString createBigCommand();

     QStandardItemModel* setTreeModel();

public slots:

     void sendQuery();
     void nextButton();
     void backButton();
     void menuExecuteOperationtion();
     void onCustomContextMenu(const QPoint &);
     void executeOperation(QString ordNumber,QString operNumber );
     void regectDetail(QString ordNumber,QString operNumber);
     void setOpersNotdone(QString destStockIndex, QString delOrd );
     void menuRegectOperation();

     void callExecuteDialog();
     void callRegectDialog();
signals:
      void newQuery(QSqlQuery q);
};

#endif // WORKERWIDGET_H
