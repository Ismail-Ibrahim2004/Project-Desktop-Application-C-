#include "loginwindow.h"
#include "adminwindow.h"
#include "../database/databasemanager.h"
#include "poswindow.h"       // ⭐ شاشة الموظف
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QMessageBox>
#include <QButtonGroup>
#include <QPushButton>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Cafe Manager - Login");
    resize(900, 750);

    this->setStyleSheet(
        "QMainWindow { "
        "background: qradialgradient(cx:0.5, cy:0.5, radius:1, fx:0.5, fy:0.5, "
        "stop:0 #8B5A2B, stop:1 #5D3A1A); "
        "}"
        );

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setAlignment(Qt::AlignCenter);

    // Login Card
    QFrame *card = new QFrame();
    card->setFixedSize(400, 680);
    card->setStyleSheet("QFrame { background: white; border-radius: 20px; }");

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 40, 40, 40);
    cardLayout->setSpacing(0);

    // Coffee Icon
    QLabel *icon = new QLabel("☕");
    icon->setAlignment(Qt::AlignCenter);
    icon->setFixedSize(80, 80);
    icon->setStyleSheet("background: #E68A4F; color: white; font-size: 38px; "
                        "border-radius: 40px; border: none;");

    QHBoxLayout *iconLayout = new QHBoxLayout();
    iconLayout->addWidget(icon);
    iconLayout->setAlignment(Qt::AlignCenter);
    cardLayout->addLayout(iconLayout);
    cardLayout->addSpacing(20);

    // Title
    QLabel *title = new QLabel("Cafe Manager");
    title->setStyleSheet("font-size: 28px; font-weight: bold; color: #5D3A1A; "
                         "background: transparent;");
    title->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(title);

    QLabel *sub = new QLabel("Management System");
    sub->setStyleSheet("color: #888; font-size: 14px; background: transparent;");
    sub->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(sub);
    cardLayout->addSpacing(30);

    // Login As
    QLabel *loginAs = new QLabel("Login As");
    loginAs->setStyleSheet("font-size: 13px; font-weight: bold; color: #333; "
                           "background: transparent;");
    cardLayout->addWidget(loginAs);
    cardLayout->addSpacing(10);

    // Radio Buttons
    QString radioStyle =
        "QRadioButton { background: #F8F5F2; padding: 12px; border-radius: 10px; "
        "color: #444; font-size: 13px; } "
        "QRadioButton::indicator { width: 18px; height: 18px; border-radius: 9px; "
        "border: 1px solid #D2A679; background: white; } "
        "QRadioButton::indicator:checked { background-color: #8B5A2B; "
        "border: 4px solid white; }";

    adminRadio = new QRadioButton("Admin / Manager");
    empRadio = new QRadioButton("Employee");
    adminRadio->setChecked(true);
    adminRadio->setStyleSheet(radioStyle);
    empRadio->setStyleSheet(radioStyle);

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(adminRadio);
    group->addButton(empRadio);

    cardLayout->addWidget(adminRadio);
    cardLayout->addSpacing(10);
    cardLayout->addWidget(empRadio);
    cardLayout->addSpacing(25);

    // Input Styles
    QString labelStyle = "font-size: 13px; font-weight: bold; color: #333; "
                         "background: transparent;";
    QString inputStyle = "QLineEdit { border: 1.5px solid #D2A679; "
                         "border-radius: 10px; padding: 12px; font-size: 14px; "
                         "background: white; color: #222; } "
                         "QLineEdit:focus { border: 2px solid #8B5A2B; }";

    // Username
    QLabel *userLabel = new QLabel("Username");
    userLabel->setStyleSheet(labelStyle);
    cardLayout->addWidget(userLabel);
    cardLayout->addSpacing(5);

    userEdit = new QLineEdit();
    userEdit->setPlaceholderText("Enter username");
    userEdit->setStyleSheet(inputStyle);
    cardLayout->addWidget(userEdit);
    cardLayout->addSpacing(15);

    // Password
    QLabel *passLabel = new QLabel("Password");
    passLabel->setStyleSheet(labelStyle);
    cardLayout->addWidget(passLabel);
    cardLayout->addSpacing(5);

    passEdit = new QLineEdit();
    passEdit->setPlaceholderText("Enter password");
    passEdit->setEchoMode(QLineEdit::Password);
    passEdit->setStyleSheet(inputStyle);
    cardLayout->addWidget(passEdit);
    cardLayout->addSpacing(35);

    // Login Button
    QPushButton *loginBtn = new QPushButton("Login");
    loginBtn->setFixedHeight(50);
    loginBtn->setCursor(Qt::PointingHandCursor);
    loginBtn->setStyleSheet("QPushButton { background: #634226; color: white; "
                            "border: none; border-radius: 10px; font-size: 16px; "
                            "font-weight: bold; } "
                            "QPushButton:hover { background: #4D331D; }");
    cardLayout->addWidget(loginBtn);

    cardLayout->addStretch();

    // Footer
    QLabel *footer = new QLabel("© 2026 Cafe Manager. All rights reserved.");
    footer->setStyleSheet("color: #999; font-size: 11px; background: transparent;");
    footer->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(footer);

    mainLayout->addWidget(card);

    // Connections
    connect(loginBtn, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    connect(passEdit, &QLineEdit::returnPressed, this, &LoginWindow::handleLogin);
}

LoginWindow::~LoginWindow() {}

void LoginWindow::handleLogin()
{
    // ⭐ التحقق من الاتصال عن طريق DatabaseManager
    if (!DatabaseManager::instance().isConnected()) {
        QMessageBox::critical(
            this,
            "Connection Error",
            "Database connection lost!\n\n" +
                DatabaseManager::instance().lastError()
            );
        return;
    }

    QString username = userEdit->text().trimmed();
    QString password = passEdit->text().trimmed();
    QString role = adminRadio->isChecked() ? "Admin" : "Employee";

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Input Required", "Please enter username and password");
        return;
    }

    // ⭐ استخدام الـ connection الصحيح
    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery query(db);

    query.prepare("SELECT role FROM users WHERE username = :u AND password = :p AND role = :r");
    query.bindValue(":u", username);
    query.bindValue(":p", password);
    query.bindValue(":r", role);

    if (!query.exec()) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
        return;
    }

    if (query.next()) {
        currentUsername = username;

        // التوجيه حسب الدور
        if (role == "Admin") {
            // ===== Admin → AdminWindow =====
            AdminWindow *adminWin = new AdminWindow();
            adminWin->setAttribute(Qt::WA_DeleteOnClose);
            adminWin->setUserInfo(username, "Admin");

            adminWin->show();
            this->hide();

            connect(adminWin, &QObject::destroyed, this, [this]() {
                userEdit->clear();
                passEdit->clear();
                userEdit->setFocus();
                this->show();
                this->raise();
                this->activateWindow();
            });
        }
        else {
            // ===== Employee → POSWindow =====
            POSWindow *posWin = new POSWindow();
            posWin->setAttribute(Qt::WA_DeleteOnClose);
            posWin->setUserInfo(username);

            posWin->show();
            this->hide();

            connect(posWin, &QObject::destroyed, this, [this]() {
                userEdit->clear();
                passEdit->clear();
                userEdit->setFocus();
                this->show();
                this->raise();
                this->activateWindow();
            });
        }
    }
    else {
        QMessageBox::warning(this, "Login Failed", "Invalid username, password, or role");
        passEdit->clear();
        passEdit->setFocus();
    }
}