#include <QApplication>
#include <QFont>
#include <QLocale>
#include <QMessageBox>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QDebug>

#include "windows/loginwindow.h"
#include "database/databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // ==============================
    //  Global App Settings
    // ==============================
    QLocale::setDefault(QLocale(QLocale::English, QLocale::UnitedStates));

    QFont appFont("Segoe UI", 10);
    app.setFont(appFont);
    app.setLayoutDirection(Qt::LeftToRight);

    app.setApplicationName("Cafe Manager");
    app.setOrganizationName("CafeManager");
    app.setApplicationVersion("1.0.0");

    // ==============================
    //  Global Styles
    // ==============================
    app.setStyleSheet(R"(
    /* ============================== */
    /*  ScrollBars                  */
    /* ============================== */
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

    /* ============================== */
    /*  MessageBox - الحل النهائي  */
    /* ============================== */
    QMessageBox {
        background-color: #FFFFFF;
        color: #1A1A1A;
    }
    QMessageBox QLabel {
        color: #1A1A1A;
        font-size: 14px;
        background-color: transparent;
        min-width: 350px;
        padding: 10px;
    }
    QMessageBox QPushButton {
        background-color: #E67E22;
        color: white;
        border: 2px solid #E67E22;
        border-radius: 6px;
        padding: 8px 24px;
        font-size: 13px;
        font-weight: bold;
        min-width: 90px;
        min-height: 32px;
    }
    QMessageBox QPushButton:hover {
        background-color: #D35400;
        border: 2px solid #D35400;
        color: white;
    }
    QMessageBox QPushButton:pressed {
        background-color: #BA4A00;
        color: white;
    }
    QMessageBox QPushButton:focus {
        background-color: #E67E22;
        color: white;
        outline: none;
    }
    QMessageBox QPushButton:default {
        background-color: #634226;
        border: 2px solid #634226;
        color: white;
    }
    QMessageBox QPushButton:default:hover {
        background-color: #4D331D;
        border: 2px solid #4D331D;
        color: white;
    }

    /* ============================== */
    /*  ToolTip                     */
    /* ============================== */
    QToolTip {
        background-color: #2C2C2C;
        color: #FFFFFF;
        border: none;
        padding: 6px 10px;
        border-radius: 6px;
        font-size: 12px;
    }

    /* ============================== */
    /*  Menu                        */
    /* ============================== */
    QMenu {
        background-color: #FFFFFF;
        color: #1A1A1A;
        border: 1px solid #E5E0D8;
        border-radius: 6px;
        padding: 4px;
    }
    QMenu::item {
        background-color: transparent;
        padding: 8px 24px;
        border-radius: 4px;
        color: #1A1A1A;
    }
    QMenu::item:selected {
        background-color: #FAF6F1;
        color: #5D3A1A;
    }
    QMenu::separator {
        height: 1px;
        background-color: #E5E0D8;
        margin: 4px 0;
    }
)");

    // ==============================
    //  Database Connection
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
        return -1;
    } else {
        qDebug() << "✓ Database connected successfully!";
    }

    // ==============================
    //  App Icon Setup
    // ==============================
    QLabel tempLabel("☕");
    tempLabel.resize(64, 64);
    tempLabel.setAlignment(Qt::AlignCenter);
    tempLabel.setStyleSheet("font-size:45px; background:transparent;");

    QPixmap iconPixmap = tempLabel.grab();
    QIcon appIcon(iconPixmap);
    app.setWindowIcon(appIcon);

    // ==============================
    //  Login Window
    // ==============================
    LoginWindow loginWindow;
    loginWindow.setWindowIcon(appIcon);
    loginWindow.show();

    return app.exec();
}