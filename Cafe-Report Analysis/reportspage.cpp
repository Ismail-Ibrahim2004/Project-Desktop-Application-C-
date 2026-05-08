#include "reportspage.h"
#include "reportsservice.h"
#include <QStyle>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QCategoryAxis>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QSizePolicy>
#include <QGraphicsDropShadowEffect>
#include <QLayoutItem>

ReportsPage::ReportsPage(QWidget *parent)
    : QWidget(parent)
    , m_dailyRevenueValue(nullptr)
    , m_dailyRevenueChange(nullptr)
    , m_ordersTodayValue(nullptr)
    , m_ordersTodayChange(nullptr)
    , m_avgOrderValue(nullptr)
    , m_avgOrderChange(nullptr)
    , m_itemsSoldValue(nullptr)
    , m_itemsSoldChange(nullptr)
    , m_bestSellersLayout(nullptr)
{
    setupUI();
    applyStyles();
    refreshData();  // ✨ جلب البيانات من الداتابيز
}

void ReportsPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 18, 20, 18);
    mainLayout->setSpacing(15);

    setupHeader(mainLayout);
    setupStatsCards(mainLayout);
    setupTabs(mainLayout);
    setupContentArea(mainLayout);
}

// ==================== Header ====================
void ReportsPage::setupHeader(QVBoxLayout *mainLayout)
{
    QWidget *headerWidget = new QWidget();
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(3);

    QLabel *title = new QLabel("Reports & Analytics");
    title->setObjectName("pageTitle");

    QLabel *subtitle = new QLabel("Track your café's performance and insights");
    subtitle->setObjectName("pageSubtitle");

    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);

    mainLayout->addWidget(headerWidget);
}

// ==================== Stats Cards ====================
void ReportsPage::setupStatsCards(QVBoxLayout *mainLayout)
{
    QWidget *cardsWidget = new QWidget();
    QHBoxLayout *cardsLayout = new QHBoxLayout(cardsWidget);
    cardsLayout->setContentsMargins(0, 0, 0, 0);
    cardsLayout->setSpacing(12);

    QFrame *card1 = createStatCard("Daily Revenue", "$",
                                   &m_dailyRevenueValue, &m_dailyRevenueChange);
    QFrame *card2 = createStatCard("Orders Today", "🛒",
                                   &m_ordersTodayValue, &m_ordersTodayChange);
    QFrame *card3 = createStatCard("Avg Order Value", "📈",
                                   &m_avgOrderValue, &m_avgOrderChange);
    QFrame *card4 = createStatCard("Items Sold", "📦",
                                   &m_itemsSoldValue, &m_itemsSoldChange);

    cardsLayout->addWidget(card1);
    cardsLayout->addWidget(card2);
    cardsLayout->addWidget(card3);
    cardsLayout->addWidget(card4);

    mainLayout->addWidget(cardsWidget);
}

QFrame* ReportsPage::createStatCard(const QString &title,
                                    const QString &iconChar,
                                    QLabel **valueLabel,
                                    QLabel **changeLabel)
{
    QFrame *card = new QFrame();
    card->setObjectName("statCard");
    card->setFixedHeight(105);

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(15, 12, 15, 12);
    layout->setSpacing(5);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("cardTitle");

    QLabel *iconLabel = new QLabel(iconChar);
    iconLabel->setObjectName("cardIcon");
    iconLabel->setAlignment(Qt::AlignRight);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(iconLabel);

    *valueLabel = new QLabel("0");
    (*valueLabel)->setObjectName("cardValue");

    *changeLabel = new QLabel("0% from yesterday");
    (*changeLabel)->setObjectName("cardChangePositive");

    layout->addLayout(headerLayout);
    layout->addWidget(*valueLabel);
    layout->addStretch();
    layout->addWidget(*changeLabel);

    return card;
}

// ==================== Tabs ====================
void ReportsPage::setupTabs(QVBoxLayout *mainLayout)
{
    QWidget *tabsWidget = new QWidget();
    tabsWidget->setObjectName("tabsContainer");
    tabsWidget->setFixedHeight(42);
    tabsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QHBoxLayout *tabsLayout = new QHBoxLayout(tabsWidget);
    tabsLayout->setContentsMargins(4, 4, 4, 4);
    tabsLayout->setSpacing(0);

    m_tabButtonGroup = new QButtonGroup(this);
    m_tabButtonGroup->setExclusive(true);

    m_revenueTab     = createTabButton("Revenue Analysis");
    m_trendsTab      = createTabButton("Order Trends");
    m_bestSellersTab = createTabButton("Best Sellers");

    m_tabButtonGroup->addButton(m_revenueTab, 0);
    m_tabButtonGroup->addButton(m_trendsTab, 1);
    m_tabButtonGroup->addButton(m_bestSellersTab, 2);

    tabsLayout->addWidget(m_revenueTab);
    tabsLayout->addWidget(m_trendsTab);
    tabsLayout->addWidget(m_bestSellersTab);

    connect(m_tabButtonGroup, &QButtonGroup::idClicked,
            this, &ReportsPage::onTabClicked);

    m_revenueTab->setChecked(true);

    QHBoxLayout *wrapperLayout = new QHBoxLayout();
    wrapperLayout->setContentsMargins(0, 0, 0, 0);
    wrapperLayout->addWidget(tabsWidget);
    wrapperLayout->addStretch();

    QWidget *wrapper = new QWidget();
    wrapper->setLayout(wrapperLayout);

    mainLayout->addWidget(wrapper);
}

QPushButton* ReportsPage::createTabButton(const QString &text)
{
    QPushButton *btn = new QPushButton(text);
    btn->setObjectName("tabButton");
    btn->setCheckable(true);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedHeight(34);
    btn->setMinimumWidth(120);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(btn);
    shadow->setBlurRadius(12);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 2);
    shadow->setEnabled(false);
    btn->setGraphicsEffect(shadow);

    connect(btn, &QPushButton::toggled, btn, [shadow](bool checked) {
        shadow->setEnabled(checked);
    });

    return btn;
}

// ==================== Content Area ====================
void ReportsPage::setupContentArea(QVBoxLayout *mainLayout)
{
    m_contentStack = new QStackedWidget();
    m_contentStack->setObjectName("reportsContent");

    m_contentStack->addWidget(createRevenueAnalysisPage());
    m_contentStack->addWidget(createOrderTrendsPage());
    m_contentStack->addWidget(createBestSellersPage());

    mainLayout->addWidget(m_contentStack, 1);
}

void ReportsPage::onTabClicked(int index)
{
    m_contentStack->setCurrentIndex(index);
}

// ==================== Tab 1: Revenue Analysis ====================
QWidget* ReportsPage::createRevenueAnalysisPage()
{
    QWidget *page = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    QFrame *chart1 = createChartContainer("Daily Revenue", createDailyRevenueChart());
    QFrame *chart2 = createChartContainer("Sales by Category", createSalesByCategoryChart());

    layout->addWidget(chart1);
    layout->addWidget(chart2);

    return page;
}

// ==================== Tab 2: Order Trends ====================
QWidget* ReportsPage::createOrderTrendsPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);

    QFrame *chart = createChartContainer("Monthly Orders Trend", createMonthlyOrdersChart());
    layout->addWidget(chart);

    return page;
}

// ==================== Tab 3: Best Sellers ====================
QWidget* ReportsPage::createBestSellersPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *outerLayout = new QVBoxLayout(page);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    QFrame *container = new QFrame();
    container->setObjectName("chartContainer");

    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setContentsMargins(18, 15, 18, 15);
    layout->setSpacing(8);

    QLabel *title = new QLabel("Top 5 Best Selling Products");
    title->setObjectName("chartTitle");
    layout->addWidget(title);
    layout->addSpacing(3);

    // ✨ Layout الـ Best Sellers (هنملاه من الداتابيز)
    m_bestSellersLayout = new QVBoxLayout();
    m_bestSellersLayout->setSpacing(8);
    layout->addLayout(m_bestSellersLayout);

    layout->addStretch();

    outerLayout->addWidget(container);

    return page;
}

QFrame* ReportsPage::createBestSellerItem(int rank,
                                          const QString &productName,
                                          int orders,
                                          double revenue)
{
    QFrame *item = new QFrame();
    item->setObjectName("bestSellerItem");
    item->setFixedHeight(58);

    QHBoxLayout *layout = new QHBoxLayout(item);
    layout->setContentsMargins(12, 8, 15, 8);
    layout->setSpacing(12);

    QLabel *rankLabel = new QLabel(QString("#%1").arg(rank));
    rankLabel->setObjectName("rankBadge");
    rankLabel->setFixedSize(38, 38);
    rankLabel->setAlignment(Qt::AlignCenter);

    QWidget *infoWidget = new QWidget();
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(1);

    QLabel *nameLabel = new QLabel(productName);
    nameLabel->setObjectName("productName");

    QLabel *ordersLabel = new QLabel(QString("%1 orders").arg(orders));
    ordersLabel->setObjectName("productOrders");

    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(ordersLabel);

    QWidget *revenueWidget = new QWidget();
    QVBoxLayout *revenueLayout = new QVBoxLayout(revenueWidget);
    revenueLayout->setContentsMargins(0, 0, 0, 0);
    revenueLayout->setSpacing(1);

    QLabel *revenueLabel = new QLabel(QString("$%1").arg(revenue, 0, 'f', 2));
    revenueLabel->setObjectName("revenueValue");
    revenueLabel->setAlignment(Qt::AlignRight);

    QLabel *revenueText = new QLabel("Revenue");
    revenueText->setObjectName("revenueText");
    revenueText->setAlignment(Qt::AlignRight);

    revenueLayout->addWidget(revenueLabel);
    revenueLayout->addWidget(revenueText);

    layout->addWidget(rankLabel);
    layout->addWidget(infoWidget);
    layout->addStretch();
    layout->addWidget(revenueWidget);

    return item;
}

// ==================== Chart Container ====================
QFrame* ReportsPage::createChartContainer(const QString &title, QChartView *chartView)
{
    QFrame *container = new QFrame();
    container->setObjectName("chartContainer");

    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setContentsMargins(18, 15, 18, 15);
    layout->setSpacing(10);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("chartTitle");

    layout->addWidget(titleLabel);
    layout->addWidget(chartView, 1);

    return container;
}

// ==================== Charts (يجيبوا البيانات من الداتابيز) ====================
QChartView* ReportsPage::createDailyRevenueChart()
{
    // ✨ جلب البيانات من قاعدة البيانات
    auto data = ReportsService::instance().getLast7DaysRevenue();

    QSplineSeries *series = new QSplineSeries();
    series->setName("Revenue");

    double maxValue = 0;
    QStringList days;

    for (int i = 0; i < data.size(); ++i) {
        series->append(i, data[i].revenue);
        days << data[i].dayName;
        if (data[i].revenue > maxValue) maxValue = data[i].revenue;
    }

    QPen pen(QColor("#E67E22"));
    pen.setWidth(3);
    series->setPen(pen);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setBackgroundBrush(QBrush(Qt::white));
    chart->setMargins(QMargins(0, 0, 0, 0));

    QCategoryAxis *axisX = new QCategoryAxis();
    for (int i = 0; i < days.size(); ++i) {
        axisX->append(days[i], i);
    }
    if (days.size() > 0) axisX->setRange(0, days.size() - 1);
    axisX->setLabelsColor(QColor("#8B6F47"));
    axisX->setGridLineVisible(false);
    axisX->setLineVisible(false);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, maxValue * 1.2);
    axisY->setTickCount(5);
    axisY->setLabelFormat("%.0f");
    axisY->setLabelsColor(QColor("#8B6F47"));
    axisY->setGridLineColor(QColor("#F0E5D8"));
    axisY->setLineVisible(false);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background: transparent; border: none;");

    return chartView;
}

QChartView* ReportsPage::createSalesByCategoryChart()
{
    // ✨ جلب البيانات من قاعدة البيانات
    auto data = ReportsService::instance().getSalesByCategory();

    QPieSeries *series = new QPieSeries();
    series->setHoleSize(0.0);

    QList<QColor> colors = {
        QColor("#5D4037"),
        QColor("#E67E22"),
        QColor("#C8B59F"),
        QColor("#8B5A2B"),
        QColor("#A0826D")
    };

    for (int i = 0; i < data.size(); ++i) {
        QString label = QString("%1 %2%").arg(data[i].category).arg(data[i].percentage, 0, 'f', 0);
        QPieSlice *slice = series->append(label, data[i].percentage);
        slice->setBrush(colors[i % colors.size()]);
        slice->setLabelVisible(true);
        slice->setPen(QPen(Qt::white, 2));
    }

    QFont labelFont;
    labelFont.setPointSize(8);
    for (auto slice : series->slices()) {
        slice->setLabelFont(labelFont);
        slice->setLabelColor(QColor("#5D4037"));
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    chart->setBackgroundBrush(QBrush(Qt::white));
    chart->setMargins(QMargins(0, 0, 0, 0));

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background: transparent; border: none;");

    return chartView;
}

QChartView* ReportsPage::createMonthlyOrdersChart()
{
    // ✨ جلب البيانات من قاعدة البيانات
    auto data = ReportsService::instance().getLast6MonthsOrders();

    QBarSet *set = new QBarSet("Orders");
    set->setColor(QColor("#5D4037"));
    set->setBorderColor(Qt::transparent);

    QStringList months;
    int maxValue = 0;

    for (const auto &item : data) {
        *set << item.ordersCount;
        months << item.monthName;
        if (item.ordersCount > maxValue) maxValue = item.ordersCount;
    }

    QBarSeries *series = new QBarSeries();
    series->append(set);
    series->setBarWidth(0.6);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setBackgroundBrush(QBrush(Qt::white));
    chart->setMargins(QMargins(0, 0, 0, 0));
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelColor(QColor("#5D4037"));

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(months);
    axisX->setLabelsColor(QColor("#8B6F47"));
    axisX->setGridLineVisible(false);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, maxValue * 1.2);
    axisY->setTickCount(5);
    axisY->setLabelFormat("%d");
    axisY->setLabelsColor(QColor("#8B6F47"));
    axisY->setGridLineColor(QColor("#F0E5D8"));

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background: transparent; border: none;");

    return chartView;
}

// ==================== ✨ Refresh Data ====================
void ReportsPage::refreshData()
{
    // ===== تحديث الـ Stats Cards =====
    auto stats = ReportsService::instance().getDashboardStats();

    m_dailyRevenueValue->setText(QString("$%1").arg(stats.dailyRevenue, 0, 'f', 2));
    m_ordersTodayValue->setText(QString::number(stats.ordersToday));
    m_avgOrderValue->setText(QString("$%1").arg(stats.avgOrderValue, 0, 'f', 2));
    m_itemsSoldValue->setText(QString::number(stats.itemsSold));

    // تحديث نسب التغيير
    auto setChangeLabel = [](QLabel *label, double percent) {
        QString sign = (percent >= 0) ? "+" : "";
        label->setText(QString("%1%2% from yesterday").arg(sign).arg(percent, 0, 'f', 1));
        label->setObjectName(percent >= 0 ? "cardChangePositive" : "cardChangeNegative");
        label->style()->unpolish(label);
        label->style()->polish(label);
    };

    setChangeLabel(m_dailyRevenueChange, stats.revenueChangePercent);
    setChangeLabel(m_ordersTodayChange, stats.ordersChangePercent);
    setChangeLabel(m_avgOrderChange, stats.avgOrderChangePercent);
    setChangeLabel(m_itemsSoldChange, stats.itemsChangePercent);

    // ===== تحديث الـ Best Sellers =====
    if (m_bestSellersLayout) {
        // مسح القديم
        QLayoutItem *child;
        while ((child = m_bestSellersLayout->takeAt(0)) != nullptr) {
            if (child->widget()) {
                child->widget()->deleteLater();
            }
            delete child;
        }

        // إضافة الجديد
        auto sellers = ReportsService::instance().getTopBestSellers(5);
        for (int i = 0; i < sellers.size(); ++i) {
            m_bestSellersLayout->addWidget(
                createBestSellerItem(i + 1,
                                     sellers[i].productName,
                                     sellers[i].totalOrders,
                                     sellers[i].totalRevenue)
                );
        }
    }
}

// ==================== Styles ====================
void ReportsPage::applyStyles()
{
    QString styles = R"(
        #pageTitle {
            color: #5D4037;
            font-size: 22px;
            font-weight: bold;
        }

        #pageSubtitle {
            color: #8B6F47;
            font-size: 12px;
        }

        #statCard {
            background-color: white;
            border-radius: 10px;
            border: 1px solid #E8DDD0;
        }

        #cardTitle {
            color: #8B6F47;
            font-size: 11px;
            font-weight: 500;
        }
        #cardIcon {
            color: #E67E22;
            font-size: 16px;
            font-weight: bold;
        }

        #cardValue {
            color: #3E2723;
            font-size: 20px;
            font-weight: bold;
        }

        #cardChangePositive {
            color: #4CAF50;
            font-size: 10px;
            font-weight: 500;
        }

        #cardChangeNegative {
            color: #F44336;
            font-size: 10px;
            font-weight: 500;
        }

        #tabsContainer {
            background-color: #F0E5D8;
            border-radius: 21px;
        }

        #tabButton {
            background-color: transparent;
            color: #5D4037;
            border: none;
            border-radius: 5px;
            font-size: 12px;
            font-weight: 500;
            padding: 0 16px;
        }

        #tabButton:hover {
            background-color: rgba(139, 90, 43, 0.1);
            border-radius: 5px;
        }

        #tabButton:checked {
            background-color: #5D4037;
            color: white;
            font-weight: bold;
            border-radius: 5px;
        }

        #reportsContent {
            background-color: transparent;
            border: none;
        }

        #chartContainer {
            background-color: white;
            border-radius: 10px;
            border: 1px solid #E8DDD0;
        }

        #chartTitle {
            color: #3E2723;
            font-size: 14px;
            font-weight: bold;
        }

        #bestSellerItem {
            background-color: #FAF6F1;
            border-radius: 8px;
            border: 1px solid #F0E5D8;
        }

        #rankBadge {
            background-color: #E67E22;
            color: white;
            border-radius: 8px;
            font-size: 12px;
            font-weight: bold;
        }

        #productName {
            color: #3E2723;
            font-size: 13px;
            font-weight: bold;
        }

        #productOrders {
            color: #8B6F47;
            font-size: 11px;
        }

        #revenueValue {
            color: #E67E22;
            font-size: 14px;
            font-weight: bold;
        }

        #revenueText {
            color: #8B6F47;
            font-size: 10px;
        }
    )";

    setStyleSheet(styles);
}