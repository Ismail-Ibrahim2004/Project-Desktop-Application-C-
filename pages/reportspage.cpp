#include "reportspage.h"
#include "../database/databasemanager.h"
#include "../widgets/statcard.h"
#include "../widgets/chartwidget.h"
#include "../widgets/barchartwidget.h"
#include "../widgets/piechartwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QGraphicsDropShadowEffect>
#include <QScrollArea>

ReportsPage::ReportsPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    refreshData();
}

ReportsPage::~ReportsPage() {}

void ReportsPage::refreshData()
{
    loadStatsFromDatabase();
    loadRevenueData();
    loadOrdersTrendData();
    loadBestSellersData();
}

// =====================================================
// Setup UI
// =====================================================

void ReportsPage::setupUI()
{
    setStyleSheet("background: #FAF6F1;");

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: #FAF6F1; border: none; }");

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: #FAF6F1;");

    QVBoxLayout *mainLayout = new QVBoxLayout(scrollContent);
    mainLayout->setContentsMargins(32, 24, 32, 24);
    mainLayout->setSpacing(20);

    // ===== Header =====
    QLabel *headerTitle = new QLabel("Reports & Analytics");
    headerTitle->setStyleSheet("color: #1A1A1A; font-size: 28px; font-weight: bold; background: transparent;");

    QLabel *headerSub = new QLabel("Track your café's performance and insights");
    headerSub->setStyleSheet("color: #888; font-size: 14px; background: transparent;");

    mainLayout->addWidget(headerTitle);
    mainLayout->addWidget(headerSub);
    mainLayout->addSpacing(5);

    setupStatsCards(mainLayout);
    setupTabs(mainLayout);

    mainLayout->addStretch();

    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    scrollArea->setWidget(scrollContent);
    outer->addWidget(scrollArea);
}

// =====================================================
// Stats Cards
// =====================================================

void ReportsPage::setupStatsCards(QVBoxLayout *parentLayout)
{
    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(20);

    m_dailyRevenueCard  = new StatCard("Daily Revenue",    "$5,900",  "+15.2% from yesterday", ":/icons/doller.png",           QColor("#4CAF50"));
    m_ordersTodayCard   = new StatCard("Orders Today",     "127",     "+8.5% from yesterday",  ":/icons/checkout.png",         QColor("#4CAF50"));
    m_avgOrderCard      = new StatCard("Avg Order Value",  "$46.46",  "+3.1% from yesterday",  ":/icons/bar-chart.png",        QColor("#4CAF50"));
    m_itemsSoldCard     = new StatCard("Items Sold",       "342",     "+12.3% from yesterday", ":/icons/product.png",          QColor("#4CAF50"));

    cardsLayout->addWidget(m_dailyRevenueCard);
    cardsLayout->addWidget(m_ordersTodayCard);
    cardsLayout->addWidget(m_avgOrderCard);
    cardsLayout->addWidget(m_itemsSoldCard);

    parentLayout->addLayout(cardsLayout);
}

// =====================================================
// Tabs
// =====================================================

void ReportsPage::setupTabs(QVBoxLayout *parentLayout)
{
    QFrame *tabsBar = new QFrame();
    tabsBar->setFixedHeight(50);
    tabsBar->setStyleSheet("QFrame { background: #F5EFE6; border-radius: 25px; }");

    QHBoxLayout *tabsLayout = new QHBoxLayout(tabsBar);
    tabsLayout->setContentsMargins(6, 6, 6, 6);
    tabsLayout->setSpacing(0);

    QString activeStyle = R"(
        QPushButton {
            background-color: #5D3A1A;
            color: white;
            border: none;
            border-radius: 20px;
            padding: 8px 24px;
            font-size: 13px;
            font-weight: 600;
        }
    )";

    QString inactiveStyle = R"(
        QPushButton {
            background: transparent;
            color: #5D3A1A;
            border: none;
            border-radius: 20px;
            padding: 8px 24px;
            font-size: 13px;
            font-weight: 500;
        }
        QPushButton:hover { background: rgba(93, 58, 26, 0.08); }
    )";

    m_tabRevenue     = new QPushButton("Revenue Analysis");
    m_tabOrders      = new QPushButton("Order Trends");
    m_tabBestSellers = new QPushButton("Best Sellers");

    m_tabRevenue->setCursor(Qt::PointingHandCursor);
    m_tabOrders->setCursor(Qt::PointingHandCursor);
    m_tabBestSellers->setCursor(Qt::PointingHandCursor);

    m_tabRevenue->setStyleSheet(activeStyle);
    m_tabOrders->setStyleSheet(inactiveStyle);
    m_tabBestSellers->setStyleSheet(inactiveStyle);

    tabsLayout->addWidget(m_tabRevenue);
    tabsLayout->addWidget(m_tabOrders);
    tabsLayout->addWidget(m_tabBestSellers);
    tabsLayout->addStretch();

    QHBoxLayout *tabsBarLayout = new QHBoxLayout();
    tabsBarLayout->addWidget(tabsBar);
    tabsBarLayout->addStretch();
    parentLayout->addLayout(tabsBarLayout);

    m_tabStack = new QStackedWidget();
    m_tabStack->addWidget(createRevenueAnalysisTab());
    m_tabStack->addWidget(createOrderTrendsTab());
    m_tabStack->addWidget(createBestSellersTab());
    parentLayout->addWidget(m_tabStack);

    connect(m_tabRevenue, &QPushButton::clicked, this, [this, activeStyle, inactiveStyle]() {
        m_tabRevenue->setStyleSheet(activeStyle);
        m_tabOrders->setStyleSheet(inactiveStyle);
        m_tabBestSellers->setStyleSheet(inactiveStyle);
        m_tabStack->setCurrentIndex(0);
    });

    connect(m_tabOrders, &QPushButton::clicked, this, [this, activeStyle, inactiveStyle]() {
        m_tabRevenue->setStyleSheet(inactiveStyle);
        m_tabOrders->setStyleSheet(activeStyle);
        m_tabBestSellers->setStyleSheet(inactiveStyle);
        m_tabStack->setCurrentIndex(1);
    });

    connect(m_tabBestSellers, &QPushButton::clicked, this, [this, activeStyle, inactiveStyle]() {
        m_tabRevenue->setStyleSheet(inactiveStyle);
        m_tabOrders->setStyleSheet(inactiveStyle);
        m_tabBestSellers->setStyleSheet(activeStyle);
        m_tabStack->setCurrentIndex(2);
    });
}

// =====================================================
// Tab 1: Revenue Analysis
// =====================================================

QWidget* ReportsPage::createRevenueAnalysisTab()
{
    QWidget *tab = new QWidget();
    tab->setStyleSheet("background: transparent;");

    QHBoxLayout *layout = new QHBoxLayout(tab);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20);

    m_dailyRevenueChart = new ChartWidget("Daily Revenue");
    m_dailyRevenueChart->setMinimumSize(450, 380);

    m_categoryPieChart = new PieChartWidget("Sales by Category");
    m_categoryPieChart->setMinimumSize(450, 380);

    layout->addWidget(m_dailyRevenueChart, 1);
    layout->addWidget(m_categoryPieChart, 1);

    return tab;
}

// =====================================================
// Tab 2: Order Trends
// =====================================================

QWidget* ReportsPage::createOrderTrendsTab()
{
    QWidget *tab = new QWidget();
    tab->setStyleSheet("background: transparent;");

    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(0, 0, 0, 0);

    m_monthlyOrdersChart = new BarChartWidget("Monthly Orders Trend");
    m_monthlyOrdersChart->setMinimumHeight(450);

    layout->addWidget(m_monthlyOrdersChart);

    return tab;
}

// =====================================================
// Tab 3: Best Sellers
// =====================================================

QWidget* ReportsPage::createBestSellersTab()
{
    QWidget *tab = new QWidget();
    tab->setStyleSheet("background: transparent;");

    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(0, 0, 0, 0);

    QFrame *card = new QFrame();
    card->setStyleSheet(R"(
        QFrame {
            background: white;
            border: 1px solid #EFE7DE;
            border-radius: 16px;
        }
    )");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 4);
    card->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(25, 22, 25, 22);
    cardLayout->setSpacing(12);

    QLabel *cardTitle = new QLabel("Top 5 Best Selling Products");
    cardTitle->setStyleSheet("color: #1A1A1A; font-size: 16px; font-weight: bold; background: transparent; border: none;");
    cardLayout->addWidget(cardTitle);
    cardLayout->addSpacing(5);

    m_bestSellersLayout = new QVBoxLayout();
    m_bestSellersLayout->setSpacing(12);
    cardLayout->addLayout(m_bestSellersLayout);

    layout->addWidget(card);
    layout->addStretch();

    return tab;
}

// =====================================================
// Best Seller Item Card
// =====================================================

QFrame* ReportsPage::createBestSellerItem(int rank, const QString &name, int orders, double revenue)
{
    QFrame *item = new QFrame();
    item->setFixedHeight(70);
    item->setStyleSheet(R"(
        QFrame {
            background: white;
            border: 1px solid #EFE7DE;
            border-radius: 12px;
        }
    )");

    QHBoxLayout *itemLayout = new QHBoxLayout(item);
    itemLayout->setContentsMargins(15, 10, 20, 10);
    itemLayout->setSpacing(15);

    // Rank Badge
    QLabel *rankBadge = new QLabel(QString("#%1").arg(rank));
    rankBadge->setFixedSize(45, 45);
    rankBadge->setAlignment(Qt::AlignCenter);
    rankBadge->setStyleSheet(R"(
        QLabel {
            background: #E67E22;
            color: white;
            border-radius: 10px;
            font-size: 14px;
            font-weight: bold;
            border: none;
        }
    )");

    // Product Icon
    QLabel *productIcon = new QLabel();
    productIcon->setFixedSize(32, 32);
    productIcon->setPixmap(
        QPixmap(":/icons/product.png")
            .scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    productIcon->setStyleSheet("background: transparent; border: none;");

    // Product Info
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(2);

    QLabel *nameLabel = new QLabel(name);
    nameLabel->setStyleSheet("color: #1A1A1A; font-size: 14px; font-weight: 600; background: transparent; border: none;");

    QLabel *ordersLabel = new QLabel(QString("%1 orders").arg(orders));
    ordersLabel->setStyleSheet("color: #888; font-size: 12px; background: transparent; border: none;");

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(ordersLabel);

    // Revenue
    QVBoxLayout *revenueLayout = new QVBoxLayout();
    revenueLayout->setSpacing(2);
    revenueLayout->setAlignment(Qt::AlignRight);

    QLabel *revenueAmount = new QLabel(QString("$%L1").arg(revenue, 0, 'f', 2));
    revenueAmount->setAlignment(Qt::AlignRight);
    revenueAmount->setStyleSheet("color: #E67E22; font-size: 16px; font-weight: bold; background: transparent; border: none;");

    QLabel *revenueLabel = new QLabel("Revenue");
    revenueLabel->setAlignment(Qt::AlignRight);
    revenueLabel->setStyleSheet("color: #888; font-size: 11px; background: transparent; border: none;");

    revenueLayout->addWidget(revenueAmount);
    revenueLayout->addWidget(revenueLabel);

    itemLayout->addWidget(rankBadge);
    itemLayout->addWidget(productIcon);
    itemLayout->addLayout(infoLayout);
    itemLayout->addStretch();
    itemLayout->addLayout(revenueLayout);

    return item;
}

// =====================================================
// Load Stats from Database
// =====================================================

void ReportsPage::loadStatsFromDatabase()
{
    if (!DatabaseManager::instance().isConnected()) return;

    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery q(db);

    if (q.exec("SELECT ISNULL(SUM(Total), 0) FROM Orders WHERE OrderDate = CAST(GETDATE() AS DATE)") && q.next()) {
        double revenue = q.value(0).toDouble();
        m_dailyRevenueCard->setValue(QString("$%L1").arg(revenue, 0, 'f', 0));
    }

    if (q.exec("SELECT COUNT(*) FROM Orders WHERE OrderDate = CAST(GETDATE() AS DATE)") && q.next()) {
        int count = q.value(0).toInt();
        m_ordersTodayCard->setValue(QString::number(count));
    }

    if (q.exec("SELECT ISNULL(AVG(Total), 0) FROM Orders WHERE OrderDate = CAST(GETDATE() AS DATE)") && q.next()) {
        double avg = q.value(0).toDouble();
        m_avgOrderCard->setValue(QString("$%1").arg(avg, 0, 'f', 2));
    }

    if (q.exec(R"(
        SELECT ISNULL(SUM(od.Quantity), 0)
        FROM OrderDetails od
        INNER JOIN Orders o ON od.id = o.id
        WHERE o.OrderDate = CAST(GETDATE() AS DATE)
    )") && q.next()) {
        int items = q.value(0).toInt();
        m_itemsSoldCard->setValue(QString::number(items));
    }
}

// =====================================================
// Load Revenue Data
// =====================================================

void ReportsPage::loadRevenueData()
{
    QList<double> dailyData = {4200, 3800, 5200, 4800, 6500, 8000, 6200};

    if (DatabaseManager::instance().isConnected()) {
        QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
        QList<double> realData;

        for (int i = 6; i >= 0; i--) {
            QSqlQuery q(db);
            q.prepare(R"(
                SELECT ISNULL(SUM(Total), 0)
                FROM Orders
                WHERE OrderDate = CAST(DATEADD(DAY, -?, GETDATE()) AS DATE)
            )");
            q.addBindValue(i);
            if (q.exec() && q.next())
                realData.append(q.value(0).toDouble());
            else
                realData.append(0);
        }

        bool allZero = true;
        for (double v : realData) if (v > 0) { allZero = false; break; }
        if (!allZero) dailyData = realData;
    }

    m_dailyRevenueChart->setData(dailyData);
    m_dailyRevenueChart->setLabels({"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"});

    QList<QPair<QString, double>> pieData;

    if (DatabaseManager::instance().isConnected()) {
        QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
        QSqlQuery q(db);

        if (q.exec(R"(
            SELECT p.Category, SUM(od.Quantity * od.price) AS CategorySales
            FROM OrderDetails od
            INNER JOIN Products p ON od.id = p.id
            INNER JOIN Orders o ON od.id = o.id
            WHERE MONTH(o.OrderDate) = MONTH(GETDATE())
              AND YEAR(o.OrderDate) = YEAR(GETDATE())
            GROUP BY p.Category
        )")) {
            while (q.next()) {
                QString cat = q.value(0).toString();
                double sales = q.value(1).toDouble();
                if (!cat.isEmpty() && sales > 0)
                    pieData.append({cat, sales});
            }
        }
    }

    if (pieData.isEmpty()) {
        pieData = {
            {"Coffee",      45},
            {"Tea",         20},
            {"Desserts",    25},
            {"Cold Drinks", 10}
        };
    }

    m_categoryPieChart->setData(pieData);
}

// =====================================================
// Load Orders Trend Data
// =====================================================

void ReportsPage::loadOrdersTrendData()
{
    QList<int> monthlyData = {850, 950, 1100, 1050, 1250, 1350};

    if (DatabaseManager::instance().isConnected()) {
        QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
        QList<int> realData;

        for (int i = 5; i >= 0; i--) {
            QSqlQuery q(db);
            q.prepare(R"(
                SELECT COUNT(*)
                FROM Orders
                WHERE MONTH(OrderDate) = MONTH(DATEADD(MONTH, -?, GETDATE()))
                  AND YEAR(OrderDate) = YEAR(DATEADD(MONTH, -?, GETDATE()))
            )");
            q.addBindValue(i);
            q.addBindValue(i);

            if (q.exec() && q.next())
                realData.append(q.value(0).toInt());
            else
                realData.append(0);
        }

        bool allZero = true;
        for (int v : realData) if (v > 0) { allZero = false; break; }
        if (!allZero) monthlyData = realData;
    }

    m_monthlyOrdersChart->setData(monthlyData);
    m_monthlyOrdersChart->setLabels({"Jan", "Feb", "Mar", "Apr", "May", "Jun"});
}

// =====================================================
// Load Best Sellers Data
// =====================================================

void ReportsPage::loadBestSellersData()
{
    QLayoutItem *item;
    while ((item = m_bestSellersLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    QList<QVariantList> bestSellers;

    if (DatabaseManager::instance().isConnected()) {
        QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
        QSqlQuery q(db);

        if (q.exec(R"(
            SELECT TOP 5
                p.name,
                SUM(od.Quantity) AS TotalOrders,
                SUM(od.Quantity * od.price) AS TotalRevenue
            FROM OrderDetails od
            INNER JOIN Products p ON od.id = p.id
            GROUP BY p.name
            ORDER BY TotalOrders DESC
        )")) {
            while (q.next()) {
                QVariantList row;
                row << q.value(0).toString()
                    << q.value(1).toInt()
                    << q.value(2).toDouble();
                bestSellers.append(row);
            }
        }
    }

    if (bestSellers.isEmpty()) {
        bestSellers = {
            {QString("Cappuccino"),     342, 1453.50},
            {QString("Latte"),          298, 1341.00},
            {QString("Espresso"),       256, 896.00},
            {QString("Americano"),      223, 836.25},
            {QString("Chocolate Cake"), 187, 1028.50}
        };
    }

    int rank = 1;
    for (const QVariantList &row : bestSellers) {
        QFrame *itemCard = createBestSellerItem(
            rank,
            row[0].toString(),
            row[1].toInt(),
            row[2].toDouble()
            );
        m_bestSellersLayout->addWidget(itemCard);
        rank++;
    }
}

void ReportsPage::switchTab(int index)
{
    m_tabStack->setCurrentIndex(index);
}