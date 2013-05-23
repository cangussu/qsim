TARGET   = qsim.exe
TEMPLATE = app
CONFIG += qt debug
QT += gui core widgets

INCLUDEPATH = src

FORMS = src/configdialog.ui

HEADERS += src/agent.h \
           src/goal.h \
           src/mainwindow.h \
           src/obstacle.h \
           src/renderarea.h \
           src/simulator.h \
           src/object.h \
           src/configdialog.h \
           src/building.h \
           src/node.h \
           src/navmesh.h \
           src/thief.h \
    src/graph.h

SOURCES += src/agent.cpp \
           src/goal.cpp \
           src/main.cpp \
           src/mainwindow.cpp \
           src/obstacle.cpp \
           src/renderarea.cpp \
           src/simulator.cpp \
           src/object.cpp \
           src/configdialog.cpp \
           src/building.cpp \
           src/node.cpp \
           src/navmesh.cpp \
    src/graph.cc

RESOURCES = src/qsim.qrc
