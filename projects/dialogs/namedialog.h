#ifndef NAMEDIALOG_H
#define NAMEDIALOG_H
#include <QDialog>
#include <QList>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QFile>

class nameDialog: public QDialog// дайлог для изменения текущего именни продукции
{
    Q_OBJECT
public:
    QLineEdit* line;
    nameDialog(QString lastName)
    {
        resize(400,150);
        setWindowTitle("Изменение названия продукции");
        QVBoxLayout * layout = new QVBoxLayout;
        QLabel       * label = new QLabel("ВВЕДИТЕ НОВОЕ ИМЯ ПРОДУКЦИИ ");
        label->setFixedHeight(20);
        line = new  QLineEdit;
        line->setText(lastName);
        QPushButton *button = new QPushButton("Сохранить изменения");
        layout->addWidget(label);
        layout->addWidget(line);
        layout->addWidget(button);
        setLayout(layout);
        connect(button, SIGNAL(clicked()), this, SLOT(saveNameChanges()));
    }
public slots:
    void saveNameChanges()
    {

        emit setNewName(line->text());
        done(1);
    }
signals:
    void setNewName(QString name);
};



#endif // NAMEDIALOG_H
