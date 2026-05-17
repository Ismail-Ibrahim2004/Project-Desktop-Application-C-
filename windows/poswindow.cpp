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
#include <QHeaderView>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>

POSWindow::POSWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupDatabase();
    loadProducts("Coffee");

    setWindowTitle("Cafe Manager - POS");
    resize(1400, 850);
    setMinimumSize(1200, 750);
}

POSWindow::~POSWindow() {}

void POSWindow::setUserInfo(const QString &username)
{
    m_username = username;
    if (m_userLabel) {
        m_userLabel->setText("  " + username);
    }
    if (receiptServedBy) {
        receiptServedBy->setText("Served by: " + username);
    }
    setWindowTitle(QString("Cafe Manager - POS (%1)").arg(username));
}

// =====================================================
// Setup UI
// =====================================================

void POSWindow::setupUI()
{
    setStyleSheet("background-color: #FAF6F1;");

    QWidget *centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // =====================================================
    // SIDEBAR
    // =====================================================
    QFrame *sidebar = new QFrame();
    sidebar->setFixedWidth(250);
    sidebar->setStyleSheet("QFrame { background-color: #634226; border: none; }");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(20, 30, 20, 20);
    sidebarLayout->setSpacing(15);

    // Logo with icon from resources
    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->setSpacing(12);

    QLabel *logoIcon = new QLabel();
    logoIcon->setFixedSize(42, 42);
    logoIcon->setAlignment(Qt::AlignCenter);
    logoIcon->setPixmap(
        QPixmap(":/icons/coffee-cup (1).png")
            .scaled(28, 28, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    logoIcon->setStyleSheet("background: #E68A4F; border-radius: 10px; padding: 6px;");

    QVBoxLayout *logoTextLayout = new QVBoxLayout();
    logoTextLayout->setSpacing(2);
    QLabel *logo = new QLabel("Cafe Manager");
    logo->setStyleSheet("color: white; font-size: 16px; font-weight: bold; background: transparent;");
    QLabel *subLogo = new QLabel("Employee");
    subLogo->setStyleSheet("color: #D8C3AF; font-size: 12px; background: transparent;");
    logoTextLayout->addWidget(logo);
    logoTextLayout->addWidget(subLogo);

    logoLayout->addWidget(logoIcon);
    logoLayout->addLayout(logoTextLayout);
    logoLayout->addStretch();
    sidebarLayout->addLayout(logoLayout);
    sidebarLayout->addSpacing(40);

    // POS Button with icon
    QPushButton *posBtn = new QPushButton("  POS System");
    posBtn->setFixedHeight(48);
    posBtn->setIcon(QIcon(":/icons/checkout.png"));
    posBtn->setIconSize(QSize(20, 20));
    posBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #E68A4F;
            color: white;
            border: none;
            border-radius: 12px;
            font-size: 15px;
            font-weight: bold;
            text-align: left;
            padding-left: 18px;
        }
    )");
    sidebarLayout->addWidget(posBtn);
    sidebarLayout->addStretch();

    // Logout Button with icon
    QPushButton *logoutBtn = new QPushButton("  Logout");
    logoutBtn->setFixedHeight(45);
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setIcon(QIcon(":/icons/logout.png"));
    logoutBtn->setIconSize(QSize(18, 18));
    logoutBtn->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            color: #D8C3AF;
            border: none;
            font-size: 15px;
            font-weight: bold;
            text-align: left;
            padding-left: 18px;
        }
        QPushButton:hover { color: #FF7675; }
    )");
    connect(logoutBtn, &QPushButton::clicked, this, &POSWindow::onLogout);
    sidebarLayout->addWidget(logoutBtn);

    mainLayout->addWidget(sidebar);

    // =====================================================
    // STACKED WIDGET
    // =====================================================
    mainStack = new QStackedWidget();
    mainStack->setStyleSheet("background-color: #FAF6F1;");

    setupPOSPage();
    setupReceiptPage();

    mainStack->addWidget(posPageWidget);
    mainStack->addWidget(receiptPageWidget);

    mainLayout->addWidget(mainStack, 1);
}

// =====================================================
// PAGE 0: POS SYSTEM
// =====================================================

void POSWindow::setupPOSPage()
{
    posPageWidget = new QWidget();
    QVBoxLayout *posMainLayout = new QVBoxLayout(posPageWidget);
    posMainLayout->setContentsMargins(0, 0, 0, 0);
    posMainLayout->setSpacing(0);

    // Top Bar
    QFrame *topBar = new QFrame();
    topBar->setFixedHeight(70);
    topBar->setStyleSheet("background-color: white; border-bottom: 1px solid #EFE7DE;");

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(30, 0, 30, 0);

    QLabel *posHeaderTitle = new QLabel("Point Of Sale");
    posHeaderTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #2B2B2B; border: none;");

    // User label with icon
    QWidget *userWidget = new QWidget();
    userWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *userLayout = new QHBoxLayout(userWidget);
    userLayout->setContentsMargins(0, 0, 0, 0);
    userLayout->setSpacing(8);

    QLabel *userIcon = new QLabel();
    userIcon->setFixedSize(20, 20);
    userIcon->setPixmap(
        QPixmap(":/icons/employee.png")
            .scaled(18, 18, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    userIcon->setStyleSheet("background: transparent; border: none;");

    m_userLabel = new QLabel("Employee");
    m_userLabel->setStyleSheet("font-size: 14px; color: #666666; border: none; background: transparent;");

    userLayout->addWidget(userIcon);
    userLayout->addWidget(m_userLabel);

    topLayout->addWidget(posHeaderTitle);
    topLayout->addStretch();
    topLayout->addWidget(userWidget);

    posMainLayout->addWidget(topBar);

    // Content Area
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(30, 25, 30, 25);
    contentLayout->setSpacing(25);

    // LEFT: Products
    QWidget *leftWidget = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(15);

    // Categories
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
                padding: 0 20px;
                color: #634226;
                font-size: 13px;
                font-weight: 600;
            }
            QPushButton:hover { background: #634226; color: white; }
        )");
        connect(btn, &QPushButton::clicked, this, [this, cat]() { filterProducts(cat); });
        categoriesLayout->addWidget(btn);
    }
    categoriesLayout->addStretch();
    leftLayout->addLayout(categoriesLayout);

    // Products Grid
    QScrollArea *productsScroll = new QScrollArea();
    productsScroll->setWidgetResizable(true);
    productsScroll->setFrameShape(QFrame::NoFrame);
    productsScroll->setStyleSheet("background: transparent;");

    QWidget *productsContainer = new QWidget();
    productsContainer->setStyleSheet("background: transparent;");

    productsGrid = new QGridLayout(productsContainer);
    productsGrid->setSpacing(20);
    productsGrid->setContentsMargins(5, 5, 5, 5);

    productsScroll->setWidget(productsContainer);
    leftLayout->addWidget(productsScroll);

    contentLayout->addWidget(leftWidget, 2);

    // RIGHT: Cart
    QFrame *cartCard = new QFrame();
    cartCard->setFixedWidth(390);
    cartCard->setStyleSheet("QFrame { background: white; border-radius: 16px; border: 1px solid #EFE7DE; }");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 15));
    cartCard->setGraphicsEffect(shadow);

    QVBoxLayout *cartLayoutMain = new QVBoxLayout(cartCard);
    cartLayoutMain->setContentsMargins(25, 25, 25, 25);
    cartLayoutMain->setSpacing(15);

    // Cart Header
    QHBoxLayout *cartHeader = new QHBoxLayout();
    QLabel *cartTitle = new QLabel("Current Order");
    cartTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #222; border: none;");

    itemCountBadge = new QLabel("0");
    itemCountBadge->setFixedSize(28, 28);
    itemCountBadge->setAlignment(Qt::AlignCenter);
    itemCountBadge->setStyleSheet("background: #E68A4F; color: white; border-radius: 14px; font-weight: bold;");

    cartHeader->addWidget(cartTitle);
    cartHeader->addStretch();
    cartHeader->addWidget(itemCountBadge);
    cartLayoutMain->addLayout(cartHeader);

    QFrame *sep1 = new QFrame();
    sep1->setFrameShape(QFrame::HLine);
    sep1->setStyleSheet("background: #EFE7DE; max-height: 1px; border: none;");
    cartLayoutMain->addWidget(sep1);

    // Cart Items Scroll
    QScrollArea *cartScroll = new QScrollArea();
    cartScroll->setWidgetResizable(true);
    cartScroll->setFrameShape(QFrame::NoFrame);
    cartScroll->setStyleSheet("background: transparent;");

    QWidget *cartContainer = new QWidget();
    cartContainer->setStyleSheet("background: transparent;");
    cartLayout = new QVBoxLayout(cartContainer);
    cartLayout->setSpacing(10);
    cartLayout->setAlignment(Qt::AlignTop);

    QLabel *emptyLabel = new QLabel("No items in cart");
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setStyleSheet("color: #BBB; font-size: 14px; padding: 40px; border: none;");
    cartLayout->addWidget(emptyLabel);

    cartScroll->setWidget(cartContainer);
    cartLayoutMain->addWidget(cartScroll, 1);

    QFrame *sep2 = new QFrame();
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("background: #EFE7DE; max-height: 1px; border: none;");
    cartLayoutMain->addWidget(sep2);

    // Totals
    subtotalLabel = new QLabel("$0.00");
    taxLabel      = new QLabel("$0.00");
    totalLabel    = new QLabel("$0.00");

    auto createRow = [&](QString text, QLabel *value, bool isTotal = false) {
        QHBoxLayout *row = new QHBoxLayout();
        QLabel *lbl = new QLabel(text);
        if (isTotal) {
            lbl->setStyleSheet("font-size: 16px; font-weight: bold; color: #222; border: none;");
            value->setStyleSheet("font-size: 22px; font-weight: bold; color: #E68A4F; border: none;");
        } else {
            lbl->setStyleSheet("font-size: 13px; color: #777; border: none;");
            value->setStyleSheet("font-size: 13px; color: #222; font-weight: 600; border: none;");
        }
        row->addWidget(lbl); row->addStretch(); row->addWidget(value);
        cartLayoutMain->addLayout(row);
    };

    createRow("Subtotal", subtotalLabel);
    createRow("Tax (8%)", taxLabel);
    createRow("Total",    totalLabel, true);

    // Complete Button with icon
    completeBtn = new QPushButton("  Complete Order");
    completeBtn->setFixedHeight(50);
    completeBtn->setCursor(Qt::PointingHandCursor);
    completeBtn->setEnabled(false);
    completeBtn->setIcon(QIcon(":/icons/checkout.png"));
    completeBtn->setIconSize(QSize(18, 18));
    completeBtn->setStyleSheet(R"(
        QPushButton {
            background: #634226; color: white; border: none;
            border-radius: 12px; font-size: 15px; font-weight: bold;
        }
        QPushButton:hover { background: #4D331D; }
        QPushButton:disabled { background: #DDD; color: #999; }
    )");
    connect(completeBtn, &QPushButton::clicked, this, &POSWindow::completeOrder);

    cartLayoutMain->addSpacing(5);
    cartLayoutMain->addWidget(completeBtn);

    contentLayout->addWidget(cartCard);
    posMainLayout->addLayout(contentLayout, 1);
}

// =====================================================
// PAGE 1: RECEIPT PAGE
// =====================================================

void POSWindow::setupReceiptPage()
{
    receiptPageWidget = new QWidget();

    QVBoxLayout *outerLayout = new QVBoxLayout(receiptPageWidget);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(R"(
        QScrollArea { background-color: #FAF6F1; border: none; }
        QScrollBar:vertical {
            background: transparent; width: 10px; margin: 0;
        }
        QScrollBar::handle:vertical {
            background: rgba(99, 66, 38, 0.3); border-radius: 5px; min-height: 30px;
        }
        QScrollBar::handle:vertical:hover { background: rgba(99, 66, 38, 0.5); }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )");

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background-color: #FAF6F1;");

    QVBoxLayout *receiptMainLayout = new QVBoxLayout(scrollContent);
    receiptMainLayout->setContentsMargins(40, 30, 40, 30);
    receiptMainLayout->setSpacing(20);
    receiptMainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // 1. SUCCESS BANNER
    QFrame *banner = new QFrame();
    banner->setFixedSize(550, 80);
    banner->setStyleSheet(R"(
        QFrame {
            background-color: #F0F9F4;
            border: 1px solid #D4EDDA;
            border-radius: 12px;
        }
    )");

    QHBoxLayout *bannerLayout = new QHBoxLayout(banner);
    bannerLayout->setContentsMargins(25, 15, 25, 15);
    bannerLayout->setSpacing(15);

    QLabel *checkIcon = new QLabel("✓");
    checkIcon->setFixedSize(40, 40);
    checkIcon->setAlignment(Qt::AlignCenter);
    checkIcon->setStyleSheet(R"(
        QLabel {
            background-color: #2ECC71; color: white;
            border-radius: 20px; font-size: 20px;
            font-weight: bold; border: none;
        }
    )");

    QVBoxLayout *bannerTexts = new QVBoxLayout();
    bannerTexts->setSpacing(2);

    QLabel *bannerTitle = new QLabel("Order Completed Successfully!");
    bannerTitle->setStyleSheet(
        "color: #155724; font-size: 16px; font-weight: bold; background: transparent; border: none;");

    QLabel *bannerSub = new QLabel("Receipt is ready to print");
    bannerSub->setStyleSheet(
        "color: #2E7D32; font-size: 13px; background: transparent; border: none;");

    bannerTexts->addWidget(bannerTitle);
    bannerTexts->addWidget(bannerSub);

    bannerLayout->addWidget(checkIcon);
    bannerLayout->addLayout(bannerTexts);
    bannerLayout->addStretch();

    receiptMainLayout->addWidget(banner);

    // 2. WHITE RECEIPT CARD
    QFrame *card = new QFrame();
    card->setFixedWidth(550);
    card->setStyleSheet(R"(
        QFrame {
            background-color: white;
            border-radius: 16px;
            border: 1px solid #EFE7DE;
        }
    )");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(25);
    shadow->setOffset(0, 5);
    shadow->setColor(QColor(0, 0, 0, 15));
    card->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 35, 40, 35);
    cardLayout->setSpacing(15);
    cardLayout->setAlignment(Qt::AlignTop);

    // Coffee icon from resources
    QLabel *coffeeCircle = new QLabel();
    coffeeCircle->setFixedSize(56, 56);
    coffeeCircle->setAlignment(Qt::AlignCenter);
    coffeeCircle->setPixmap(
        QPixmap(":/icons/coffee-cup (1).png")
            .scaled(36, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    coffeeCircle->setStyleSheet(R"(
        QLabel {
            background-color: #E67E22;
            border-radius: 28px;
            padding: 8px;
            border: none;
        }
    )");

    QLabel *cafeTitle = new QLabel("Café Manager");
    cafeTitle->setAlignment(Qt::AlignCenter);
    cafeTitle->setStyleSheet(
        "color: #5D3A1A; font-size: 24px; font-weight: bold; background: transparent; border: none;");

    QLabel *cafeAddress = new QLabel("123 Coffee Street, Suite 100\nTel: (555) 123-4567");
    cafeAddress->setAlignment(Qt::AlignCenter);
    cafeAddress->setStyleSheet(
        "color: #666666; font-size: 13px; background: transparent; border: none;");

    cardLayout->addWidget(coffeeCircle, 0, Qt::AlignHCenter);
    cardLayout->addSpacing(5);
    cardLayout->addWidget(cafeTitle);
    cardLayout->addWidget(cafeAddress);
    cardLayout->addSpacing(10);

    QFrame *dash1 = new QFrame();
    dash1->setFixedHeight(1);
    dash1->setStyleSheet("border-top: 1px dashed #C9BBAE; background: transparent;");
    cardLayout->addWidget(dash1);
    cardLayout->addSpacing(10);

    receiptOrderNum = new QLabel("Order ID: ORD-337");
    receiptOrderNum->setAlignment(Qt::AlignCenter);
    receiptOrderNum->setStyleSheet(
        "color: #333333; font-size: 13px; font-weight: 500; background: transparent; border: none;");

    receiptDateTime = new QLabel("March 7, 2026 at 02:45 PM");
    receiptDateTime->setAlignment(Qt::AlignCenter);
    receiptDateTime->setStyleSheet(
        "color: #666666; font-size: 13px; background: transparent; border: none;");

    receiptServedBy = new QLabel("Served by: John Doe");
    receiptServedBy->setAlignment(Qt::AlignCenter);
    receiptServedBy->setStyleSheet(
        "color: #666666; font-size: 13px; background: transparent; border: none;");

    cardLayout->addWidget(receiptOrderNum);
    cardLayout->addWidget(receiptDateTime);
    cardLayout->addWidget(receiptServedBy);
    cardLayout->addSpacing(15);

    // Items Table
    receiptTable = new QTableWidget();
    receiptTable->setColumnCount(4);
    receiptTable->setHorizontalHeaderLabels(
        QStringList() << "Item" << "Qty" << "Price" << "Total");

    receiptTable->setStyleSheet(R"(
        QTableWidget {
            background: white; border: none;
            color: #1A1A1A; font-size: 13px; outline: none;
        }
        QTableWidget::item {
            padding: 12px 5px;
            border-bottom: 1px solid #F5F0E8;
        }
        QHeaderView::section {
            background: white; color: #1A1A1A;
            font-weight: bold; font-size: 13px;
            border: none; border-bottom: 2px solid #EFE7DE;
            padding: 10px 5px;
        }
    )");

    receiptTable->horizontalHeader()->setStretchLastSection(false);
    receiptTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    receiptTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    receiptTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    receiptTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    receiptTable->setColumnWidth(1, 50);
    receiptTable->setColumnWidth(2, 70);
    receiptTable->setColumnWidth(3, 70);

    receiptTable->verticalHeader()->setVisible(false);
    receiptTable->verticalHeader()->setDefaultSectionSize(45);
    receiptTable->setShowGrid(false);
    receiptTable->setFocusPolicy(Qt::NoFocus);
    receiptTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    receiptTable->setSelectionMode(QAbstractItemView::NoSelection);
    receiptTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    receiptTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    receiptTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    cardLayout->addWidget(receiptTable);
    cardLayout->addSpacing(10);

    // Totals
    QGridLayout *totalsLayout = new QGridLayout();
    totalsLayout->setSpacing(10);
    totalsLayout->setContentsMargins(0, 5, 0, 5);

    QLabel *subText = new QLabel("Subtotal");
    subText->setStyleSheet("color: #555555; font-size: 14px; background: transparent; border: none;");
    receiptSubtotal = new QLabel("$18.50");
    receiptSubtotal->setAlignment(Qt::AlignRight);
    receiptSubtotal->setStyleSheet(
        "color: #333333; font-size: 14px; font-weight: 500; background: transparent; border: none;");

    QLabel *taxText = new QLabel("Tax (8%)");
    taxText->setStyleSheet("color: #555555; font-size: 14px; background: transparent; border: none;");
    receiptTax = new QLabel("$1.48");
    receiptTax->setAlignment(Qt::AlignRight);
    receiptTax->setStyleSheet(
        "color: #333333; font-size: 14px; font-weight: 500; background: transparent; border: none;");

    QLabel *totText = new QLabel("Total");
    totText->setStyleSheet(
        "color: #1A1A1A; font-size: 17px; font-weight: bold; background: transparent; border: none;");
    receiptTotal = new QLabel("$19.98");
    receiptTotal->setAlignment(Qt::AlignRight);
    receiptTotal->setStyleSheet(
        "color: #E67E22; font-size: 20px; font-weight: bold; background: transparent; border: none;");

    totalsLayout->addWidget(subText,       0, 0);
    totalsLayout->addWidget(receiptSubtotal, 0, 1);
    totalsLayout->addWidget(taxText,       1, 0);
    totalsLayout->addWidget(receiptTax,    1, 1);
    totalsLayout->addWidget(totText,       2, 0);
    totalsLayout->addWidget(receiptTotal,  2, 1);

    cardLayout->addLayout(totalsLayout);
    cardLayout->addSpacing(15);

    QFrame *dash2 = new QFrame();
    dash2->setFixedHeight(1);
    dash2->setStyleSheet("border-top: 1px dashed #C9BBAE; background: transparent;");
    cardLayout->addWidget(dash2);
    cardLayout->addSpacing(15);

    QLabel *footerThanks = new QLabel("Thank you for your visit!");
    footerThanks->setAlignment(Qt::AlignCenter);
    footerThanks->setStyleSheet(
        "color: #5D3A1A; font-size: 15px; font-weight: bold; background: transparent; border: none;");

    QLabel *footerCome = new QLabel("Please come again");
    footerCome->setAlignment(Qt::AlignCenter);
    footerCome->setStyleSheet(
        "color: #888888; font-size: 12px; background: transparent; border: none;");

    QLabel *footerWeb = new QLabel("www.cafemanager.com");
    footerWeb->setAlignment(Qt::AlignCenter);
    footerWeb->setStyleSheet(
        "color: #888888; font-size: 12px; background: transparent; border: none;");

    cardLayout->addWidget(footerThanks);
    cardLayout->addWidget(footerCome);
    cardLayout->addWidget(footerWeb);

    receiptMainLayout->addWidget(card);

    // 3. BOTTOM BUTTONS
    QHBoxLayout *btnsLayout = new QHBoxLayout();
    btnsLayout->setSpacing(20);
    btnsLayout->setAlignment(Qt::AlignHCenter);
    btnsLayout->setContentsMargins(0, 10, 0, 20);

    QPushButton *printBtn = new QPushButton("  Print Receipt");
    printBtn->setFixedSize(265, 50);
    printBtn->setCursor(Qt::PointingHandCursor);
    printBtn->setIcon(QIcon(":/icons/printer.png"));
    printBtn->setIconSize(QSize(18, 18));
    printBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #5D3A1A; color: white;
            border: none; border-radius: 10px;
            font-size: 14px; font-weight: bold;
        }
        QPushButton:hover { background-color: #432812; }
    )");
    connect(printBtn, &QPushButton::clicked, this, &POSWindow::printReceipt);

    QPushButton *newOrderBtn = new QPushButton("  New Order");
    newOrderBtn->setFixedSize(265, 50);
    newOrderBtn->setCursor(Qt::PointingHandCursor);
    newOrderBtn->setIcon(QIcon(":/icons/plus.png"));
    newOrderBtn->setIconSize(QSize(18, 18));
    newOrderBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #E67E22; color: white;
            border: none; border-radius: 10px;
            font-size: 14px; font-weight: bold;
        }
        QPushButton:hover { background-color: #D35400; }
    )");
    connect(newOrderBtn, &QPushButton::clicked, this, &POSWindow::showPOSView);

    btnsLayout->addWidget(printBtn);
    btnsLayout->addWidget(newOrderBtn);

    receiptMainLayout->addLayout(btnsLayout);

    scrollArea->setWidget(scrollContent);
    outerLayout->addWidget(scrollArea);
}

// =====================================================
// View Navigation
// =====================================================

void POSWindow::showPOSView()
{
    cartItems.clear();
    itemPrices.clear();
    updateCartUI();
    calculateTotals();
    mainStack->setCurrentIndex(0);
}

// =====================================================
// Database & Products
// =====================================================

void POSWindow::setupDatabase()
{
    if (!DatabaseManager::instance().isConnected()) {
        qWarning() << "POS: Database not connected.";
    }
}

void POSWindow::loadProducts(QString category)
{
    QLayoutItem *item;
    while ((item = productsGrid->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    QList<Product> products;
    if (DatabaseManager::instance().isConnected()) {
        QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
        QSqlQuery q(db);
        q.prepare("SELECT ProductName, Price, Category FROM Products WHERE Category = ? AND Status = 'active'");
        q.addBindValue(category);
        if (q.exec()) {
            while (q.next()) {
                products.append({q.value(0).toString(), q.value(1).toDouble(), q.value(2).toString()});
            }
        }
    }

    if (products.isEmpty()) {
        if (category == "Coffee") {
            products = {{"Cappuccino", 4.25, "Coffee"}, {"Latte", 4.50, "Coffee"}, {"Espresso", 3.50, "Coffee"}};
        } else if (category == "Dessert") {
            products = {{"Chocolate Cake", 5.50, "Dessert"}, {"Cheesecake", 6.00, "Dessert"}};
        }
    }

    int row = 0, col = 0;
    for (const Product &p : products) {
        productsGrid->addWidget(createProductCard(p), row, col);
        if (++col >= 3) { col = 0; row++; }
    }
}

QFrame* POSWindow::createProductCard(Product p)
{
    QFrame *card = new QFrame();
    card->setFixedSize(180, 160);
    card->setCursor(Qt::PointingHandCursor);
    card->setStyleSheet(
        "QFrame { background: white; border-radius: 12px; border: 1px solid #EFE7DE; }"
        "QFrame:hover { border: 2px solid #E68A4F; }");

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(15, 15, 15, 15);

    // Product icon from resources
    QLabel *icon = new QLabel();
    icon->setAlignment(Qt::AlignCenter);
    icon->setFixedSize(48, 48);
    icon->setPixmap(
        QPixmap(p.category == "Dessert" ? ":/icons/product.png" : ":/icons/coffee-cup (1).png")
            .scaled(36, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    icon->setStyleSheet("background: #FFF0E0; border-radius: 10px; padding: 5px; border: none;");

    QLabel *name = new QLabel(p.name);
    name->setAlignment(Qt::AlignCenter);
    name->setStyleSheet(
        "font-size: 13px; font-weight: bold; color: #222; border: none; background: transparent;");

    QLabel *price = new QLabel(QString("$%1").arg(p.price, 0, 'f', 2));
    price->setAlignment(Qt::AlignCenter);
    price->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #E68A4F; border: none; background: transparent;");

    layout->addWidget(icon, 0, Qt::AlignHCenter);
    layout->addWidget(name);
    layout->addWidget(price);

    QPushButton *btn = new QPushButton(card);
    btn->setGeometry(0, 0, 180, 160);
    btn->setStyleSheet("background: transparent; border: none;");
    connect(btn, &QPushButton::clicked, this, [this, p]() { addToCart(p.name, p.price); });

    return card;
}

void POSWindow::filterProducts(QString category) { loadProducts(category); }

// =====================================================
// Cart Logic
// =====================================================

void POSWindow::addToCart(QString name, double price)
{
    cartItems[name]++;
    itemPrices[name] = price;
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
    if (cartItems[name] <= 0) removeFromCart(name);
    else { updateCartUI(); calculateTotals(); }
}

void POSWindow::updateCartUI()
{
    QLayoutItem *it;
    while ((it = cartLayout->takeAt(0))) {
        if (it->widget()) it->widget()->deleteLater();
        delete it;
    }

    if (cartItems.isEmpty()) {
        QLabel *emptyLabel = new QLabel("No items in cart");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: #BBB; font-size: 14px; padding: 40px; border: none;");
        cartLayout->addWidget(emptyLabel);
        itemCountBadge->setText("0");
        completeBtn->setEnabled(false);
        return;
    }

    int totalCount = 0;
    for (auto i = cartItems.begin(); i != cartItems.end(); ++i) {
        QString name = i.key(); int qty = i.value(); double price = itemPrices[name];
        totalCount += qty;

        QFrame *itemCard = new QFrame();
        itemCard->setStyleSheet("background: #FAF6F1; border-radius: 8px; border: none;");
        QHBoxLayout *iLayout = new QHBoxLayout(itemCard);

        QVBoxLayout *info = new QVBoxLayout();
        QLabel *nLabel = new QLabel(name);
        nLabel->setStyleSheet("font-weight: bold; color: #222; font-size: 13px;");
        QLabel *pLabel = new QLabel(QString("$%1 x %2").arg(price, 0, 'f', 2).arg(qty));
        pLabel->setStyleSheet("color: #777; font-size: 12px;");
        info->addWidget(nLabel);
        info->addWidget(pLabel);

        QPushButton *mBtn = new QPushButton("−"); mBtn->setFixedSize(24, 24);
        mBtn->setStyleSheet("background: white; border: 1px solid #DDD; border-radius: 12px; font-weight: bold; color: #634226;");

        QLabel *qLabel = new QLabel(QString::number(qty));
        qLabel->setFixedWidth(20); qLabel->setAlignment(Qt::AlignCenter);
        qLabel->setStyleSheet("font-weight: bold; color: #222; font-size: 13px;");

        QPushButton *pBtn = new QPushButton("+"); pBtn->setFixedSize(24, 24);
        pBtn->setStyleSheet("background: #E68A4F; color: white; border: none; border-radius: 12px; font-weight: bold;");

        // Delete button with icon
        QPushButton *dBtn = new QPushButton(); dBtn->setFixedSize(24, 24);
        dBtn->setIcon(QIcon(":/icons/trash.png"));
        dBtn->setIconSize(QSize(14, 14));
        dBtn->setStyleSheet("background: transparent; border: none;");

        iLayout->addLayout(info); iLayout->addStretch();
        iLayout->addWidget(mBtn); iLayout->addWidget(qLabel);
        iLayout->addWidget(pBtn); iLayout->addWidget(dBtn);

        connect(mBtn, &QPushButton::clicked, this, [this, name]() { changeQuantity(name, -1); });
        connect(pBtn, &QPushButton::clicked, this, [this, name]() { changeQuantity(name, 1); });
        connect(dBtn, &QPushButton::clicked, this, [this, name]() { removeFromCart(name); });

        cartLayout->addWidget(itemCard);
    }
    itemCountBadge->setText(QString::number(totalCount));
    completeBtn->setEnabled(true);
}

void POSWindow::calculateTotals()
{
    double subtotal = 0.0;
    for (auto i = cartItems.begin(); i != cartItems.end(); ++i)
        subtotal += itemPrices[i.key()] * i.value();

    double tax = subtotal * 0.08;
    currentOrderTotal = subtotal + tax;

    subtotalLabel->setText(QString("$%1").arg(subtotal, 0, 'f', 2));
    taxLabel->setText(QString("$%1").arg(tax, 0, 'f', 2));
    totalLabel->setText(QString("$%1").arg(currentOrderTotal, 0, 'f', 2));
}

QString POSWindow::generateOrderNumber()
{
    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery q(db);
    if (q.exec("SELECT TOP 1 OrderID FROM Orders ORDER BY OrderID DESC") && q.next()) {
        return QString("ORD-%1").arg(q.value(0).toInt() + 1, 3, 10, QChar('0'));
    }
    return "ORD-337";
}

// =====================================================
// Complete Order
// =====================================================

void POSWindow::completeOrder()
{
    if (cartItems.isEmpty()) return;

    currentOrderNum = generateOrderNumber();
    QString orderDate = QDate::currentDate().toString("yyyy-MM-dd");
    QString orderTime = QTime::currentTime().toString("HH:mm:ss");

    if (DatabaseManager::instance().isConnected()) {
        DatabaseManager::instance().insertOrder(
            m_username, orderDate, orderTime, currentOrderTotal, "completed");
    }

    receiptOrderNum->setText("Order ID: " + currentOrderNum);
    receiptDateTime->setText(QDateTime::currentDateTime().toString("MMMM d, yyyy 'at' hh:mm AP"));
    receiptServedBy->setText("Served by: " + (m_username.isEmpty() ? "John Doe" : m_username));

    receiptTable->setRowCount(0);
    double subtotal = 0.0;
    int row = 0;

    for (auto i = cartItems.begin(); i != cartItems.end(); ++i) {
        receiptTable->insertRow(row);
        QString name    = i.key();
        int     qty     = i.value();
        double  price   = itemPrices[name];
        double  itemTotal = price * qty;
        subtotal += itemTotal;

        QTableWidgetItem *nIt = new QTableWidgetItem(name);
        nIt->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QTableWidgetItem *qIt = new QTableWidgetItem(QString::number(qty));
        qIt->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *pIt = new QTableWidgetItem(QString("$%1").arg(price, 0, 'f', 2));
        pIt->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        QTableWidgetItem *tIt = new QTableWidgetItem(QString("$%1").arg(itemTotal, 0, 'f', 2));
        tIt->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QFont boldF; boldF.setBold(true); tIt->setFont(boldF);

        receiptTable->setItem(row, 0, nIt);
        receiptTable->setItem(row, 1, qIt);
        receiptTable->setItem(row, 2, pIt);
        receiptTable->setItem(row, 3, tIt);
        row++;
    }

    int totalHeight = 45 + (row * 45) + 10;
    receiptTable->setFixedHeight(totalHeight);
    receiptTable->setMinimumHeight(totalHeight);
    receiptTable->setMaximumHeight(totalHeight);

    double tax = subtotal * 0.08;
    receiptSubtotal->setText(QString("$%1").arg(subtotal, 0, 'f', 2));
    receiptTax->setText(QString("$%1").arg(tax, 0, 'f', 2));
    receiptTotal->setText(QString("$%1").arg(subtotal + tax, 0, 'f', 2));

    mainStack->setCurrentIndex(1);
}

// =====================================================
// Print Receipt
// =====================================================

void POSWindow::printReceipt()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);

    if (dialog.exec() == QDialog::Accepted) {
        QString rowsHtml = "";
        for (int i = 0; i < receiptTable->rowCount(); ++i) {
            rowsHtml += QString(
                            "<tr><td>%1</td><td align='center'>%2</td>"
                            "<td align='right'>%3</td><td align='right'><b>%4</b></td></tr>")
                            .arg(receiptTable->item(i,0)->text(),
                                 receiptTable->item(i,1)->text(),
                                 receiptTable->item(i,2)->text(),
                                 receiptTable->item(i,3)->text());
        }

        QString html = QString(R"(
            <html>
            <body style='font-family: "Segoe UI", sans-serif; color: #333;'>
                <h2 align='center' style='color: #5D3A1A;'>Café Manager</h2>
                <p align='center' style='font-size: 12px; color: #666;'>
                    123 Coffee Street<br>Tel: (555) 123-4567</p>
                <hr style='border: 1px dashed #CCC;'>
                <p align='center'><b>%1</b><br>%2<br>%3</p>
                <hr style='border: 1px dashed #CCC;'>
                <table width='100%%' cellspacing='0' cellpadding='5' style='font-size: 12px;'>
                    <tr style='background:#FAF6F1;'>
                        <th align='left'>Item</th>
                        <th align='center'>Qty</th>
                        <th align='right'>Price</th>
                        <th align='right'>Total</th>
                    </tr>
                    %4
                </table>
                <hr style='border: 1px dashed #CCC;'>
                <table width='100%%' style='font-size: 12px;'>
                    <tr><td>Subtotal</td><td align='right'>%5</td></tr>
                    <tr><td>Tax (8%%)</td><td align='right'>%6</td></tr>
                    <tr><td><b>Total</b></td>
                        <td align='right'><b style='color:#E67E22; font-size:16px;'>%7</b></td></tr>
                </table>
                <hr style='border: 1px dashed #CCC;'>
                <p align='center'><b>Thank you for your visit!</b></p>
            </body>
            </html>
        )").arg(receiptOrderNum->text(),
                                receiptDateTime->text(),
                                receiptServedBy->text(),
                                rowsHtml,
                                receiptSubtotal->text(),
                                receiptTax->text(),
                                receiptTotal->text());

        QTextDocument doc;
        doc.setHtml(html);
        doc.print(&printer);
    }
}

// =====================================================
// Logout
// =====================================================

void POSWindow::onLogout()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Logout");
    msgBox.setText("Are you sure you want to logout?");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setStyleSheet(R"(
        QMessageBox { background-color: white; }
        QMessageBox QLabel {
            color: #1A1A1A; font-size: 14px;
            background-color: white; min-width: 320px; padding: 10px;
        }
        QMessageBox QPushButton {
            background-color: #E67E22; color: white;
            border: none; border-radius: 6px;
            padding: 10px 24px; font-size: 13px;
            font-weight: bold; min-width: 90px;
        }
        QMessageBox QPushButton:hover { background-color: #D35400; }
    )");

    if (msgBox.exec() == QMessageBox::Yes) {
        this->close();
    }
}