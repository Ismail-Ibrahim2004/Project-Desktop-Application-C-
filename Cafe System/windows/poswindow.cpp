#include "poswindow.h"
#include "../database/databasemanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QFrame>
#include <QScrollArea>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QGraphicsDropShadowEffect>
#include <QDateTime>
#include <QLabel>

// =====================================================
// Constructor & Destructor
// =====================================================

POSWindow::POSWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupDatabase();
    loadProducts("Coffee");

    setWindowTitle("Cafe Manager - POS");
    resize(1400, 820);
    setMinimumSize(1200, 720);
}

POSWindow::~POSWindow()
{
}

// =====================================================
// User Info
// =====================================================

void POSWindow::setUserInfo(const QString &username)
{
    m_username = username;

    if (m_userLabel) {
        m_userLabel->setText("  👤  " + username);
    }

    setWindowTitle(QString("Cafe Manager - POS (%1)").arg(username));
}

// =====================================================
// Setup UI
// =====================================================

void POSWindow::setupUI()
{
    setStyleSheet("background-color: #FDFBF9;");

    QWidget *centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // =====================================================
    // 🟤 SIDEBAR
    // =====================================================

    QFrame *sidebar = new QFrame();
    sidebar->setFixedWidth(240);
    sidebar->setStyleSheet("QFrame { background-color: #634226; }");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(20, 30, 20, 20);
    sidebarLayout->setSpacing(15);

    // Logo
    QLabel *logo = new QLabel("☕ Cafe Manager");
    logo->setStyleSheet(R"(
        color: white;
        font-size: 22px;
        font-weight: bold;
        background: transparent;
    )");

    QLabel *subLogo = new QLabel("POS SYSTEM");
    subLogo->setStyleSheet(R"(
        color: #D8C3AF;
        font-size: 13px;
        letter-spacing: 2px;
        background: transparent;
    )");

    sidebarLayout->addWidget(logo);
    sidebarLayout->addWidget(subLogo);
    sidebarLayout->addSpacing(40);

    // POS Button (Active)
    QPushButton *posBtn = new QPushButton("  🛒  POS System");
    posBtn->setFixedHeight(48);
    posBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #E68A4F;
            color: white;
            border: none;
            border-radius: 12px;
            font-size: 14px;
            font-weight: bold;
            text-align: left;
            padding-left: 18px;
        }
        QPushButton:hover {
            background-color: #D97838;
        }
    )");

    sidebarLayout->addWidget(posBtn);
    sidebarLayout->addStretch();

    // User Label
    m_userLabel = new QLabel("  👤  Employee");
    m_userLabel->setStyleSheet(R"(
        color: white;
        font-size: 14px;
        background: transparent;
    )");

    // Logout Button
    QPushButton *logoutBtn = new QPushButton("  ⮕  Logout");
    logoutBtn->setFixedHeight(45);
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #4D331D;
            color: white;
            border: none;
            border-radius: 10px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #3E2816;
        }
    )");

    connect(logoutBtn, &QPushButton::clicked, this, &POSWindow::onLogout);

    sidebarLayout->addWidget(m_userLabel);
    sidebarLayout->addSpacing(10);
    sidebarLayout->addWidget(logoutBtn);

    mainLayout->addWidget(sidebar);

    // =====================================================
    // 📦 CENTER SECTION - Products
    // =====================================================

    QWidget *centerWidget = new QWidget();
    QVBoxLayout *centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(30, 30, 30, 30);
    centerLayout->setSpacing(20);

    // Title
    QLabel *title = new QLabel("Point Of Sale");
    title->setStyleSheet(R"(
        font-size: 30px;
        font-weight: bold;
        color: #2B2B2B;
        background: transparent;
    )");

    QLabel *subtitle = new QLabel("Select products and manage customer orders");
    subtitle->setStyleSheet(R"(
        color: #888888;
        font-size: 14px;
        background: transparent;
    )");

    centerLayout->addWidget(title);
    centerLayout->addWidget(subtitle);

    // =====================================================
    // 🏷️ Categories
    // =====================================================

    QHBoxLayout *categoriesLayout = new QHBoxLayout();
    categoriesLayout->setSpacing(10);

    QStringList categories = {"Coffee", "Tea", "Beverages", "Pastry", "Dessert"};

    for (const QString &cat : categories) {
        QPushButton *btn = new QPushButton(cat);
        btn->setFixedHeight(40);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(R"(
            QPushButton {
                background: white;
                border: 1px solid #E8E2DA;
                border-radius: 20px;
                padding: 0 22px;
                color: #634226;
                font-size: 13px;
                font-weight: 600;
            }
            QPushButton:hover {
                background: #634226;
                color: white;
            }
        )");

        connect(btn, &QPushButton::clicked, this, [this, cat]() {
            filterProducts(cat);
        });

        categoriesLayout->addWidget(btn);
    }

    categoriesLayout->addStretch();
    centerLayout->addLayout(categoriesLayout);

    // =====================================================
    // 🛍️ Products Grid
    // =====================================================

    QScrollArea *productsScroll = new QScrollArea();
    productsScroll->setWidgetResizable(true);
    productsScroll->setFrameShape(QFrame::NoFrame);
    productsScroll->setStyleSheet("QScrollArea { background: transparent; border: none; }");

    QWidget *productsContainer = new QWidget();
    productsContainer->setStyleSheet("background: transparent;");

    productsGrid = new QGridLayout(productsContainer);
    productsGrid->setSpacing(20);
    productsGrid->setContentsMargins(5, 5, 5, 5);

    productsScroll->setWidget(productsContainer);
    centerLayout->addWidget(productsScroll);

    mainLayout->addWidget(centerWidget, 2);

    // =====================================================
    // 🛒 CART SECTION
    // =====================================================

    QFrame *cartCard = new QFrame();
    cartCard->setFixedWidth(390);
    cartCard->setStyleSheet("QFrame { background: white; border-left: 1px solid #EFE7DE; }");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(25);
    shadow->setOffset(0, 5);
    shadow->setColor(QColor(0, 0, 0, 25));
    cartCard->setGraphicsEffect(shadow);

    QVBoxLayout *cartLayoutMain = new QVBoxLayout(cartCard);
    cartLayoutMain->setContentsMargins(25, 25, 25, 25);
    cartLayoutMain->setSpacing(18);

    // Cart Header
    QHBoxLayout *cartHeader = new QHBoxLayout();

    QLabel *cartTitle = new QLabel("Current Order");
    cartTitle->setStyleSheet(R"(
        font-size: 22px;
        font-weight: bold;
        color: #222;
        background: transparent;
    )");

    itemCountBadge = new QLabel("0");
    itemCountBadge->setFixedSize(30, 30);
    itemCountBadge->setAlignment(Qt::AlignCenter);
    itemCountBadge->setStyleSheet(R"(
        background: #E68A4F;
        color: white;
        border-radius: 15px;
        font-size: 12px;
        font-weight: bold;
    )");

    cartHeader->addWidget(cartTitle);
    cartHeader->addStretch();
    cartHeader->addWidget(itemCountBadge);

    cartLayoutMain->addLayout(cartHeader);

    // Separator
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("background-color: #EEEEEE; border: none; max-height: 1px;");
    cartLayoutMain->addWidget(separator);

    // Cart Items Scroll
    QScrollArea *cartScroll = new QScrollArea();
    cartScroll->setWidgetResizable(true);
    cartScroll->setFrameShape(QFrame::NoFrame);
    cartScroll->setStyleSheet("QScrollArea { background: transparent; border: none; }");

    QWidget *cartContainer = new QWidget();
    cartContainer->setStyleSheet("background: transparent;");

    cartLayout = new QVBoxLayout(cartContainer);
    cartLayout->setSpacing(12);
    cartLayout->setAlignment(Qt::AlignTop);

    // Empty cart label
    QLabel *emptyLabel = new QLabel("🛒\n\nNo items in cart\nClick on products to add them");
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setStyleSheet(R"(
        color: #BBBBBB;
        font-size: 13px;
        background: transparent;
        padding: 40px 20px;
    )");
    cartLayout->addWidget(emptyLabel);

    cartScroll->setWidget(cartContainer);
    cartLayoutMain->addWidget(cartScroll, 1);

    // Separator 2
    QFrame *separator2 = new QFrame();
    separator2->setFrameShape(QFrame::HLine);
    separator2->setStyleSheet("background-color: #EEEEEE; border: none; max-height: 1px;");
    cartLayoutMain->addWidget(separator2);

    // =====================================================
    // 💰 Totals Section
    // =====================================================

    subtotalLabel = new QLabel("$0.00");
    taxLabel = new QLabel("$0.00");
    totalLabel = new QLabel("$0.00");

    auto createRow = [&](QString text, QLabel *value, bool isTotal = false) {
        QHBoxLayout *row = new QHBoxLayout();

        QLabel *label = new QLabel(text);

        if (isTotal) {
            label->setStyleSheet("font-size: 18px; font-weight: bold; color: #222; background: transparent;");
            value->setStyleSheet("font-size: 24px; font-weight: bold; color: #E68A4F; background: transparent;");
        } else {
            label->setStyleSheet("font-size: 14px; color: #777; background: transparent;");
            value->setStyleSheet("font-size: 14px; color: #222; font-weight: 600; background: transparent;");
        }

        row->addWidget(label);
        row->addStretch();
        row->addWidget(value);

        cartLayoutMain->addLayout(row);
    };

    createRow("Subtotal", subtotalLabel);
    createRow("Tax (15%)", taxLabel);
    createRow("Total", totalLabel, true);

    // =====================================================
    // ✅ Complete Button
    // =====================================================

    completeBtn = new QPushButton("✓  Complete Order");
    completeBtn->setFixedHeight(52);
    completeBtn->setCursor(Qt::PointingHandCursor);
    completeBtn->setEnabled(false);
    completeBtn->setStyleSheet(R"(
        QPushButton {
            background: #634226;
            color: white;
            border: none;
            border-radius: 14px;
            font-size: 15px;
            font-weight: bold;
        }
        QPushButton:hover {
            background: #4D331D;
        }
        QPushButton:disabled {
            background: #DDDDDD;
            color: #999999;
        }
    )");

    connect(completeBtn, &QPushButton::clicked, this, &POSWindow::completeOrder);

    cartLayoutMain->addSpacing(10);
    cartLayoutMain->addWidget(completeBtn);

    mainLayout->addWidget(cartCard);
}

// =====================================================
// 🗄️ Database Setup
// =====================================================

void POSWindow::setupDatabase()
{
    if (!DatabaseManager::instance().isConnected()) {
        qWarning() << "POS: Database not connected. Using demo products.";
    }
}

// =====================================================
// 📦 Load Products
// =====================================================

void POSWindow::loadProducts(QString category)
{
    // Clear existing products
    QLayoutItem *item;
    while ((item = productsGrid->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    QList<Product> products;

    // Try loading from database
    if (DatabaseManager::instance().isConnected()) {
        QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
        QSqlQuery query(db);
        query.prepare("SELECT ProductName, Price, Category FROM Products WHERE Category = ?");
        query.addBindValue(category);

        if (query.exec()) {
            while (query.next()) {
                Product p;
                p.name = query.value("ProductName").toString();
                p.price = query.value("Price").toDouble();
                p.category = query.value("Category").toString();
                products.append(p);
            }
        }
    }

    // Demo data if empty
    if (products.isEmpty()) {
        if (category == "Coffee") {
            products = {
                {"Cappuccino", 4.50, "Coffee"},
                {"Latte", 5.00, "Coffee"},
                {"Espresso", 3.50, "Coffee"},
                {"Americano", 3.00, "Coffee"},
                {"Mocha", 5.50, "Coffee"},
                {"Macchiato", 4.75, "Coffee"}
            };
        } else if (category == "Tea") {
            products = {
                {"Green Tea", 3.50, "Tea"},
                {"Black Tea", 3.00, "Tea"},
                {"Mint Tea", 3.25, "Tea"},
                {"Earl Grey", 3.75, "Tea"}
            };
        } else if (category == "Beverages") {
            products = {
                {"Hot Chocolate", 4.00, "Beverages"},
                {"Orange Juice", 3.50, "Beverages"},
                {"Lemonade", 3.00, "Beverages"},
                {"Smoothie", 5.00, "Beverages"}
            };
        } else if (category == "Pastry") {
            products = {
                {"Croissant", 3.00, "Pastry"},
                {"Muffin", 2.50, "Pastry"},
                {"Donut", 2.00, "Pastry"},
                {"Bagel", 2.75, "Pastry"}
            };
        } else if (category == "Dessert") {
            products = {
                {"Cheesecake", 6.00, "Dessert"},
                {"Brownie", 4.50, "Dessert"},
                {"Tiramisu", 6.50, "Dessert"},
                {"Ice Cream", 4.00, "Dessert"}
            };
        }
    }

    // Display in Grid
    int row = 0, col = 0;
    int maxCols = 3;

    for (const Product &p : products) {
        QFrame *card = createProductCard(p);
        productsGrid->addWidget(card, row, col);

        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

// =====================================================
// 🃏 Create Product Card
// =====================================================

QFrame* POSWindow::createProductCard(Product p)
{
    QFrame *card = new QFrame();
    card->setFixedSize(180, 170);
    card->setCursor(Qt::PointingHandCursor);
    card->setStyleSheet(R"(
        QFrame {
            background-color: white;
            border-radius: 14px;
            border: 1px solid #EFE7DE;
        }
        QFrame:hover {
            border: 2px solid #E68A4F;
        }
    )");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 20));
    shadow->setOffset(0, 3);
    card->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(15, 15, 15, 15);
    cardLayout->setSpacing(8);

    // Icon
    QString icon = "☕";
    if (p.category == "Tea") icon = "🍵";
    else if (p.category == "Beverages") icon = "🥤";
    else if (p.category == "Pastry") icon = "🥐";
    else if (p.category == "Dessert") icon = "🍰";

    QLabel *iconLabel = new QLabel(icon);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 38px; background: transparent; border: none;");

    QLabel *nameLabel = new QLabel(p.name);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setWordWrap(true);
    nameLabel->setStyleSheet(R"(
        font-size: 13px;
        font-weight: 600;
        color: #1A1A1A;
        background: transparent;
        border: none;
    )");

    QLabel *priceLabel = new QLabel(QString("$%1").arg(p.price, 0, 'f', 2));
    priceLabel->setAlignment(Qt::AlignCenter);
    priceLabel->setStyleSheet(R"(
        font-size: 17px;
        font-weight: bold;
        color: #E68A4F;
        background: transparent;
        border: none;
    )");

    cardLayout->addWidget(iconLabel);
    cardLayout->addWidget(nameLabel);
    cardLayout->addWidget(priceLabel);

    // Invisible clickable button
    QPushButton *clickBtn = new QPushButton(card);
    clickBtn->setStyleSheet("background: transparent; border: none;");
    clickBtn->setGeometry(0, 0, 180, 170);
    clickBtn->setCursor(Qt::PointingHandCursor);

    connect(clickBtn, &QPushButton::clicked, this, [this, p]() {
        addToCart(p.name, p.price);
    });

    return card;
}

// =====================================================
// 🏷️ Filter Products
// =====================================================

void POSWindow::filterProducts(QString category)
{
    loadProducts(category);
}

// =====================================================
// 🛒 Cart Operations
// =====================================================

void POSWindow::addToCart(QString name, double price)
{
    if (cartItems.contains(name)) {
        cartItems[name]++;
    } else {
        cartItems[name] = 1;
        itemPrices[name] = price;
    }

    updateCartUI();
    calculateTotals();
}

void POSWindow::removeFromCart(QString name)
{
    cartItems.remove(name);
    itemPrices.remove(name);

    updateCartUI();
    calculateTotals();
}

void POSWindow::changeQuantity(QString name, int delta)
{
    if (!cartItems.contains(name)) return;

    cartItems[name] += delta;
    if (cartItems[name] <= 0) {
        removeFromCart(name);
        return;
    }

    updateCartUI();
    calculateTotals();
}

// =====================================================
// 🔄 Update Cart UI
// =====================================================

void POSWindow::updateCartUI()
{
    // Clear cart layout
    QLayoutItem *item;
    while ((item = cartLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    // Empty cart
    if (cartItems.isEmpty()) {
        QLabel *emptyLabel = new QLabel("🛒\n\nNo items in cart\nClick on products to add them");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet(R"(
            color: #BBBBBB;
            font-size: 13px;
            background: transparent;
            padding: 40px 20px;
        )");
        cartLayout->addWidget(emptyLabel);

        itemCountBadge->setText("0");
        completeBtn->setEnabled(false);
        return;
    }

    int totalItems = 0;

    for (auto it = cartItems.begin(); it != cartItems.end(); ++it) {
        QString name = it.key();
        int qty = it.value();
        double price = itemPrices[name];
        totalItems += qty;

        // Item Card
        QFrame *itemCard = new QFrame();
        itemCard->setStyleSheet("QFrame { background-color: #FAF6F1; border-radius: 10px; }");

        QHBoxLayout *itemLayout = new QHBoxLayout(itemCard);
        itemLayout->setContentsMargins(12, 10, 12, 10);
        itemLayout->setSpacing(8);

        // Info
        QVBoxLayout *infoLayout = new QVBoxLayout();
        infoLayout->setSpacing(2);

        QLabel *nameLabel = new QLabel(name);
        nameLabel->setStyleSheet("font-size: 13px; font-weight: 600; color: #1A1A1A; background: transparent;");

        QLabel *priceLabel = new QLabel(QString("$%1 x %2").arg(price, 0, 'f', 2).arg(qty));
        priceLabel->setStyleSheet("font-size: 11px; color: #888888; background: transparent;");

        infoLayout->addWidget(nameLabel);
        infoLayout->addWidget(priceLabel);

        // Minus Button
        QPushButton *minusBtn = new QPushButton("−");
        minusBtn->setFixedSize(26, 26);
        minusBtn->setCursor(Qt::PointingHandCursor);
        minusBtn->setStyleSheet(R"(
            QPushButton {
                background-color: white;
                border: 1px solid #DDDDDD;
                border-radius: 13px;
                font-size: 14px;
                font-weight: bold;
                color: #634226;
            }
            QPushButton:hover { background-color: #F0F0F0; }
        )");

        // Quantity Label
        QLabel *qtyLabel = new QLabel(QString::number(qty));
        qtyLabel->setAlignment(Qt::AlignCenter);
        qtyLabel->setFixedWidth(20);
        qtyLabel->setStyleSheet("font-size: 13px; font-weight: bold; color: #1A1A1A; background: transparent;");

        // Plus Button
        QPushButton *plusBtn = new QPushButton("+");
        plusBtn->setFixedSize(26, 26);
        plusBtn->setCursor(Qt::PointingHandCursor);
        plusBtn->setStyleSheet(R"(
            QPushButton {
                background-color: #E68A4F;
                border: none;
                border-radius: 13px;
                font-size: 14px;
                font-weight: bold;
                color: white;
            }
            QPushButton:hover { background-color: #D97838; }
        )");

        // Delete Button
        QPushButton *deleteBtn = new QPushButton("🗑");
        deleteBtn->setFixedSize(26, 26);
        deleteBtn->setCursor(Qt::PointingHandCursor);
        deleteBtn->setStyleSheet(R"(
            QPushButton {
                background-color: transparent;
                border: none;
                font-size: 13px;
                color: #D32F2F;
            }
            QPushButton:hover {
                background-color: #FFEBEE;
                border-radius: 13px;
            }
        )");

        itemLayout->addLayout(infoLayout);
        itemLayout->addStretch();
        itemLayout->addWidget(minusBtn);
        itemLayout->addWidget(qtyLabel);
        itemLayout->addWidget(plusBtn);
        itemLayout->addWidget(deleteBtn);

        // Connect buttons
        connect(minusBtn, &QPushButton::clicked, this, [this, name]() { changeQuantity(name, -1); });
        connect(plusBtn, &QPushButton::clicked, this, [this, name]() { changeQuantity(name, 1); });
        connect(deleteBtn, &QPushButton::clicked, this, [this, name]() { removeFromCart(name); });

        cartLayout->addWidget(itemCard);
    }

    cartLayout->addStretch();
    itemCountBadge->setText(QString::number(totalItems));
    completeBtn->setEnabled(true);
}

// =====================================================
// 💰 Calculate Totals
// =====================================================

void POSWindow::calculateTotals()
{
    double subtotal = 0.0;

    for (auto it = cartItems.begin(); it != cartItems.end(); ++it) {
        subtotal += itemPrices[it.key()] * it.value();
    }

    double tax = subtotal * 0.15;
    double total = subtotal + tax;

    subtotalLabel->setText(QString("$%1").arg(subtotal, 0, 'f', 2));
    taxLabel->setText(QString("$%1").arg(tax, 0, 'f', 2));
    totalLabel->setText(QString("$%1").arg(total, 0, 'f', 2));
}

// =====================================================
// ✅ Complete Order
// =====================================================

void POSWindow::completeOrder()
{
    if (cartItems.isEmpty()) return;

    double subtotal = 0.0;
    for (auto it = cartItems.begin(); it != cartItems.end(); ++it) {
        subtotal += itemPrices[it.key()] * it.value();
    }
    double total = subtotal * 1.15;

    // Build summary
    QString orderSummary = "Order Summary:\n\n";
    for (auto it = cartItems.begin(); it != cartItems.end(); ++it) {
        orderSummary += QString("  %1 x %2  =  $%3\n")
        .arg(it.key())
            .arg(it.value())
            .arg(itemPrices[it.key()] * it.value(), 0, 'f', 2);
    }
    orderSummary += QString("\nTotal: $%1").arg(total, 0, 'f', 2);

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Complete Order",
        orderSummary + "\n\nConfirm order?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // Save to database
        if (DatabaseManager::instance().isConnected()) {
            QString currentDate = QDate::currentDate().toString("yyyy-MM-dd");
            QString currentTime = QTime::currentTime().toString("HH:mm:ss");

            bool success = DatabaseManager::instance().insertOrder(
                m_username, currentDate, currentTime, total, "completed"
                );

            if (success) {
                QMessageBox::information(this, "Success", "Order completed successfully! ✓");
            } else {
                QMessageBox::warning(this, "Warning",
                                     "Failed to save to database:\n" +
                                         DatabaseManager::instance().lastError());
            }
        } else {
            QMessageBox::information(this, "Success", "Order completed (Demo mode) ✓");
        }

        // Clear cart
        cartItems.clear();
        itemPrices.clear();
        updateCartUI();
        calculateTotals();
    }
}

// =====================================================
// 🚪 Logout
// =====================================================

void POSWindow::onLogout()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Logout",
        "Are you sure you want to logout?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        this->close();
    }
}