#include "mainwindow.h"
#include "databasemanager.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (!DatabaseManager::instance().connect()) {
        QMessageBox::critical(nullptr, "Database Error",
                              "Failed to connect to database!");
        return -1;
    }

    MainWindow w;
    w.show();
    return a.exec();
}