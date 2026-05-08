QT += core gui widgets sql

CONFIG += c++17

TARGET = CafeEmployeeManagement
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    database.cpp \
    addemployeedialog.cpp \
    dashboardwidget.cpp

HEADERS += \
    mainwindow.h \
    database.h \
    addemployeedialog.h \
    dashboardwidget.h

RESOURCES += \
    resources.qrc
