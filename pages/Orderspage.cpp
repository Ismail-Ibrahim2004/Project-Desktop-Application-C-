#include "orderspage.h"
#include "../database/databasemanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QScrollArea>
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>

OrdersPage::OrdersPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadOrders();
}

void OrdersPage::refreshData()
{
    loadOrders();
}

// ==============================================
// Setup UI
// ==============================================

void OrdersPage::setupUI()
{
    setStyleSheet("background: #FAF6F1;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_stack = new QStackedWidget();
    setupListPage();
    setupDetailsPage();
    m_stack->addWidget(m_listPage);
    m_stack->addWidget(m_detailsPage);

    mainLayout->addWidget(m_stack);
}

// ==============================================
// Page 0: Orders List
// ==============================================

void OrdersPage::setupListPage()
{
    m_listPage = new QWidget();
    m_listPage->setStyleSheet("background: #FAF6F1;");

    QVBoxLayout *outerLayout = new QVBoxLayout(m_listPage);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: #FAF6F1; border: none; }");

    QWidget *content = new QWidget();
    content->setStyleSheet("background: #FAF6F1;");

    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(40, 35, 40, 35);
    layout->setSpacing(20);

    // Header
    QLabel *pageTitle = new QLabel("Order Management");
    pageTitle->setStyleSheet(
        "font-size: 30px; font-weight: bold; color: #1A1A1A; background: transparent;");

    QLabel *pageSub = new QLabel("View and manage customer orders");
    pageSub->setStyleSheet(
        "font-size: 14px; color: #888888; background: transparent;");

    layout->addWidget(pageTitle);
    layout->addWidget(pageSub);
    layout->addSpacing(5);

    // Search + Filter
    QWidget *searchCard = new QWidget();
    searchCard->setStyleSheet(R"(
        QWidget {
            background: white; border-radius: 12px;
            border: 1px solid rgba(0,0,0,0.07);
        }
    )");

    QGraphicsDropShadowEffect *searchShadow = new QGraphicsDropShadowEffect();
    searchShadow->setBlurRadius(15);
    searchShadow->setColor(QColor(0, 0, 0, 18));
    searchShadow->setOffset(0, 3);
    searchCard->setGraphicsEffect(searchShadow);

    QHBoxLayout *searchLayout = new QHBoxLayout(searchCard);
    searchLayout->setContentsMargins(16, 12, 16, 12);
    searchLayout->setSpacing(10);

    // Search icon from resources
    QLabel *searchIcon = new QLabel();
    searchIcon->setFixedSize(20, 20);
    searchIcon->setPixmap(
        QPixmap(":/icons/search-interface-symbol.png")
            .scaled(18, 18, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    searchIcon->setStyleSheet("background: transparent; border: none;");

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Search by order ID or employee...");
    m_searchEdit->setStyleSheet(R"(
        QLineEdit {
            border: none; font-size: 14px;
            color: #333333; background: transparent; padding: 2px;
        }
    )");

    m_statusFilter = new QComboBox();
    m_statusFilter->addItems({"All Status", "completed", "pending", "cancelled"});
    m_statusFilter->setFixedWidth(150);
    m_statusFilter->setStyleSheet(R"(
        QComboBox {
            border: 1px solid #D4C5B0; border-radius: 8px;
            padding: 6px 14px; font-size: 13px;
            color: #5D3A1A; background: white;
        }
        QComboBox::drop-down { border: none; width: 25px; }
        QComboBox QAbstractItemView {
            background: white; color: #5D3A1A;
            selection-background-color: #E8882F;
            selection-color: white;
        }
    )");

    searchLayout->addWidget(searchIcon);
    searchLayout->addWidget(m_searchEdit, 1);
    searchLayout->addWidget(m_statusFilter);

    layout->addWidget(searchCard);

    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &OrdersPage::onSearchChanged);
    connect(m_statusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &OrdersPage::onStatusFilterChanged);

    // Table Card
    QWidget *tableCard = new QWidget();
    tableCard->setStyleSheet("QWidget { background: white; border-radius: 16px; }");

    QGraphicsDropShadowEffect *tableShadow = new QGraphicsDropShadowEffect();
    tableShadow->setBlurRadius(20);
    tableShadow->setColor(QColor(0, 0, 0, 15));
    tableShadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(tableShadow);

    QVBoxLayout *tableCardLayout = new QVBoxLayout(tableCard);
    tableCardLayout->setContentsMargins(25, 20, 25, 20);
    tableCardLayout->setSpacing(15);

    QLabel *tableTitle = new QLabel("Order History");
    tableTitle->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #1A1A1A; background: transparent;");
    tableCardLayout->addWidget(tableTitle);

    setupOrdersTable();
    tableCardLayout->addWidget(m_ordersTable);

    layout->addWidget(tableCard);
    layout->addStretch();

    scrollArea->setWidget(content);
    outerLayout->addWidget(scrollArea);
}

void OrdersPage::setupOrdersTable()
{
    m_ordersTable = new QTableWidget();
    m_ordersTable->setColumnCount(7);
    m_ordersTable->setHorizontalHeaderLabels({
        "Order ID", "Employee", "Date", "Time", "Total Price", "Status", "Actions"
    });

    m_ordersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ordersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ordersTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ordersTable->setAlternatingRowColors(false);
    m_ordersTable->verticalHeader()->setVisible(false);
    m_ordersTable->setShowGrid(false);
    m_ordersTable->setFocusPolicy(Qt::NoFocus);
    m_ordersTable->setFrameShape(QFrame::NoFrame);
    m_ordersTable->verticalHeader()->setDefaultSectionSize(52);

    m_ordersTable->horizontalHeader()->setStretchLastSection(false);
    m_ordersTable->setColumnWidth(0, 120);
    m_ordersTable->setColumnWidth(1, 180);
    m_ordersTable->setColumnWidth(2, 130);
    m_ordersTable->setColumnWidth(3, 120);
    m_ordersTable->setColumnWidth(4, 120);
    m_ordersTable->setColumnWidth(5, 120);
    m_ordersTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);

    m_ordersTable->setStyleSheet(R"(
        QTableWidget {
            background: transparent; border: none;
            font-size: 14px; color: #374151;
        }
        QTableWidget::item {
            padding: 8px 12px;
            border-bottom: 1px solid #F3F4F6;
            background: transparent;
        }
        QTableWidget::item:selected {
            background-color: #FFF3E8; color: #1A1A1A;
        }
        QHeaderView::section {
            background: transparent; color: #6B7280;
            font-size: 13px; font-weight: 600;
            padding: 10px 12px; border: none;
            border-bottom: 2px solid #F3F4F6;
        }
    )");
}

// ==============================================
// Page 1: Order Details
// ==============================================

void OrdersPage::setupDetailsPage()
{
    m_detailsPage = new QWidget();
    m_detailsPage->setStyleSheet("background: #FAF6F1;");

    QVBoxLayout *outerLayout = new QVBoxLayout(m_detailsPage);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: #FAF6F1; border: none; }");

    QWidget *content = new QWidget();
    content->setStyleSheet("background: #FAF6F1;");

    QVBoxLayout *layout = new QVBoxLayout(content);
    layout->setContentsMargins(40, 35, 40, 35);
    layout->setSpacing(20);

    // Top Bar
    QHBoxLayout *topBar = new QHBoxLayout();

    QPushButton *backBtn = new QPushButton("  Back");
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setFixedSize(100, 36);
    backBtn->setIcon(QIcon(":/icons/logout.png"));
    backBtn->setIconSize(QSize(16, 16));
    backBtn->setStyleSheet(R"(
        QPushButton {
            background: transparent; color: #888;
            border: none; font-size: 14px; font-weight: 500;
        }
        QPushButton:hover { color: #5D3A1A; }
    )");
    connect(backBtn, &QPushButton::clicked, this, &OrdersPage::onBackToList);

    QVBoxLayout *titleLayout = new QVBoxLayout();
    QLabel *detailTitle = new QLabel("Order Details");
    detailTitle->setStyleSheet(
        "font-size: 28px; font-weight: bold; color: #1A1A1A; background: transparent;");

    m_detailOrderId = new QLabel("Order ID: ORD-001");
    m_detailOrderId->setStyleSheet(
        "font-size: 14px; color: #888888; background: transparent;");

    titleLayout->addWidget(detailTitle);
    titleLayout->addWidget(m_detailOrderId);

    // Print Button with icon
    QPushButton *printBtn = new QPushButton("  Print Receipt");
    printBtn->setFixedSize(160, 42);
    printBtn->setCursor(Qt::PointingHandCursor);
    printBtn->setIcon(QIcon(":/icons/printer.png"));
    printBtn->setIconSize(QSize(16, 16));
    printBtn->setStyleSheet(R"(
        QPushButton {
            background: #5D3A1A; color: white;
            border: none; border-radius: 10px;
            font-size: 13px; font-weight: bold;
        }
        QPushButton:hover { background: #432812; }
    )");
    connect(printBtn, &QPushButton::clicked, this, &OrdersPage::onPrintReceipt);

    topBar->addWidget(backBtn);
    topBar->addSpacing(10);
    topBar->addLayout(titleLayout);
    topBar->addStretch();
    topBar->addWidget(printBtn);
    layout->addLayout(topBar);

    // Content Row
    QHBoxLayout *contentRow = new QHBoxLayout();
    contentRow->setSpacing(20);
    contentRow->setAlignment(Qt::AlignTop);

    // Left: Order Information
    QWidget *infoCard = new QWidget();
    infoCard->setStyleSheet("QWidget { background: white; border-radius: 16px; }");

    QGraphicsDropShadowEffect *infoShadow = new QGraphicsDropShadowEffect();
    infoShadow->setBlurRadius(20);
    infoShadow->setColor(QColor(0, 0, 0, 15));
    infoShadow->setOffset(0, 4);
    infoCard->setGraphicsEffect(infoShadow);

    QVBoxLayout *infoLayout = new QVBoxLayout(infoCard);
    infoLayout->setContentsMargins(25, 22, 25, 22);
    infoLayout->setSpacing(20);

    QLabel *infoTitle = new QLabel("Order Information");
    infoTitle->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #1A1A1A; background: transparent;");
    infoLayout->addWidget(infoTitle);

    QGridLayout *infoGrid = new QGridLayout();
    infoGrid->setSpacing(20);

    auto createInfoField = [](const QString &label, QLabel *&valueLabel) -> QWidget* {
        QWidget *w = new QWidget();
        w->setStyleSheet("background: transparent;");
        QVBoxLayout *l = new QVBoxLayout(w);
        l->setContentsMargins(0, 0, 0, 0);
        l->setSpacing(4);
        QLabel *lbl = new QLabel(label);
        lbl->setStyleSheet("color: #888; font-size: 12px; background: transparent;");
        valueLabel = new QLabel("—");
        valueLabel->setStyleSheet(
            "color: #1A1A1A; font-size: 14px; font-weight: bold; background: transparent;");
        l->addWidget(lbl);
        l->addWidget(valueLabel);
        return w;
    };

    QLabel *statusValueLabel = nullptr;
    QWidget *orderIdField  = createInfoField("Order ID",    m_detailOrderId);
    QWidget *statusField   = createInfoField("Status",      statusValueLabel);
    QWidget *employeeField = createInfoField("Employee",    m_detailEmployee);
    QWidget *dateTimeField = createInfoField("Date & Time", m_detailDateTime);

    m_detailStatus = statusValueLabel;

    infoGrid->addWidget(orderIdField,  0, 0);
    infoGrid->addWidget(statusField,   0, 1);
    infoGrid->addWidget(employeeField, 1, 0);
    infoGrid->addWidget(dateTimeField, 1, 1);

    infoLayout->addLayout(infoGrid);

    // Right: Summary
    QWidget *summaryCard = new QWidget();
    summaryCard->setFixedWidth(300);
    summaryCard->setStyleSheet("QWidget { background: white; border-radius: 16px; }");

    QGraphicsDropShadowEffect *sumShadow = new QGraphicsDropShadowEffect();
    sumShadow->setBlurRadius(20);
    sumShadow->setColor(QColor(0, 0, 0, 15));
    sumShadow->setOffset(0, 4);
    summaryCard->setGraphicsEffect(sumShadow);

    QVBoxLayout *summaryLayout = new QVBoxLayout(summaryCard);
    summaryLayout->setContentsMargins(25, 22, 25, 22);
    summaryLayout->setSpacing(15);

    QLabel *summaryTitle = new QLabel("Summary");
    summaryTitle->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #1A1A1A; background: transparent;");
    summaryLayout->addWidget(summaryTitle);

    auto createSummaryRow = [&](const QString &label, QLabel *&valLabel, bool isTotal = false) {
        QHBoxLayout *row = new QHBoxLayout();
        QLabel *lbl = new QLabel(label);
        valLabel = new QLabel("$0.00");
        if (isTotal) {
            lbl->setStyleSheet(
                "font-size: 15px; font-weight: bold; color: #1A1A1A; background: transparent;");
            valLabel->setStyleSheet(
                "font-size: 18px; font-weight: bold; color: #E8882F; background: transparent;");
        } else {
            lbl->setStyleSheet("font-size: 14px; color: #555; background: transparent;");
            valLabel->setStyleSheet(
                "font-size: 14px; color: #1A1A1A; font-weight: 500; background: transparent;");
        }
        valLabel->setAlignment(Qt::AlignRight);
        row->addWidget(lbl);
        row->addStretch();
        row->addWidget(valLabel);
        summaryLayout->addLayout(row);
    };

    createSummaryRow("Subtotal", m_subtotalLabel);

    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setStyleSheet("background: #F0F0F0; max-height: 1px; border: none;");
    summaryLayout->addWidget(divider);

    createSummaryRow("Tax (8%)", m_taxLabel);
    createSummaryRow("Total",    m_totalLabel, true);
    summaryLayout->addStretch();

    contentRow->addWidget(infoCard, 1);
    contentRow->addWidget(summaryCard);
    layout->addLayout(contentRow);

    // Order Items Table
    QWidget *itemsCard = new QWidget();
    itemsCard->setStyleSheet("QWidget { background: white; border-radius: 16px; }");

    QGraphicsDropShadowEffect *itemsShadow = new QGraphicsDropShadowEffect();
    itemsShadow->setBlurRadius(20);
    itemsShadow->setColor(QColor(0, 0, 0, 15));
    itemsShadow->setOffset(0, 4);
    itemsCard->setGraphicsEffect(itemsShadow);

    QVBoxLayout *itemsLayout = new QVBoxLayout(itemsCard);
    itemsLayout->setContentsMargins(25, 22, 25, 22);
    itemsLayout->setSpacing(15);

    QLabel *itemsTitle = new QLabel("Order Items");
    itemsTitle->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #1A1A1A; background: transparent;");
    itemsLayout->addWidget(itemsTitle);

    m_itemsTable = new QTableWidget();
    m_itemsTable->setColumnCount(4);
    m_itemsTable->setHorizontalHeaderLabels({"Product", "Quantity", "Price", "Subtotal"});

    m_itemsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_itemsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_itemsTable->verticalHeader()->setVisible(false);
    m_itemsTable->setShowGrid(false);
    m_itemsTable->setFocusPolicy(Qt::NoFocus);
    m_itemsTable->setFrameShape(QFrame::NoFrame);
    m_itemsTable->verticalHeader()->setDefaultSectionSize(50);

    m_itemsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_itemsTable->setColumnWidth(1, 120);
    m_itemsTable->setColumnWidth(2, 120);
    m_itemsTable->setColumnWidth(3, 120);

    m_itemsTable->setStyleSheet(R"(
        QTableWidget {
            background: transparent; border: none;
            font-size: 14px; color: #374151;
        }
        QTableWidget::item {
            padding: 8px 12px;
            border-bottom: 1px solid #F3F4F6;
            background: transparent;
        }
        QHeaderView::section {
            background: transparent; color: #6B7280;
            font-size: 13px; font-weight: 600;
            padding: 10px 12px; border: none;
            border-bottom: 2px solid #F3F4F6;
        }
    )");

    itemsLayout->addWidget(m_itemsTable);
    layout->addWidget(itemsCard);
    layout->addStretch();

    scrollArea->setWidget(content);
    outerLayout->addWidget(scrollArea);
}

// ==============================================
// Load Data
// ==============================================

void OrdersPage::loadOrders()
{
    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery query(db);

    QString search = m_searchEdit->text().trimmed();
    QString status = m_statusFilter->currentText();

    QString sql = R"(
        SELECT o.OrderID,
               ISNULL(e.EmployeeName, 'N/A') as EmployeeName,
               CONVERT(VARCHAR(10), o.OrderDate, 120) as OrderDate,
               FORMAT(CAST(o.OrderTime AS DATETIME), 'hh:mm tt') as OrderTime,
               o.Total, o.Status
        FROM Orders o
        LEFT JOIN Employees e ON o.EmployeeID = e.EmployeeID
        WHERE 1=1
    )";

    if (!search.isEmpty()) {
        sql += QString(" AND (CAST(o.OrderID AS VARCHAR) LIKE '%%1%'"
                       " OR e.EmployeeName LIKE '%%1%')").arg(search);
    }
    if (status != "All Status") {
        sql += QString(" AND o.Status = '%1'").arg(status);
    }

    sql += " ORDER BY o.OrderDate DESC, o.OrderTime DESC";

    m_ordersTable->setRowCount(0);
    int row = 0;

    if (!query.exec(sql)) return;

    while (query.next()) {
        int     id          = query.value("OrderID").toInt();
        QString empName     = query.value("EmployeeName").toString();
        QString date        = query.value("OrderDate").toString();
        QString time        = query.value("OrderTime").toString();
        double  total       = query.value("Total").toDouble();
        QString orderStatus = query.value("Status").toString();

        m_ordersTable->insertRow(row);

        // Order ID
        QString ordIdStr = QString("ORD-%1").arg(id, 3, 10, QChar('0'));
        QTableWidgetItem *idItem = new QTableWidgetItem(ordIdStr);
        idItem->setForeground(QColor("#5D3A1A"));
        QFont f; f.setBold(true); idItem->setFont(f);
        idItem->setData(Qt::UserRole, id);
        m_ordersTable->setItem(row, 0, idItem);

        // Employee
        QTableWidgetItem *empItem = new QTableWidgetItem(empName);
        empItem->setForeground(QColor("#374151"));
        m_ordersTable->setItem(row, 1, empItem);

        // Date
        QTableWidgetItem *dateItem = new QTableWidgetItem(date);
        dateItem->setForeground(QColor("#555"));
        m_ordersTable->setItem(row, 2, dateItem);

        // Time
        QTableWidgetItem *timeItem = new QTableWidgetItem(time);
        timeItem->setForeground(QColor("#555"));
        m_ordersTable->setItem(row, 3, timeItem);

        // Total
        QTableWidgetItem *totalItem = new QTableWidgetItem(
            QString("$%1").arg(total, 0, 'f', 2));
        totalItem->setForeground(QColor("#1A1A1A"));
        QFont bf; bf.setBold(true); totalItem->setFont(bf);
        m_ordersTable->setItem(row, 4, totalItem);

        // Status Badge
        QWidget *statusWidget = new QWidget();
        statusWidget->setStyleSheet("background: transparent;");
        QHBoxLayout *statusLayout = new QHBoxLayout(statusWidget);
        statusLayout->setContentsMargins(10, 0, 10, 0);
        statusLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QLabel *badge = new QLabel(orderStatus);
        badge->setAlignment(Qt::AlignCenter);
        badge->setFixedHeight(26);
        QFontMetrics fm(badge->font());
        badge->setFixedWidth(fm.horizontalAdvance(orderStatus) + 24);

        if (orderStatus == "completed")
            badge->setStyleSheet("background:#DCFCE7; color:#16A34A; border-radius:13px; font-size:12px; font-weight:600;");
        else if (orderStatus == "pending")
            badge->setStyleSheet("background:#FEF3C7; color:#D97706; border-radius:13px; font-size:12px; font-weight:600;");
        else
            badge->setStyleSheet("background:#FEE2E2; color:#DC2626; border-radius:13px; font-size:12px; font-weight:600;");

        statusLayout->addWidget(badge);
        m_ordersTable->setCellWidget(row, 5, statusWidget);

        // View Details Button with icon
        QWidget *actionsWidget = new QWidget();
        actionsWidget->setStyleSheet("background: transparent;");
        QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
        actionsLayout->setContentsMargins(10, 0, 10, 0);
        actionsLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QPushButton *viewBtn = new QPushButton("  View Details");
        viewBtn->setCursor(Qt::PointingHandCursor);
        viewBtn->setIcon(QIcon(":/icons/visible.png"));
        viewBtn->setIconSize(QSize(16, 16));
        viewBtn->setStyleSheet(R"(
            QPushButton {
                background: transparent; color: #E8882F;
                border: none; font-size: 13px; font-weight: 600;
            }
            QPushButton:hover { color: #C06810; }
        )");
        connect(viewBtn, &QPushButton::clicked, this, [this, id]() {
            onViewDetails(id);
        });

        actionsLayout->addWidget(viewBtn);
        m_ordersTable->setCellWidget(row, 6, actionsWidget);

        row++;
    }
}

void OrdersPage::loadOrderDetails(int orderId)
{
    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");

    QSqlQuery infoQuery(db);
    infoQuery.prepare(R"(
        SELECT o.OrderID,
               ISNULL(e.EmployeeName, 'N/A') as EmployeeName,
               CONVERT(VARCHAR(10), o.OrderDate, 120) as OrderDate,
               FORMAT(CAST(o.OrderTime AS DATETIME), 'hh:mm tt') as OrderTime,
               o.Total, o.Status
        FROM Orders o
        LEFT JOIN Employees e ON o.EmployeeID = e.EmployeeID
        WHERE o.OrderID = ?
    )");
    infoQuery.addBindValue(orderId);

    if (!infoQuery.exec() || !infoQuery.next()) return;

    QString ordIdStr = QString("ORD-%1").arg(orderId, 3, 10, QChar('0'));
    QString empName  = infoQuery.value("EmployeeName").toString();
    QString date     = infoQuery.value("OrderDate").toString();
    QString time     = infoQuery.value("OrderTime").toString();
    double  total    = infoQuery.value("Total").toDouble();
    QString status   = infoQuery.value("Status").toString();

    m_detailOrderId->setText("Order ID: " + ordIdStr);
    m_detailEmployee->setText(empName);
    m_detailDateTime->setText(date + " at " + time);

    if (status == "completed")
        m_detailStatus->setStyleSheet(
            "background:#DCFCE7; color:#16A34A; border-radius:10px; "
            "font-size:12px; font-weight:600; padding:3px 12px;");
    else if (status == "pending")
        m_detailStatus->setStyleSheet(
            "background:#FEF3C7; color:#D97706; border-radius:10px; "
            "font-size:12px; font-weight:600; padding:3px 12px;");
    else
        m_detailStatus->setStyleSheet(
            "background:#FEE2E2; color:#DC2626; border-radius:10px; "
            "font-size:12px; font-weight:600; padding:3px 12px;");
    m_detailStatus->setText(status);

    QSqlQuery itemsQuery(db);
    itemsQuery.prepare(R"(
        SELECT p.ProductName, od.Quantity, od.UnitPrice,
               (od.Quantity * od.UnitPrice) as Subtotal
        FROM OrderDetails od
        INNER JOIN Products p ON od.ProductID = p.ProductID
        WHERE od.OrderID = ?
    )");
    itemsQuery.addBindValue(orderId);

    m_itemsTable->setRowCount(0);
    double subtotal = 0.0;
    int row = 0;

    if (itemsQuery.exec()) {
        while (itemsQuery.next()) {
            m_itemsTable->insertRow(row);

            QString productName = itemsQuery.value("ProductName").toString();
            int     qty         = itemsQuery.value("Quantity").toInt();
            double  price       = itemsQuery.value("UnitPrice").toDouble();
            double  sub         = itemsQuery.value("Subtotal").toDouble();
            subtotal += sub;

            QTableWidgetItem *nameItem  = new QTableWidgetItem(productName);
            QTableWidgetItem *qtyItem   = new QTableWidgetItem(QString::number(qty));
            QTableWidgetItem *priceItem = new QTableWidgetItem(
                QString("$%1").arg(price, 0, 'f', 2));
            QTableWidgetItem *subItem   = new QTableWidgetItem(
                QString("$%1").arg(sub, 0, 'f', 2));

            qtyItem->setTextAlignment(Qt::AlignCenter);
            priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            subItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

            QFont bf; bf.setBold(true);
            subItem->setFont(bf);

            m_itemsTable->setItem(row, 0, nameItem);
            m_itemsTable->setItem(row, 1, qtyItem);
            m_itemsTable->setItem(row, 2, priceItem);
            m_itemsTable->setItem(row, 3, subItem);
            row++;
        }
    }

    if (subtotal == 0.0) subtotal = total / 1.08;

    double tax        = subtotal * 0.08;
    double finalTotal = subtotal + tax;

    m_subtotalLabel->setText(QString("$%1").arg(subtotal, 0, 'f', 2));
    m_taxLabel->setText(QString("$%1").arg(tax, 0, 'f', 2));
    m_totalLabel->setText(QString("$%1").arg(finalTotal, 0, 'f', 2));

    m_currentOrderId = orderId;
}

// ==============================================
// Slots
// ==============================================

void OrdersPage::onSearchChanged(const QString &)  { loadOrders(); }
void OrdersPage::onStatusFilterChanged(int)        { loadOrders(); }

void OrdersPage::onViewDetails(int orderId)
{
    loadOrderDetails(orderId);
    m_stack->setCurrentIndex(1);
}

void OrdersPage::onBackToList()
{
    m_stack->setCurrentIndex(0);
    loadOrders();
}

void OrdersPage::onPrintReceipt()
{
    if (m_currentOrderId < 0) return;

    QString rowsHtml = "";
    for (int i = 0; i < m_itemsTable->rowCount(); i++) {
        rowsHtml += QString("<tr><td>%1</td><td align='center'>%2</td>"
                            "<td align='right'>%3</td><td align='right'><b>%4</b></td></tr>")
                        .arg(m_itemsTable->item(i,0)->text(),
                             m_itemsTable->item(i,1)->text(),
                             m_itemsTable->item(i,2)->text(),
                             m_itemsTable->item(i,3)->text());
    }

    QString html = QString(R"(
        <html><body style='font-family: Segoe UI, sans-serif; color: #333;'>
        <h2 align='center' style='color: #5D3A1A;'>Café Manager</h2>
        <p align='center' style='font-size:12px; color:#666;'>
            123 Coffee Street<br>Tel: (555) 123-4567</p>
        <hr style='border:1px dashed #CCC;'>
        <p align='center'><b>%1</b><br>%2<br>Employee: %3</p>
        <hr style='border:1px dashed #CCC;'>
        <table width='100%' cellspacing='0' cellpadding='5' style='font-size:12px;'>
            <tr style='background:#FAF6F1;'>
                <th align='left'>Product</th>
                <th align='center'>Qty</th>
                <th align='right'>Price</th>
                <th align='right'>Subtotal</th>
            </tr>
            %4
        </table>
        <hr style='border:1px dashed #CCC;'>
        <table width='100%' style='font-size:12px;'>
            <tr><td>Subtotal</td><td align='right'>%5</td></tr>
            <tr><td>Tax (8%%)</td><td align='right'>%6</td></tr>
            <tr><td><b>Total</b></td>
                <td align='right'><b style='color:#E67E22; font-size:16px;'>%7</b></td></tr>
        </table>
        <hr style='border:1px dashed #CCC;'>
        <p align='center'><b>Thank you for your visit!</b></p>
        </body></html>
    )").arg(m_detailOrderId->text(),
                            m_detailDateTime->text(),
                            m_detailEmployee->text(),
                            rowsHtml,
                            m_subtotalLabel->text(),
                            m_taxLabel->text(),
                            m_totalLabel->text());

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        QTextDocument doc;
        doc.setHtml(html);
        doc.print(&printer);
    }
}