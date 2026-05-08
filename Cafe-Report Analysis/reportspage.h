#ifndef REPORTSPAGE_H
#define REPORTSPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QScrollArea>
#include <QtCharts/QChartView>

class ReportsPage : public QWidget
{
    Q_OBJECT
public:
    explicit ReportsPage(QWidget *parent = nullptr);

    // ✨ تحديث البيانات من الداتابيز
    void refreshData();

private slots:
    void onTabClicked(int index);

private:
    // الأقسام الرئيسية
    void setupUI();
    void setupHeader(QVBoxLayout *mainLayout);
    void setupStatsCards(QVBoxLayout *mainLayout);
    void setupTabs(QVBoxLayout *mainLayout);
    void setupContentArea(QVBoxLayout *mainLayout);

    // الـ Tabs الـ 3
    QWidget* createRevenueAnalysisPage();
    QWidget* createOrderTrendsPage();
    QWidget* createBestSellersPage();

    // عناصر مساعدة
    QFrame* createStatCard(const QString &title,
                           const QString &iconChar,
                           QLabel **valueLabel,
                           QLabel **changeLabel);

    QPushButton* createTabButton(const QString &text);

    QFrame* createBestSellerItem(int rank,
                                 const QString &productName,
                                 int orders,
                                 double revenue);

    QFrame* createChartContainer(const QString &title, QChartView *chartView);

    // الرسوم البيانية
    QChartView* createDailyRevenueChart();
    QChartView* createSalesByCategoryChart();
    QChartView* createMonthlyOrdersChart();

    // Stylesheet
    void applyStyles();

    // المتغيرات
    QButtonGroup *m_tabButtonGroup;
    QPushButton *m_revenueTab;
    QPushButton *m_trendsTab;
    QPushButton *m_bestSellersTab;

    QStackedWidget *m_contentStack;

    // ✨ Labels للـ Stats Cards (عشان نعدلها لما البيانات تتحدث)
    QLabel *m_dailyRevenueValue;
    QLabel *m_dailyRevenueChange;
    QLabel *m_ordersTodayValue;
    QLabel *m_ordersTodayChange;
    QLabel *m_avgOrderValue;
    QLabel *m_avgOrderChange;
    QLabel *m_itemsSoldValue;
    QLabel *m_itemsSoldChange;

    // ✨ Container للـ Best Sellers (عشان نمسحها ونعيد ملاها)
    QVBoxLayout *m_bestSellersLayout;
};

#endif // REPORTSPAGE_H