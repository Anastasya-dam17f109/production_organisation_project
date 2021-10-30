#ifndef SIZEDIALOG_H
#define SIZEDIALOG_H


#include <QDialog>
#include <QList>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QComboBox>

class sizeDialog: public QDialog// дайлог для изменения текущего именни продукции
{
    Q_OBJECT
public:
    QLineEdit* line;
    QComboBox *sizes;
    sizeDialog(QString prodName)//окно добавления нового размера
    {
        resize(400,200);
        setWindowTitle("Добавление нового размера");
        QLineEdit   * nameOf = new QLineEdit;
        QVBoxLayout * layout = new QVBoxLayout;
        QLabel      * label1 = new QLabel("НАЗВАНИЕ ПРОДУКЦИИ ");
        QLabel      * label2 = new QLabel("ВВЕДИТЕ НОВЫЙ РАЗМЕР ");
        label1->setFixedHeight(20);
        label2->setFixedHeight(20);
        nameOf->setText(prodName);
        nameOf->setReadOnly(true);
        line = new  QLineEdit;

        QPushButton *button = new QPushButton("Добавить размер");
        layout->addWidget(label1);
        layout->addWidget(nameOf);
        layout->addWidget(label2);
        layout->addWidget(line);
        layout->addWidget(button);
        setLayout(layout);
        connect(button, SIGNAL(clicked()), this, SLOT(saveSizeChanges()));
    }

    sizeDialog(QString prodName, QList<QString> list)//окно удаления размера
    {
        resize(400,200);
        setWindowTitle("удаление размера");
        QLineEdit   * nameOf = new QLineEdit;
        QVBoxLayout * layout = new QVBoxLayout;
        QLabel      * label1 = new QLabel("НАЗВАНИЕ ПРОДУКЦИИ ");
        QLabel      * label2 = new QLabel("ВЫБЕРИТЕ РАЗМЕР ДЛЯ УДАЛЕНИЯ ");
        label1->setFixedHeight(20);
        label2->setFixedHeight(20);
        nameOf->setText(prodName);
        nameOf->setReadOnly(true);
        sizes    = new  QComboBox ;
        sizes->addItems(list);

        QPushButton *button = new QPushButton("Удалить размер");
        layout->addWidget(label1);
        layout->addWidget(nameOf);
        layout->addWidget(label2);
        layout->addWidget(sizes);
        layout->addWidget(button);
        setLayout(layout);
        connect(button, SIGNAL(clicked()), this, SLOT(deleteChosenSize()));
    }

    sizeDialog()//окно добавления новой доллжности
    {
        resize(400,200);
        setWindowTitle("Добавление новой должности");
        QVBoxLayout * layout = new QVBoxLayout;
        QPushButton * button = new QPushButton("Добавить должность");
        QLabel      * label2 = new QLabel("ВВЕДИТЕ НАЗВАНИЕ ДОЛЖНОСТИ ");
        QLabel      * label1 = new QLabel("ВЫБЕРИТЕ СФЕРУ ДЕЯТЕЛЬНОСТИ ");
        QList <QString> list;
        list<<""<<"Выпуск"<<"Доставка"<<"Услуги"<<"Закупка";
              //"Тех.Обслуживание"<<"Уборка";
        sizes    = new  QComboBox ;
        sizes->addItems(list);
        label2->setFixedHeight(20);
        label1->setFixedHeight(20);
        line = new  QLineEdit;


        layout->addWidget(label1);
        layout->addWidget(sizes);
        layout->addWidget(label2);
        layout->addWidget(line);
        layout->addWidget(button);
        setLayout(layout);
        connect(button, SIGNAL(clicked()), this, SLOT(savePositionChanges()));
    }

    sizeDialog( QList<QString> list)//окно удаления размера
    {
        resize(400,200);
        setWindowTitle("Удаление должности");
        QVBoxLayout * layout = new QVBoxLayout;
        QLabel      * label2 = new QLabel("ВЫБЕРИТЕ ДОЛЖНОСТЬ ДЛЯ УДАЛЕНИЯ ");
        label2->setFixedHeight(20);
        sizes    = new  QComboBox ;
        sizes->addItems(list);

        QPushButton *button = new QPushButton("Удалить должность");
        layout->addWidget(label2);
        layout->addWidget(sizes);
        layout->addWidget(button);
        setLayout(layout);
        connect(button, SIGNAL(clicked()), this, SLOT(deleteChosenPos()));
    }
public slots:
    void saveSizeChanges()
    {
        emit setNewSize(line->text());
        done(1);
    }

    void savePositionChanges()
    {
        emit NewPosition(line->text(),sizes->currentText());
        done(1);
    }
    void deleteChosenSize()
    {
        emit  sendDelSize(sizes->currentText());
        done(1);
    }
    void deleteChosenPos()
    {
        emit  sendDelPos(sizes->currentText());
        done(1);
    }
signals:
    void setNewSize(QString name);
    void sendDelSize(QString name);
    void NewPosition(QString name, QString sphere);
    void sendDelPos(QString name);
};

#endif // SIZEDIALOG_H
