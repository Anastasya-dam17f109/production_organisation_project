
QT += core gui
QT += widgets
QT += sql
HEADERS += \
    deliverywidget.h \
    dialogs/currentorderview.h \
    dialogs/deliverdialog.h \
    dialogs/executeoperation.h \
    dialogs/opendialog.h \
    dialogs/responsibledialog.h \
    dialogs/stockerdetdialog.h \
    dialogs/workerdialog.h \
    graphicEdit/edge.h \
    graphicEdit/edgenode.h \
    graphicEdit/graphicedit.h \
    graphicEdit/graphwidget.h \
    graphicEdit/node.h \
    graphicEdit/paintwidget.h \
    graphicEdit/scene.h \
    adminTabWidget.h \
    dialogs/namedialog.h \
    dialogs/sizedialog.h \
    dialogs/usersdialog.h \
    dialogs/userselectdialog.h \
    dialogs/stockdialog.h \
    dialogs/operationdialog.h \
    dialogs/addorderdialog.h \
    dialogs/deleteorderdialog.h \
    managerwidget.h \
    stockerwidget.h \
    workerwidget.h

SOURCES += \
    deliverywidget.cpp \
        edge.cpp \
        main.cpp \
    managerwidget.cpp \
        node.cpp \
        graphwidget.cpp \
    paintwidget.cpp \
    scene.cpp \
    edgenode.cpp \
    graphicedit.cpp \
    admintabwidget.cpp \
    stockerwidget.cpp \
    workerwidget.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/graphicsview/elasticnodes
INSTALLS += target

RESOURCES += \
    icons.qrc
