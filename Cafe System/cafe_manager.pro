QT       += core gui widgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = CafeManager
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    database/databasemanager.cpp \
    models/dashboarddata.cpp \
    pages/dashboardpage.cpp \
    widgets/sidebar.cpp \
    widgets/statcard.cpp \
    widgets/chartwidget.cpp \
    widgets/barchartwidget.cpp \
    windows/adminwindow.cpp \
    windows/loginwindow.cpp \
    windows/poswindow.cpp

HEADERS += \
    mainwindow.h \
    database/databasemanager.h \
    models/dashboarddata.h \
    pages/dashboardpage.h \
    widgets/sidebar.h \
    widgets/statcard.h \
    widgets/chartwidget.h \
    widgets/barchartwidget.h \
    windows/adminwindow.h \
    windows/loginwindow.h \
    windows/poswindow.h

RESOURCES += \
    resources.qrc