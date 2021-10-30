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


#include "graphicEdit/graphicedit.h"
#include <QApplication>
#include <QTime>
#include <QMainWindow>
#include <QMessageBox>
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QTextStream>
#include <QtSql/QSqlTableModel>
#include "adminTabWidget.h"
#include "dialogs/opendialog.h"
#include "workerwidget.h"
#include "managerwidget.h"
#include "stockerwidget.h"
#include "deliverywidget.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setStyleSheet("adminTabWidget{background-color: #E6E6FA;};");

    openDatabaseDialog ch;
    int indicator =  ch.exec();
qDebug()<< indicator;
    if(indicator ==1 )
    {
        adminTabWidget *c = new adminTabWidget();
        c->show();
    }
    if(indicator==2)
    {
        QString id;
        QFile file("workerId.txt");
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        in>>id;
        file.close();
        workerWidget *c = new workerWidget(id);
        c->show();
    }
    if(indicator==3)
    {
        QString id;
        QFile file("workerId.txt");
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        in>>id;
        file.close();
        managerWidget *c = new managerWidget(id);
        c->show();
    }
    if(indicator==4)
    {
        QString id;
        QFile file("workerId.txt");
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        in>>id;
        file.close();
        stockerWidget *c = new stockerWidget(id);
        c->show();
    }
    if(indicator==5)
    {
        QString id;
        QFile file("workerId.txt");
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        in>>id;
        file.close();
        deliveryWidget *c = new deliveryWidget(id);
        c->show();
    }
    return app.exec();
}
