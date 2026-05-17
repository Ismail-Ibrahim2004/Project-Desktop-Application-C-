#ifndef REPORTSPAGE_H
#define REPORTSPAGE_H

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

class StatCard;
class ChartWidget;
class BarChartWidget;
class PieChartWidget;

class ReportsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsPage(QWidget *parent = nullptr);
    ~ReportsPage();

    void refreshData();

private slots:
    void switchTab(int index);

private:
    void setupUI();
    void setupStatsCards(QVBoxLayout *parentLayout);
    void setupTabs(QVBoxLayout *parentLayout);
    QWidget* createRevenueAnalysisTab();
    QWidget* createOrderTrendsTab();
    QWidget* createBestSellersTab();
    void loadStatsFromDatabase();
    void loadRevenueData();
    void loadOrdersTrendData();
    void loadBestSellersData();
    QFrame* createBestSellerItem(int rank, const QString &name, int orders, double revenue);

    // Stat Cards
    StatCard *m_dailyRevenueCard;
    StatCard *m_ordersTodayCard;
    StatCard *m_avgOrderCard;
    StatCard *m_itemsSoldCard;

    // Tabs
    QStackedWidget *m_tabStack;
    QPushButton *m_tabRevenue;
    QPushButton *m_tabOrders;
    QPushButton *m_tabBestSellers;

    // Charts
    ChartWidget *m_dailyRevenueChart;
    PieChartWidget *m_categoryPieChart;
    BarChartWidget *m_monthlyOrdersChart;

    // Best Sellers Container
    QVBoxLayout *m_bestSellersLayout;
};

#endif // REPORTSPAGE_H