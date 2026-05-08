#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ضبط حجم النافذة ومظهرها العام
    this->resize(1200, 700);
    this->setWindowTitle("Café Manager - Order Management");
    this->setStyleSheet("background-color: #FAF6F0; font-family: 'Segoe UI', Arial, sans-serif;");

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. إنشاء القائمة الجانبية (Sidebar)
    setupSidebar();
    mainLayout->addWidget(sidebar);

    // 2. إنشاء منطقة المحتوى الرئيسية
    setupMainContent();
    mainLayout->addWidget(mainContentArea);
}

MainWindow::~MainWindow() {}

void MainWindow::setupSidebar()
{
    sidebar = new QWidget(this);
    sidebar->setFixedWidth(220);
    sidebar->setStyleSheet("background-color: #5C3A21;"); // اللون البني الداكن للشريط الجانبي

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(0, 20, 0, 20);
    sidebarLayout->setSpacing(5);

    // شعار التطبيق (Logo Area)
    QWidget *logoWidget = new QWidget();
    QHBoxLayout *logoLayout = new QHBoxLayout(logoWidget);
    logoLayout->setContentsMargins(20, 10, 20, 30);

    QLabel *logoIcon = new QLabel("☕"); // رمز بديل للأيقونة
    logoIcon->setStyleSheet("font-size: 24px; color: white;");
    QLabel *logoText = new QLabel("Café Manager\n<span style='font-size: 11px; color: #C4A48A;'>Admin</span>");
    logoText->setStyleSheet("color: white; font-weight: bold; font-size: 16px;");

    logoLayout->addWidget(logoIcon);
    logoLayout->addWidget(logoText);
    logoLayout->addStretch();
    sidebarLayout->addWidget(logoWidget);

    // أزرار القائمة الجانبية (Navigation Items)
    QStringList menuItems = {"Dashboard", "Products", "Orders", "Inventory", "Employees", "Reports", "Settings"};
    QStringList menuIcons = {"📊", "🛍️", "🛒", "📦", "👥", "📈", "⚙️"};

    for (int i = 0; i < menuItems.size(); ++i) {
        QPushButton *btn = new QPushButton("  " + menuIcons[i] + "   " + menuItems[i]);
        btn->setFlat(true);
        btn->setCursor(Qt::PointingHandCursor);

        if (menuItems[i] == "Orders") {
            // الزر النشط باللون البرتقالي مثل الصورة
            btn->setStyleSheet(
                "QPushButton {"
                "   color: white;"
                "   background-color: #E87722;"
                "   border-top-right-radius: 12px;"
                "   border-bottom-right-radius: 12px;"
                "   text-align: left;"
                "   padding: 12px 20px;"
                "   font-size: 14px;"
                "   font-weight: bold;"
                "   margin-right: 15px;"
                "}"
                );
        } else {
            // الأزرار العادية غير النشطة
            btn->setStyleSheet(
                "QPushButton {"
                "   color: #D3C2B5;"
                "   text-align: left;"
                "   padding: 12px 20px;"
                "   font-size: 14px;"
                "   border: none;"
                "}"
                "QPushButton:hover {"
                "   color: white;"
                "   background-color: #4A2E1A;"
                "   border-top-right-radius: 12px;"
                "   border-bottom-right-radius: 12px;"
                "   margin-right: 15px;"
                "}"
                );
        }
        sidebarLayout->addWidget(btn);
    }

    sidebarLayout->addStretch(); // دفع زر تسجيل الخروج لأسفل الصفحة

    // زر تسجيل الخروج (Logout)
    QPushButton *logoutBtn = new QPushButton("  🔑   Logout");
    logoutBtn->setFlat(true);
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setStyleSheet(
        "QPushButton {"
        "   color: #D3C2B5;"
        "   text-align: left;"
        "   padding: 15px 20px;"
        "   font-size: 14px;"
        "   border: none;"
        "}"
        "QPushButton:hover { color: #FFA07A; }"
        );
    sidebarLayout->addWidget(logoutBtn);
}

void MainWindow::setupMainContent()
{
    mainContentArea = new QWidget(this);
    QVBoxLayout *mainContentLayout = new QVBoxLayout(mainContentArea);
    mainContentLayout->setContentsMargins(30, 30, 30, 30);
    mainContentLayout->setSpacing(20);

    // 1. ترويسة الصفحة (Header)
    QVBoxLayout *headerLayout = new QVBoxLayout();
    headerLayout->setSpacing(5);
    QLabel *titleLabel = new QLabel("Order Management");
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold; color: #5C3A21;");
    QLabel *subtitleLabel = new QLabel("View and manage customer orders");
    subtitleLabel->setStyleSheet("font-size: 13px; color: #8A7363;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(subtitleLabel);
    mainContentLayout->addLayout(headerLayout);

    // 2. شريط البحث والتصفية (Search and Filter bar)
    QWidget *filterCard = new QWidget();
    filterCard->setStyleSheet("background-color: white; border-radius: 12px; border: 1px solid #EADEC9;");
    QHBoxLayout *filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setContentsMargins(15, 15, 15, 15);
    filterLayout->setSpacing(15);

    searchField = new QLineEdit();
    searchField->setPlaceholderText("🔍  Search by order ID or employee...");
    searchField->setStyleSheet(
        "QLineEdit {"
        "   background-color: #FAF6F0;"
        "   border: 1px solid #EADEC9;"
        "   border-radius: 8px;"
        "   padding: 10px 15px;"
        "   color: #5C3A21;"
        "   font-size: 13px;"
        "}"
        );

    statusFilter = new QComboBox();
    statusFilter->addItems({"All Status", "Completed", "Pending", "Cancelled"});
    statusFilter->setFixedWidth(150);
    statusFilter->setStyleSheet(
        "QComboBox {"
        "   background-color: #FAF6F0;"
        "   border: 1px solid #EADEC9;"
        "   border-radius: 8px;"
        "   padding: 10px 15px;"
        "   color: #5C3A21;"
        "   font-size: 13px;"
        "}"
        "QComboBox::drop-down { border: none; width: 30px; }"
        );

    filterLayout->addWidget(searchField, 4); // شريط البحث يأخذ مساحة أكبر
    filterLayout->addWidget(statusFilter, 1);
    mainContentLayout->addWidget(filterCard);

    // 3. كرت جدول البيانات (Order History Container)
    QWidget *tableCard = new QWidget();
    tableCard->setStyleSheet("background-color: white; border-radius: 12px; border: 1px solid #EADEC9;");
    QVBoxLayout *tableCardLayout = new QVBoxLayout(tableCard);
    tableCardLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *tableTitle = new QLabel("Order History");
    tableTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #5C3A21; margin-bottom: 10px;");
    tableCardLayout->addWidget(tableTitle);

    // إنشاء الجدول وتنسيقه
    orderTable = new QTableWidget();
    orderTable->setColumnCount(7);
    orderTable->setHorizontalHeaderLabels({"Order ID", "Employee", "Date", "Time", "Total Price", "Status", "Actions"});
    orderTable->setShowGrid(false); // إلغاء خطوط الشبكة الداخلية للحصول على مظهر نظيف ومسطح
    orderTable->setFocusPolicy(Qt::NoFocus);
    orderTable->setSelectionMode(QAbstractItemView::NoSelection);

    // تنسيق ترويسة الجدول (Header Style)
    orderTable->horizontalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "   background-color: white;"
        "   color: #8A7363;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "   border: none;"
        "   border-bottom: 1px solid #EADEC9;"
        "   padding: 10px 5px;"
        "   text-align: left;"
        "}"
        );
    orderTable->horizontalHeader()->setStretchLastSection(true);
    orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // تنسيق الخلايا
    orderTable->setStyleSheet(
        "QTableWidget {"
        "   background-color: white;"
        "   gridline-color: transparent;"
        "   border: none;"
        "   color: #333333;"
        "}"
        "QTableWidget::item {"
        "   border-bottom: 1px solid #FAF6F0;"
        "   padding: 12px 5px;"
        "}"
        );

    populateTableData(); // ملء البيانات
    tableCardLayout->addWidget(orderTable);
    mainContentLayout->addWidget(tableCard);
}

// دالة لإنشاء شارات الحالة الملونة (Completed, Pending, Cancelled)
QWidget* MainWindow::createStatusBadge(const QString &status)
{
    QWidget *badgeContainer = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(badgeContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *badge = new QLabel(status);
    badge->setAlignment(Qt::AlignCenter);

    // تطبيق الألوان بناءً على الحالة المعطاة في الصورة
    if (status == "completed") {
        badge->setStyleSheet(
            "background-color: #E6F7ED; color: #219653;"
            "border-radius: 6px; padding: 4px 10px; font-weight: bold; font-size: 11px;"
            );
    } else if (status == "pending") {
        badge->setStyleSheet(
            "background-color: #FFF9E6; color: #F2C94C;"
            "border-radius: 6px; padding: 4px 10px; font-weight: bold; font-size: 11px;"
            );
    } else if (status == "cancelled") {
        badge->setStyleSheet(
            "background-color: #FDE8E8; color: #EB5757;"
            "border-radius: 6px; padding: 4px 10px; font-weight: bold; font-size: 11px;"
            );
    }

    layout->addWidget(badge);
    return badgeContainer;
}

void MainWindow::populateTableData()
{
    // مصفوفة تحتوي على البيانات الموجودة في صورة الجدول تماماً
    struct Order {
        QString id;
        QString employee;
        QString date;
        QString time;
        QString price;
        QString status;
    };

    QVector<Order> orders = {
        {"ORD-001", "John Doe", "2026-03-07", "10:30 AM", "$24.50", "completed"},
        {"ORD-002", "Jane Smith", "2026-03-07", "11:15 AM", "$18.75", "completed"},
        {"ORD-003", "Mike Johnson", "2026-03-07", "11:45 AM", "$32.00", "completed"},
        {"ORD-004", "Sarah Brown", "2026-03-07", "12:30 PM", "$15.25", "pending"},
        {"ORD-005", "John Doe", "2026-03-07", "01:00 PM", "$42.50", "completed"},
        {"ORD-006", "Emily Davis", "2026-03-06", "02:15 PM", "$28.00", "completed"},
        {"ORD-007", "Chris Wilson", "2026-03-06", "03:00 PM", "$36.75", "completed"},
        {"ORD-008", "Jane Smith", "2026-03-06", "04:30 PM", "$21.50", "cancelled"},
        {"ORD-009", "Mike Johnson", "2026-03-05", "09:45 AM", "$45.00", "completed"},
        {"ORD-010", "Sarah Brown", "2026-03-05", "11:00 AM", "$19.25", "completed"}
    };

    orderTable->setRowCount(orders.size());

    for (int i = 0; i < orders.size(); ++i) {
        // ID
        QTableWidgetItem *itemId = new QTableWidgetItem(orders[i].id);
        itemId->setForeground(QBrush(QColor("#5C3A21")));
        itemId->setFont(QFont("Segoe UI", 9, QFont::Bold));
        orderTable->setItem(i, 0, itemId);

        // Employee
        orderTable->setItem(i, 1, new QTableWidgetItem(orders[i].employee));

        // Date
        orderTable->setItem(i, 2, new QTableWidgetItem(orders[i].date));

        // Time
        orderTable->setItem(i, 3, new QTableWidgetItem(orders[i].time));

        // Price
        QTableWidgetItem *itemPrice = new QTableWidgetItem(orders[i].price);
        itemPrice->setFont(QFont("Segoe UI", 9, QFont::Bold));
        orderTable->setItem(i, 4, itemPrice);

        // Status (تطبيق الشارة الملونة داخل الخلية)
        QWidget *statusBadge = createStatusBadge(orders[i].status);
        orderTable->setCellWidget(i, 5, statusBadge);

        // Actions (زر التفاصيل "👁️ View Details")
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 0, 0);
        actionLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QPushButton *actionBtn = new QPushButton(" 👁️  View Details");
        actionBtn->setFlat(true);
        actionBtn->setCursor(Qt::PointingHandCursor);
        actionBtn->setStyleSheet(
            "QPushButton {"
            "   color: #5C3A21;"
            "   border: none;"
            "   font-size: 12px;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover { color: #E87722; text-decoration: underline; }"
            );
        actionLayout->addWidget(actionBtn);
        orderTable->setCellWidget(i, 6, actionWidget);
    }
}