#include "mainwindow.h"
#include <QApplication>

    int main(int argc, char *argv[])
{
// لحل مشاكل جودة الشاشات عالية الدقة (High DPI) في Qt
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}