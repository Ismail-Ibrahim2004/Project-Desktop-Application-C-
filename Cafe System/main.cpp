#include <QApplication>
#include <QFont>
#include <QLocale>
#include <QMessageBox>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QDebug>

#include "windows/loginwindow.h"        // ⭐ بدلاً من mainwindow.h
#include "database/databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // ==============================
    // 🌍 Global App Settings
    // ==============================
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));

    QFont appFont("Segoe UI", 10);
    app.setFont(appFont);
    app.setLayoutDirection(Qt::LeftToRight);

    app.setApplicationName("Cafe Manager");
    app.setOrganizationName("CafeManager");
    app.setApplicationVersion("1.0.0");

    // ==============================
    // 🎨 ScrollBar Style
    // ==============================
    app.setStyleSheet(R"(
        QScrollBar:vertical {
            border: none;
            background: transparent;
            width: 8px;
            margin: 0;
        }
        QScrollBar::handle:vertical {
            background: rgba(0, 0, 0, 0.15);
            border-radius: 4px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background: rgba(0, 0, 0, 0.25);
        }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar:horizontal {
            border: none;
            background: transparent;
            height: 8px;
        }
        QScrollBar::handle:horizontal {
            background: rgba(0, 0, 0, 0.15);
            border-radius: 4px;
            min-width: 30px;
        }
        QScrollBar::handle:horizontal:hover {
            background: rgba(0, 0, 0, 0.25);
        }
    )");

    // ==============================
    // 🔌 Database Connection
    // ==============================
    DatabaseManager &db = DatabaseManager::instance();

    bool connected = db.connectToDatabase(
        "localhost\\SQLEXPRESS",
        "CafeManagerDB",
        "",
        ""
        );

    if (!connected) {
        QMessageBox::critical(
            nullptr,
            "Database Error",
            "Could not connect to SQL Server.\n"
            "Please check if SQL Server service is running.\n\n"
            "Error: " + db.lastError()
            );
        return -1;  // ⭐ نوقف البرنامج لإن فيه login محتاج DB
    } else {
        qDebug() << "✓ Database connected successfully!";
    }

    // ==============================
    // 🎯 App Icon Setup
    // ==============================
    QLabel tempLabel("☕");
    tempLabel.resize(64, 64);
    tempLabel.setAlignment(Qt::AlignCenter);
    tempLabel.setStyleSheet("font-size:45px; background:transparent;");

    QPixmap iconPixmap = tempLabel.grab();
    QIcon appIcon(iconPixmap);
    app.setWindowIcon(appIcon);

    // ==============================
    // 🔐 Login Window (نقطة البداية)
    // ==============================
    LoginWindow loginWindow;        // ⭐ بدلاً من MainWindow
    loginWindow.setWindowIcon(appIcon);
    loginWindow.show();

    return app.exec();
}