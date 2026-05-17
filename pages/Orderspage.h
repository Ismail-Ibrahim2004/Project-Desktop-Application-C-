#ifndef ORDERSPAGE_H
#define ORDERSPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

class OrdersPage : public QWidget
{
    Q_OBJECT
public:
    explicit OrdersPage(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onSearchChanged(const QString &text);
    void onStatusFilterChanged(int index);
    void onViewDetails(int orderId);
    void onBackToList();
    void onPrintReceipt();

private:
    void setupUI();
    void setupListPage();
    void setupDetailsPage();
    void setupOrdersTable();
    void loadOrders();
    void loadOrderDetails(int orderId);

    // Stack
    QStackedWidget *m_stack;
    QWidget        *m_listPage;
    QWidget        *m_detailsPage;

    // List Page
    QLineEdit      *m_searchEdit;
    QComboBox      *m_statusFilter;
    QTableWidget   *m_ordersTable;

    // Details Page
    QLabel         *m_detailOrderId;
    QLabel         *m_detailEmployee;
    QLabel         *m_detailDateTime;
    QLabel         *m_detailStatus;
    QTableWidget   *m_itemsTable;
    QLabel         *m_subtotalLabel;
    QLabel         *m_taxLabel;
    QLabel         *m_totalLabel;

    // Current order
    int m_currentOrderId = -1;
};

#endif // ORDERSPAGE_H