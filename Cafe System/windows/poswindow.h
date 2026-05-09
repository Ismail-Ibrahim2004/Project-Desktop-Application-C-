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

// =========================
// 📦 Product Structure
// =========================
struct Product
{
    QString name;
    double price;
    QString category;
};

// =========================
// 🪟 POSWindow Class
// =========================
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

private:
    void setupUI();
    void setupDatabase();
    void loadProducts(QString category = "Coffee");
    QFrame* createProductCard(Product p);
    void updateCartUI();

    // 👤 User Data
    QString m_username;
    QLabel *m_userLabel = nullptr;

    // 🎨 UI Components
    QGridLayout *productsGrid = nullptr;
    QVBoxLayout *cartLayout = nullptr;
    QLabel *subtotalLabel = nullptr;
    QLabel *taxLabel = nullptr;
    QLabel *totalLabel = nullptr;
    QLabel *itemCountBadge = nullptr;
    QPushButton *completeBtn = nullptr;

    // 🛒 Cart Data
    QMap<QString, int> cartItems;
    QMap<QString, double> itemPrices;
};

#endif // POSWINDOW_H