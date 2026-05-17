QT += core gui widgets sql printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = CafeManager
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    database/databasemanager.cpp \
    models/dashboarddata.cpp \
    pages/Orderspage.cpp \
    pages/Settingspage.cpp \
    pages/dashboardpage.cpp \
    pages/employeespage.cpp \
    pages/inventorypage.cpp \
    pages/productspage.cpp \
    pages/reportspage.cpp \
    widgets/piechartwidget.cpp \
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
    pages/Orderspage.h \
    pages/Settingspage.h \
    pages/dashboardpage.h \
    pages/employeespage.h \
    pages/inventorypage.h \
    pages/productspage.h \
    pages/reportspage.h \
    widgets/piechartwidget.h \
    widgets/sidebar.h \
    widgets/statcard.h \
    widgets/chartwidget.h \
    widgets/barchartwidget.h \
    windows/adminwindow.h \
    windows/loginwindow.h \
    windows/poswindow.h

RESOURCES += \
    resources.qrc