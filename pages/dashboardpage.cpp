#include "dashboardpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QScrollArea>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>

DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent)
{
    m_dataModel = new DashboardDataModel();
    setupUI();
    refreshData();
}

void DashboardPage::setupUI()
{
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(R"(
        QScrollArea {
            background-color: #F8F5F1;
            border: none;
        }
    )");

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background-color: #F8F5F1;");

    QVBoxLayout *mainLayout = new QVBoxLayout(scrollContent);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    mainLayout->setSpacing(25);

    // ==============================
    // Header
    // ==============================
    QLabel *headerTitle = new QLabel("Dashboard");
    headerTitle->setStyleSheet(R"(
        QLabel {
            font-size: 32px;
            font-weight: 700;
            color: #1F1F1F;
            background: transparent;
        }
    )");

    QLabel *headerSubtitle = new QLabel("Welcome back! Here's your café overview.");
    headerSubtitle->setStyleSheet(R"(
        QLabel {
            font-size: 15px;
            color: #888888;
            background: transparent;
        }
    )");

    mainLayout->addWidget(headerTitle);
    mainLayout->addWidget(headerSubtitle);
    mainLayout->addSpacing(10);

    // ==============================
    // Stat Cards
    // ==============================
    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(20);

    m_revenueCard = new StatCard(
        "Total Revenue",
        "$42,580",
        "+12.5% from last month",
        ":/icons/doller.png",
        QColor(76, 175, 80)
        );

    m_ordersCard = new StatCard(
        "Total Orders",
        "1,248",
        "+8.2% from last month",
        ":/icons/checkout.png",
        QColor(33, 150, 243)
        );

    m_employeesCard = new StatCard(
        "Active Employees",
        "24",
        "4 working right now",
        ":/icons/employee.png",
        QColor(232, 136, 47)
        );

    m_topProductCard = new StatCard(
        "Top Selling\nProduct",
        "Cappuccino",
        "342 orders this month",
        ":/icons/bar-chart.png",
        QColor(136, 136, 136)
        );

    cardsLayout->addWidget(m_revenueCard);
    cardsLayout->addWidget(m_ordersCard);
    cardsLayout->addWidget(m_employeesCard);
    cardsLayout->addWidget(m_topProductCard);

    mainLayout->addLayout(cardsLayout);

    // ==============================
    // Charts
    // ==============================
    QHBoxLayout *chartsLayout = new QHBoxLayout();
    chartsLayout->setSpacing(20);

    m_lineChart = new ChartWidget("Weekly Sales");
    m_barChart = new BarChartWidget("Orders by Day");

    chartsLayout->addWidget(m_lineChart, 1);
    chartsLayout->addWidget(m_barChart, 1);

    mainLayout->addLayout(chartsLayout);

    // ==============================
    // Recent Orders Section
    // ==============================
    QWidget *ordersCard = new QWidget();
    ordersCard->setStyleSheet(R"(
        QWidget {
            background-color: white;
            border-radius: 16px;
        }
    )");

    QGraphicsDropShadowEffect *ordersShadow = new QGraphicsDropShadowEffect(ordersCard);
    ordersShadow->setBlurRadius(20);
    ordersShadow->setColor(QColor(0, 0, 0, 15));
    ordersShadow->setOffset(0, 4);
    ordersCard->setGraphicsEffect(ordersShadow);

    QVBoxLayout *ordersCardLayout = new QVBoxLayout(ordersCard);
    ordersCardLayout->setContentsMargins(25, 22, 25, 22);
    ordersCardLayout->setSpacing(15);

    QLabel *recentOrdersTitle = new QLabel("Recent Orders");
    QFont recentTitleFont("Segoe UI", 16);
    recentTitleFont.setWeight(QFont::DemiBold);
    recentOrdersTitle->setFont(recentTitleFont);
    recentOrdersTitle->setStyleSheet("color: #1A1A1A; background: transparent; border: none;");
    ordersCardLayout->addWidget(recentOrdersTitle);

    setupRecentOrdersTable();

    m_recentOrdersTable->setGraphicsEffect(nullptr);
    m_recentOrdersTable->setStyleSheet(R"(
        QTableWidget {
            background-color: white;
            border: none;
            font-size: 14px;
        }
        QTableWidget::item {
            padding: 12px 15px;
            border-bottom: 1px solid #F0F0F0;
            color: #333333;
        }
        QTableWidget::item:selected {
            background-color: #FFF3E8;
            color: #333333;
        }
        QHeaderView::section {
            background-color: white;
            color: #1A1A1A;
            font-weight: bold;
            font-size: 14px;
            padding: 12px 15px;
            border: none;
            border-bottom: 2px solid #F0F0F0;
        }
    )");

    ordersCardLayout->addWidget(m_recentOrdersTable);

    mainLayout->addWidget(ordersCard);
    mainLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    outerLayout->addWidget(scrollArea);
}

void DashboardPage::setupRecentOrdersTable()
{
    m_recentOrdersTable = new QTableWidget();
    m_recentOrdersTable->setColumnCount(6);
    m_recentOrdersTable->setHorizontalHeaderLabels({
        "Order ID", "Employee", "Date", "Time", "Total", "Status"
    });

    m_recentOrdersTable->setMinimumHeight(350);
    m_recentOrdersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_recentOrdersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_recentOrdersTable->setAlternatingRowColors(false);
    m_recentOrdersTable->verticalHeader()->setVisible(false);
    m_recentOrdersTable->setShowGrid(false);
    m_recentOrdersTable->setFocusPolicy(Qt::NoFocus);
    m_recentOrdersTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_recentOrdersTable->horizontalHeader()->setStretchLastSection(true);

    m_recentOrdersTable->setColumnWidth(0, 150);
    m_recentOrdersTable->setColumnWidth(1, 180);
    m_recentOrdersTable->setColumnWidth(2, 160);
    m_recentOrdersTable->setColumnWidth(3, 140);
    m_recentOrdersTable->setColumnWidth(4, 120);

    m_recentOrdersTable->setStyleSheet(R"(
        QTableWidget {
            background-color: white;
            border: 1px solid #ECECEC;
            border-radius: 18px;
            padding: 15px;
            font-size: 14px;
            color: #333333;
        }
        QTableWidget::item {
            padding: 14px 16px;
            border-bottom: 1px solid #F2F2F2;
        }
        QTableWidget::item:selected {
            background-color: #FFF3E8;
            color: #1F1F1F;
        }
        QHeaderView::section {
            background-color: white;
            color: #1F1F1F;
            font-weight: 700;
            font-size: 14px;
            padding: 14px 16px;
            border: none;
            border-bottom: 2px solid #F2F2F2;
        }
    )");

    m_recentOrdersTable->verticalHeader()->setDefaultSectionSize(52);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(m_recentOrdersTable);
    shadow->setBlurRadius(25);
    shadow->setColor(QColor(0, 0, 0, 20));
    shadow->setOffset(0, 6);
    m_recentOrdersTable->setGraphicsEffect(shadow);
}

void DashboardPage::refreshData()
{
    m_dataModel->refresh();
    populateStats();
    populateCharts();
    populateOrders();
}

void DashboardPage::populateStats()
{
    m_revenueCard->setValue(
        QString("$%L1").arg(m_dataModel->totalRevenue(), 0, 'f', 0));
    m_revenueCard->setSubtitle(
        QString("+%1% from last month").arg(m_dataModel->revenueGrowth(), 0, 'f', 1));

    m_ordersCard->setValue(
        QString("%L1").arg(m_dataModel->totalOrders()));
    m_ordersCard->setSubtitle(
        QString("+%1% from last month").arg(m_dataModel->ordersGrowth(), 0, 'f', 1));

    m_employeesCard->setValue(
        QString::number(m_dataModel->activeEmployees()));
    m_employeesCard->setSubtitle(
        QString("%1 working right now").arg(m_dataModel->workingNow()));

    m_topProductCard->setValue(m_dataModel->topSellingProduct());
    m_topProductCard->setSubtitle(
        QString("%1 orders this month").arg(m_dataModel->topSellingCount()));
}

void DashboardPage::populateCharts()
{
    m_lineChart->setData(m_dataModel->weeklySales());
    m_lineChart->setLabels({"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"});

    m_barChart->setData(m_dataModel->weeklyOrders());
    m_barChart->setLabels({"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"});
}

void DashboardPage::populateOrders()
{
    QList<OrderRecord> orders = m_dataModel->recentOrders();
    QLocale enLocale(QLocale::English, QLocale::UnitedStates);

    m_recentOrdersTable->setRowCount(orders.size());

    for (int i = 0; i < orders.size(); i++) {
        const OrderRecord &order = orders[i];

        // Order ID
        QTableWidgetItem *idItem = new QTableWidgetItem(order.orderId);
        idItem->setForeground(QColor("#555555"));
        idItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_recentOrdersTable->setItem(i, 0, idItem);

        // Employee
        QTableWidgetItem *empItem = new QTableWidgetItem(order.employeeName);
        empItem->setForeground(QColor("#333333"));
        empItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_recentOrdersTable->setItem(i, 1, empItem);

        // Date
        QTableWidgetItem *dateItem = new QTableWidgetItem(order.orderDate);
        dateItem->setForeground(QColor("#555555"));
        dateItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_recentOrdersTable->setItem(i, 2, dateItem);

        // Time
        QTableWidgetItem *timeItem = new QTableWidgetItem(order.orderTime);
        timeItem->setForeground(QColor("#555555"));
        timeItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_recentOrdersTable->setItem(i, 3, timeItem);

        // Total
        QTableWidgetItem *totalItem = new QTableWidgetItem(
            "$" + enLocale.toString(order.total, 'f', 2));
        totalItem->setForeground(QColor("#1A1A1A"));
        QFont boldFont;
        boldFont.setBold(true);
        totalItem->setFont(boldFont);
        totalItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_recentOrdersTable->setItem(i, 4, totalItem);

        // Status Badge
        QWidget *statusContainer = new QWidget();
        statusContainer->setStyleSheet("background: transparent;");

        QHBoxLayout *statusLayout = new QHBoxLayout(statusContainer);
        statusLayout->setContentsMargins(15, 0, 15, 0);
        statusLayout->setSpacing(0);
        statusLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QLabel *statusLabel = new QLabel(order.status);
        statusLabel->setAlignment(Qt::AlignCenter);

        QFont badgeFont("Segoe UI", 9);
        badgeFont.setBold(true);
        statusLabel->setFont(badgeFont);

        QFontMetrics fm(badgeFont);
        int textWidth = fm.horizontalAdvance(order.status) + 24;
        statusLabel->setFixedSize(textWidth, 26);

        if (order.status.toLower() == "completed") {
            statusLabel->setStyleSheet(R"(
                QLabel {
                    background-color: #D1F4DD;
                    color: #1B7F3F;
                    border-radius: 13px;
                    padding: 4px 12px;
                }
            )");
        } else if (order.status.toLower() == "pending") {
            statusLabel->setStyleSheet(R"(
                QLabel {
                    background-color: #FFF3C4;
                    color: #92590B;
                    border-radius: 13px;
                    padding: 4px 12px;
                }
            )");
        } else if (order.status.toLower() == "cancelled") {
            statusLabel->setStyleSheet(R"(
                QLabel {
                    background-color: #FFD6D6;
                    color: #B91C1C;
                    border-radius: 13px;
                    padding: 4px 12px;
                }
            )");
        } else {
            statusLabel->setStyleSheet(R"(
                QLabel {
                    background-color: #E0E0E0;
                    color: #555555;
                    border-radius: 13px;
                    padding: 4px 12px;
                }
            )");
        }

        statusLayout->addWidget(statusLabel, 0, Qt::AlignLeft | Qt::AlignVCenter);
        statusLayout->addStretch();

        m_recentOrdersTable->setCellWidget(i, 5, statusContainer);
    }
}

void DashboardPage::styleStatusLabel(QLabel *label, const QString &status)
{
    Q_UNUSED(label);
    Q_UNUSED(status);
}