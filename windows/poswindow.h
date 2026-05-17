#ifndef POSWINDOW_H
#define POSWINDOW_H
#include <QMainWindow>
#include <QFrame>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMap>
#include <QString>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QTableWidget>

// =====================================================
//  Product Structure
// =====================================================
struct Product
{
    QString name;
    double price;
    QString category;
};

// =====================================================
//  POSWindow Class
// =====================================================
class POSWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit POSWindow(QWidget *parent = nullptr);
    ~POSWindow();

    void setUserInfo(const QString &username);

private slots:
    void filterProducts(QString category);
    void addToCart(QString name, double price);
    void removeFromCart(QString name);
    void changeQuantity(QString name, int delta);
    void calculateTotals();
    void completeOrder();
    void onLogout();


    void showPOSView();
    void printReceipt();

private:
    void setupUI();
    void setupPOSPage();
    void setupReceiptPage();
    void setupDatabase();
    void loadProducts(QString category = "Coffee");
    QFrame* createProductCard(Product p);
    void updateCartUI();
    QString generateOrderNumber();

    QString m_username;
    QLabel *m_userLabel = nullptr;

    QStackedWidget *mainStack = nullptr;
    QWidget *posPageWidget = nullptr;
    QWidget *receiptPageWidget = nullptr;

    QGridLayout *productsGrid = nullptr;
    QVBoxLayout *cartLayout = nullptr;
    QLabel *subtotalLabel = nullptr;
    QLabel *taxLabel = nullptr;
    QLabel *totalLabel = nullptr;
    QLabel *itemCountBadge = nullptr;
    QPushButton *completeBtn = nullptr;

    //  UI Components (Receipt)
    QLabel *receiptOrderNum = nullptr;
    QLabel *receiptDateTime = nullptr;
    QLabel *receiptServedBy = nullptr;
    QTableWidget *receiptTable = nullptr;
    QLabel *receiptSubtotal = nullptr;
    QLabel *receiptTax = nullptr;
    QLabel *receiptTotal = nullptr;

    //  Cart Data
    QMap<QString, int> cartItems;
    QMap<QString, double> itemPrices;

    double currentOrderTotal = 0.0;
    QString currentOrderNum;
};

#endif // POSWINDOW_H