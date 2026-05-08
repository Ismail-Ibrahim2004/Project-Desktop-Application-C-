#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFrame>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QFileInfo>
#include <QStackedWidget>
#include <QTextEdit>
#include <QVariant>
#include <QAbstractItemView>
#include <QEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , isEditing(false)
    , editingId(-1)
{
    setupDB();
    setupUI();
    showProducts();
}

MainWindow::~MainWindow()
{
    db.close();
}

void MainWindow::setupDB()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("cafe_manager.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", "Failed to open database: " + db.lastError().text());
        return;
    }

    QSqlQuery q;
    q.exec("CREATE TABLE IF NOT EXISTS categories (id INTEGER PRIMARY KEY, name TEXT)");
    q.exec("CREATE TABLE IF NOT EXISTS products ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
           "product_id TEXT UNIQUE, "
           "name TEXT NOT NULL, "
           "category TEXT, "
           "price REAL, "
           "description TEXT, "
           "image TEXT, "
           "status TEXT DEFAULT 'active')");

    QStringList cats;
    cats << "Coffee" << "Tea" << "Desserts" << "Cold Drinks";
    for (int i = 0; i < cats.size(); ++i) {
        q.prepare("INSERT OR IGNORE INTO categories (name) VALUES (?)");
        q.addBindValue(cats.at(i));
        q.exec();
    }

    QList<QVariantList> prods;
    QVariantList p1; p1 << "PRD-001" << "Espresso" << "Coffee" << 3.50 << "Rich and bold single shot espresso" << "active"; prods.append(p1);
    QVariantList p2; p2 << "PRD-002" << "Cappuccino" << "Coffee" << 4.25 << "A rich and creamy espresso-based coffee drink" << "active"; prods.append(p2);
    QVariantList p3; p3 << "PRD-003" << "Latte" << "Coffee" << 4.50 << "Smooth espresso with steamed milk" << "active"; prods.append(p3);
    QVariantList p4; p4 << "PRD-004" << "Americano" << "Coffee" << 3.75 << "Espresso diluted with hot water" << "active"; prods.append(p4);
    QVariantList p5; p5 << "PRD-005" << "Green Tea" << "Tea" << 2.50 << "Refreshing green tea" << "active"; prods.append(p5);
    QVariantList p6; p6 << "PRD-006" << "Earl Grey" << "Tea" << 2.75 << "Classic bergamot flavored black tea" << "active"; prods.append(p6);
    QVariantList p7; p7 << "PRD-007" << "Chocolate Cake" << "Desserts" << 5.50 << "Decadent chocolate layer cake" << "active"; prods.append(p7);
    QVariantList p8; p8 << "PRD-008" << "Cheesecake" << "Desserts" << 6.00 << "Creamy New York style cheesecake" << "active"; prods.append(p8);
    QVariantList p9; p9 << "PRD-009" << "Iced Latte" << "Cold Drinks" << 5.00 << "Chilled espresso with milk over ice" << "active"; prods.append(p9);
    QVariantList p10; p10 << "PRD-010" << "Frappe" << "Cold Drinks" << 5.50 << "Blended iced coffee drink" << "inactive"; prods.append(p10);

    for (int i = 0; i < prods.size(); ++i) {
        q.prepare("INSERT OR IGNORE INTO products (product_id, name, category, price, description, status) VALUES (?,?,?,?,?,?)");
        q.addBindValue(prods.at(i).at(0));
        q.addBindValue(prods.at(i).at(1));
        q.addBindValue(prods.at(i).at(2));
        q.addBindValue(prods.at(i).at(3));
        q.addBindValue(prods.at(i).at(4));
        q.addBindValue(prods.at(i).at(5));
        q.exec();
    }
}

void MainWindow::setupUI()
{
    setWindowTitle("Cafe Manager");
    resize(1200, 800);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ===== SIDEBAR =====
    QWidget *sidebar = new QWidget();
    sidebar->setFixedWidth(260);
    sidebar->setStyleSheet("background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #6B4423, stop:1 #5D3A1A);");
    QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(0);

    // Logo
    QWidget *logoWidget = new QWidget();
    logoWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *logoLayout = new QHBoxLayout(logoWidget);
    logoLayout->setContentsMargins(20, 20, 20, 20);
    QLabel *logoIcon = new QLabel("C");
    logoIcon->setFixedSize(36, 36);
    logoIcon->setAlignment(Qt::AlignCenter);
    logoIcon->setStyleSheet("font-size: 18px; color: white; background: #E67E22; border-radius: 8px; font-weight: bold;");
    QVBoxLayout *logoTextLayout = new QVBoxLayout();
    logoTextLayout->setSpacing(2);
    QLabel *logoTitle = new QLabel("Cafe Manager");
    logoTitle->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    QLabel *logoSub = new QLabel("Admin");
    logoSub->setStyleSheet("color: rgba(255,255,255,0.6); font-size: 12px;");
    logoTextLayout->addWidget(logoTitle);
    logoTextLayout->addWidget(logoSub);
    logoLayout->addWidget(logoIcon);
    logoLayout->addLayout(logoTextLayout);
    logoLayout->addStretch();
    sideLayout->addWidget(logoWidget);

    QWidget *sep = new QWidget();
    sep->setFixedHeight(1);
    sep->setStyleSheet("background: rgba(255,255,255,0.1);");
    sideLayout->addWidget(sep);

    // Nav items
    QStringList navItems;
    navItems << "Dashboard" << "Products" << "Orders" << "Inventory" << "Employees" << "Reports" << "Settings";
    for (int i = 0; i < navItems.size(); ++i) {
        QPushButton *btn = new QPushButton(navItems.at(i));
        btn->setCursor(Qt::PointingHandCursor);
        if (navItems.at(i) == "Products") {
            btn->setStyleSheet(
                "QPushButton {"
                "  background: #E67E22;"
                "  color: white;"
                "  border: none;"
                "  text-align: left;"
                "  padding: 14px 24px;"
                "  font-size: 14px;"
                "  margin: 2px 12px;"
                "  border-radius: 8px;"
                "}"
            );
            connect(btn, SIGNAL(clicked()), this, SLOT(showProducts()));
        } else {
            btn->setStyleSheet(
                "QPushButton {"
                "  background: transparent;"
                "  color: rgba(255,255,255,0.85);"
                "  border: none;"
                "  text-align: left;"
                "  padding: 14px 24px;"
                "  font-size: 14px;"
                "  margin: 2px 12px;"
                "  border-radius: 8px;"
                "}"
                "QPushButton:hover {"
                "  background: rgba(255,255,255,0.1);"
                "}"
            );
        }
        sideLayout->addWidget(btn);
    }

    sideLayout->addStretch();

    QPushButton *logoutBtn = new QPushButton("Logout");
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setStyleSheet(
        "QPushButton {"
        "  background: transparent;"
        "  color: rgba(255,255,255,0.7);"
        "  border: none;"
        "  text-align: left;"
        "  padding: 14px 24px;"
        "  font-size: 14px;"
        "  margin: 2px 12px 16px 12px;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "  background: rgba(255,255,255,0.1);"
        "}"
    );
    sideLayout->addWidget(logoutBtn);

    mainLayout->addWidget(sidebar);

    // ===== RIGHT AREA =====
    QWidget *rightArea = new QWidget();
    rightArea->setStyleSheet("background: #F5F0E8;");
    QVBoxLayout *rightLayout = new QVBoxLayout(rightArea);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // Header
    QWidget *header = new QWidget();
    header->setFixedHeight(70);
    header->setStyleSheet("background: white; border-bottom: 1px solid #E5E0D8;");
    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(32, 0, 32, 0);

    QVBoxLayout *titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(2);
    pageTitle = new QLabel("Products Management");
    pageTitle->setStyleSheet("color: #5D3A1A; font-size: 20px; font-weight: bold;");
    pageSubtitle = new QLabel("Manage your cafe menu items");
    pageSubtitle->setStyleSheet("color: #888; font-size: 13px;");
    titleLayout->addWidget(pageTitle);
    titleLayout->addWidget(pageSubtitle);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    rightLayout->addWidget(header);

    // Stack
    stack = new QStackedWidget();
    setupProductsPage();
    setupFormPage();
    rightLayout->addWidget(stack);

    mainLayout->addWidget(rightArea);
}

void MainWindow::setupProductsPage()
{
    productsPage = new QWidget();
    productsPage->setStyleSheet("background: #F5F0E8;");
    QVBoxLayout *layout = new QVBoxLayout(productsPage);
    layout->setContentsMargins(32, 24, 32, 24);
    layout->setSpacing(20);

    // Top bar
    QWidget *topBar = new QWidget();
    topBar->setStyleSheet("background: transparent;");
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);

    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Search products...");
    searchEdit->setStyleSheet(
        "QLineEdit {"
        "  background: white;"
        "  border: 1px solid #D4C5B0;"
        "  border-radius: 8px;"
        "  padding: 10px 14px;"
        "  font-size: 13px;"
        "  color: #5D3A1A;"
        "}"
        "QLineEdit:focus {"
        "  border: 1px solid #E67E22;"
        "}"
    );
    searchEdit->setFixedWidth(400);
    connect(searchEdit, SIGNAL(textChanged(QString)), this, SLOT(searchProducts()));

    categoryFilter = new QComboBox();
    categoryFilter->addItem("All Categories");
    categoryFilter->addItems(QStringList() << "Coffee" << "Tea" << "Desserts" << "Cold Drinks");
    categoryFilter->setStyleSheet(
        "QComboBox {"
        "  background: white;"
        "  border: 1px solid #D4C5B0;"
        "  border-radius: 8px;"
        "  padding: 10px 14px;"
        "  font-size: 13px;"
        "  color: #5D3A1A;"
        "  min-width: 140px;"
        "}"
        "QComboBox:focus {"
        "  border: 1px solid #E67E22;"
        "}"
        "QComboBox::drop-down {"
        "  border: none;"
        "  width: 30px;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background: white;"
        "  border: 1px solid #D4C5B0;"
        "  selection-background-color: #F5F0E8;"
        "  selection-color: #5D3A1A;"
        "}"
    );
    connect(categoryFilter, SIGNAL(currentIndexChanged(int)), this, SLOT(filterCategory()));

    topLayout->addWidget(searchEdit);
    topLayout->addWidget(categoryFilter);
    topLayout->addStretch();

    QPushButton *addBtn = new QPushButton("+  Add Product");
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet(
        "QPushButton {"
        "  background: #E67E22;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 8px;"
        "  padding: 10px 20px;"
        "  font-size: 13px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "  background: #D35400;"
        "}"
    );
    connect(addBtn, SIGNAL(clicked()), this, SLOT(showAddProduct()));
    topLayout->addWidget(addBtn);

    layout->addWidget(topBar);

    // Table card
    QFrame *tableCard = new QFrame();
    tableCard->setStyleSheet(
        "QFrame {"
        "  background: white;"
        "  border: 1px solid #E5E0D8;"
        "  border-radius: 12px;"
        "}"
    );
    tableCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *cardLayout = new QVBoxLayout(tableCard);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

    QLabel *tableTitle = new QLabel("All Products");
    tableTitle->setStyleSheet("color: #5D3A1A; font-size: 14px; font-weight: 600; padding: 16px 24px;");
    cardLayout->addWidget(tableTitle);

    productTable = new QTableWidget();
    productTable->setColumnCount(6);
    QStringList headers;
    headers << "Product ID" << "Product Name" << "Category" << "Price" << "Status" << "Actions";
    productTable->setHorizontalHeaderLabels(headers);
    productTable->setStyleSheet(
        "QTableWidget {"
        "  background: white;"
        "  border: none;"
        "  gridline-color: transparent;"
        "  outline: none;"
        "}"
        "QTableWidget::item {"
        "  border-bottom: 1px solid #F0EDE6;"
        "  padding: 12px 24px;"
        "  color: #5D3A1A;"
        "  font-size: 13px;"
        "}"
        "QTableWidget::item:selected {"
        "  background: transparent;"
        "  color: #5D3A1A;"
        "}"
        "QHeaderView::section {"
        "  background: white;"
        "  color: #999;"
        "  font-weight: 600;"
        "  font-size: 11px;"
        "  border: none;"
        "  padding: 14px 24px;"
        "}"
    );
    productTable->horizontalHeader()->setStretchLastSection(true);
    productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    productTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    productTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    productTable->setColumnWidth(0, 100);
    productTable->setColumnWidth(5, 120);
    productTable->verticalHeader()->setVisible(false);
    productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    productTable->setSelectionMode(QAbstractItemView::SingleSelection);
    productTable->setShowGrid(false);
    productTable->setFocusPolicy(Qt::NoFocus);

    cardLayout->addWidget(productTable);
    layout->addWidget(tableCard);

    stack->addWidget(productsPage);
}

void MainWindow::setupFormPage()
{
    formPage = new QWidget();
    formPage->setStyleSheet("background: #F5F0E8;");
    QVBoxLayout *layout = new QVBoxLayout(formPage);
    layout->setContentsMargins(32, 24, 32, 24);
    layout->setSpacing(16);

    QPushButton *backBtn = new QPushButton("<-  Back to Products");
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet(
        "QPushButton {"
        "  background: transparent;"
        "  color: #888;"
        "  border: none;"
        "  text-align: left;"
        "  font-size: 13px;"
        "  padding: 0;"
        "}"
        "QPushButton:hover {"
        "  color: #5D3A1A;"
        "}"
    );
    connect(backBtn, SIGNAL(clicked()), this, SLOT(goBack()));
    layout->addWidget(backBtn);

    QFrame *formCard = new QFrame();
    formCard->setStyleSheet(
        "QFrame {"
        "  background: white;"
        "  border: 1px solid #E5E0D8;"
        "  border-radius: 12px;"
        "}"
    );
    QVBoxLayout *formLayout = new QVBoxLayout(formCard);
    formLayout->setContentsMargins(32, 28, 32, 28);
    formLayout->setSpacing(16);

    formTitle = new QLabel("Add New Product");
    formTitle->setStyleSheet("color: #5D3A1A; font-size: 22px; font-weight: bold;");
    formSubtitle = new QLabel("Add a new item to your menu");
    formSubtitle->setStyleSheet("color: #888; font-size: 13px;");
    formLayout->addWidget(formTitle);
    formLayout->addWidget(formSubtitle);

    QLabel *infoTitle = new QLabel("Product Information");
    infoTitle->setStyleSheet("color: #5D3A1A; font-size: 14px; font-weight: 600; margin-top: 8px;");
    formLayout->addWidget(infoTitle);

    // Name
    QLabel *nameLabel = new QLabel("Product Name *");
    nameLabel->setStyleSheet("color: #5D3A1A; font-size: 13px; font-weight: 500;");
    formLayout->addWidget(nameLabel);
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("e.g., Cappuccino");
    nameEdit->setStyleSheet(
        "QLineEdit {"
        "  background: #F5F0E8;"
        "  border: 1px solid #D4C5B0;"
        "  border-radius: 8px;"
        "  padding: 10px 14px;"
        "  font-size: 13px;"
        "  color: #5D3A1A;"
        "}"
        "QLineEdit:focus {"
        "  border: 1px solid #E67E22;"
        "}"
    );
    formLayout->addWidget(nameEdit);

    // Category
    QLabel *catLabel = new QLabel("Category *");
    catLabel->setStyleSheet("color: #5D3A1A; font-size: 13px; font-weight: 500;");
    formLayout->addWidget(catLabel);
    categoryEdit = new QComboBox();
    categoryEdit->addItem("Select category");
    categoryEdit->addItems(QStringList() << "Coffee" << "Tea" << "Desserts" << "Cold Drinks");
    categoryEdit->setStyleSheet(
        "QComboBox {"
        "  background: #F5F0E8;"
        "  border: 1px solid #D4C5B0;"
        "  border-radius: 8px;"
        "  padding: 10px 14px;"
        "  font-size: 13px;"
        "  color: #5D3A1A;"
        "}"
        "QComboBox:focus {"
        "  border: 1px solid #E67E22;"
        "}"
    );
    formLayout->addWidget(categoryEdit);

    // Price
    QLabel *priceLabel = new QLabel("Price ($) *");
    priceLabel->setStyleSheet("color: #5D3A1A; font-size: 13px; font-weight: 500;");
    formLayout->addWidget(priceLabel);
    priceEdit = new QLineEdit();
    priceEdit->setPlaceholderText("0.00");
    priceEdit->setStyleSheet(
        "QLineEdit {"
        "  background: #F5F0E8;"
        "  border: 1px solid #D4C5B0;"
        "  border-radius: 8px;"
        "  padding: 10px 14px;"
        "  font-size: 13px;"
        "  color: #5D3A1A;"
        "}"
        "QLineEdit:focus {"
        "  border: 1px solid #E67E22;"
        "}"
    );
    formLayout->addWidget(priceEdit);

    // Description
    QLabel *descLabel = new QLabel("Description");
    descLabel->setStyleSheet("color: #5D3A1A; font-size: 13px; font-weight: 500;");
    formLayout->addWidget(descLabel);
    descEdit = new QTextEdit();
    descEdit->setPlaceholderText("Enter product description...");
    descEdit->setMaximumHeight(80);
    descEdit->setStyleSheet(
        "QTextEdit {"
        "  background: #F5F0E8;"
        "  border: 1px solid #D4C5B0;"
        "  border-radius: 8px;"
        "  padding: 10px 14px;"
        "  font-size: 13px;"
        "  color: #5D3A1A;"
        "}"
        "QTextEdit:focus {"
        "  border: 1px solid #E67E22;"
        "}"
    );
    formLayout->addWidget(descEdit);

    // Status
    QLabel *statusLabel = new QLabel("Status");
    statusLabel->setStyleSheet("color: #5D3A1A; font-size: 13px; font-weight: 500;");
    formLayout->addWidget(statusLabel);
    statusEdit = new QComboBox();
    statusEdit->addItems(QStringList() << "active" << "inactive");
    statusEdit->setStyleSheet(
        "QComboBox {"
        "  background: #F5F0E8;"
        "  border: 1px solid #D4C5B0;"
        "  border-radius: 8px;"
        "  padding: 10px 14px;"
        "  font-size: 13px;"
        "  color: #5D3A1A;"
        "}"
    );
    formLayout->addWidget(statusEdit);

    // Image
    QLabel *imgLabel = new QLabel("Product Image");
    imgLabel->setStyleSheet("color: #5D3A1A; font-size: 13px; font-weight: 500;");
    formLayout->addWidget(imgLabel);

    imageLabel = new QLabel();
    imageLabel->setFixedHeight(140);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet(
        "QLabel {"
        "  background: #F5F0E8;"
        "  border: 2px dashed #D4C5B0;"
        "  border-radius: 12px;"
        "  color: #888;"
        "  font-size: 12px;"
        "}"
    );
    imageLabel->setText("Click to upload image\nPNG, JPG, GIF up to 10MB");
    imageLabel->setCursor(Qt::PointingHandCursor);
    imageLabel->setWordWrap(true);
    imageLabel->installEventFilter(this);
    formLayout->addWidget(imageLabel);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    saveBtn = new QPushButton("Add Product");
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setStyleSheet(
        "QPushButton {"
        "  background: #6B4423;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 8px;"
        "  padding: 10px 24px;"
        "  font-size: 13px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "  background: #5D3A1A;"
        "}"
    );
    connect(saveBtn, SIGNAL(clicked()), this, SLOT(saveProduct()));

    QPushButton *cancelBtn = new QPushButton("Cancel");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setStyleSheet(
        "QPushButton {"
        "  background: white;"
        "  color: #5D3A1A;"
        "  border: 1px solid #D4C5B0;"
        "  border-radius: 8px;"
        "  padding: 10px 24px;"
        "  font-size: 13px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "  background: #F5F0E8;"
        "}"
    );
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(goBack()));

    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch();
    formLayout->addLayout(btnLayout);

    layout->addWidget(formCard);
    layout->addStretch();

    stack->addWidget(formPage);
}

void MainWindow::loadProducts()
{
    productTable->setRowCount(0);

    QString search = searchEdit->text().trimmed();
    QString cat = categoryFilter->currentText();

    QString sql = "SELECT id, product_id, name, category, price, status FROM products WHERE 1=1";
    QStringList conditions;
    QVariantList values;

    if (!search.isEmpty()) {
        conditions << "(name LIKE ? OR product_id LIKE ?)";
        values << QVariant("%" + search + "%");
        values << QVariant("%" + search + "%");
    }
    if (cat != "All Categories") {
        conditions << "category = ?";
        values << QVariant(cat);
    }
    for (int i = 0; i < conditions.size(); ++i) {
        sql += " AND " + conditions.at(i);
    }
    sql += " ORDER BY id";

    QSqlQuery q;
    q.prepare(sql);
    for (int i = 0; i < values.size(); ++i) {
        q.addBindValue(values.at(i));
    }
    q.exec();

    int row = 0;
    while (q.next()) {
        productTable->insertRow(row);
        int id = q.value(0).toInt();

        QTableWidgetItem *item0 = new QTableWidgetItem(q.value(1).toString());
        QTableWidgetItem *item1 = new QTableWidgetItem(q.value(2).toString());
        QTableWidgetItem *item2 = new QTableWidgetItem(q.value(3).toString());
        QTableWidgetItem *item3 = new QTableWidgetItem(QString("$%1").arg(q.value(4).toDouble(), 0, 'f', 2));
        QTableWidgetItem *item4 = new QTableWidgetItem(q.value(5).toString());

        item0->setFlags(item0->flags() & ~Qt::ItemIsEditable);
        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
        item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
        item3->setFlags(item3->flags() & ~Qt::ItemIsEditable);
        item4->setFlags(item4->flags() & ~Qt::ItemIsEditable);

        QString status = q.value(5).toString();
        if (status == "active") {
            item4->setBackground(QColor("#D4EDDA"));
            item4->setForeground(QColor("#155724"));
        } else {
            item4->setBackground(QColor("#E2E3E5"));
            item4->setForeground(QColor("#383D41"));
        }

        productTable->setItem(row, 0, item0);
        productTable->setItem(row, 1, item1);
        productTable->setItem(row, 2, item2);
        productTable->setItem(row, 3, item3);
        productTable->setItem(row, 4, item4);

        // Actions
        QWidget *actionWidget = new QWidget();
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 12, 0);
        actionLayout->setSpacing(8);
        actionLayout->addStretch();

        QPushButton *editBtn = new QPushButton("Edit");
        editBtn->setFixedSize(50, 28);
        editBtn->setCursor(Qt::PointingHandCursor);
        editBtn->setStyleSheet(
            "QPushButton {"
            "  background: transparent;"
            "  color: #bbb;"
            "  border: none;"
            "  font-size: 12px;"
            "}"
            "QPushButton:hover {"
            "  color: #E67E22;"
            "}"
        );
        connect(editBtn, SIGNAL(clicked()), this, SLOT(showEditProduct()));
        editBtn->setProperty("productId", id);

        QPushButton *delBtn = new QPushButton("Del");
        delBtn->setFixedSize(50, 28);
        delBtn->setCursor(Qt::PointingHandCursor);
        delBtn->setStyleSheet(
            "QPushButton {"
            "  background: transparent;"
            "  color: #bbb;"
            "  border: none;"
            "  font-size: 12px;"
            "}"
            "QPushButton:hover {"
            "  color: #e74c3c;"
            "}"
        );
        connect(delBtn, SIGNAL(clicked()), this, SLOT(showEditProduct()));
        delBtn->setProperty("productId", id);
        delBtn->setProperty("deleteMode", true);

        actionLayout->addWidget(editBtn);
        actionLayout->addWidget(delBtn);
        productTable->setCellWidget(row, 5, actionWidget);

        row++;
    }
}

void MainWindow::showProducts()
{
    pageTitle->setText("Products Management");
    pageSubtitle->setText("Manage your cafe menu items");
    stack->setCurrentWidget(productsPage);
    loadProducts();
}

void MainWindow::showAddProduct()
{
    isEditing = false;
    editingId = -1;
    clearForm();
    formTitle->setText("Add New Product");
    formSubtitle->setText("Add a new item to your menu");
    saveBtn->setText("Add Product");
    pageTitle->setText("Add New Product");
    pageSubtitle->setText("Add a new item to your menu");
    stack->setCurrentWidget(formPage);
}

void MainWindow::showEditProduct()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    int id = btn->property("productId").toInt();
    bool delMode = btn->property("deleteMode").toBool();

    if (delMode) {
        if (QMessageBox::question(this, "Confirm", "Delete this product?") == QMessageBox::Yes) {
            QSqlQuery q;
            q.prepare("DELETE FROM products WHERE id = ?");
            q.addBindValue(id);
            if (q.exec()) {
                loadProducts();
            }
        }
        return;
    }

    isEditing = true;
    editingId = id;
    QSqlQuery q;
    q.prepare("SELECT product_id, name, category, price, description, image, status FROM products WHERE id = ?");
    q.addBindValue(editingId);
    q.exec();
    if (!q.next()) return;

    nameEdit->setText(q.value(1).toString());
    categoryEdit->setCurrentText(q.value(2).toString());
    priceEdit->setText(QString::number(q.value(3).toDouble(), 'f', 2));
    descEdit->setPlainText(q.value(4).toString());
    statusEdit->setCurrentText(q.value(6).toString());
    currentImagePath = q.value(5).toString();

    if (!currentImagePath.isEmpty()) {
        QFileInfo fi(currentImagePath);
        imageLabel->setText("Selected: " + fi.fileName());
        imageLabel->setStyleSheet(
            "QLabel {"
            "  background: #F5F0E8;"
            "  border: 2px solid #E67E22;"
            "  border-radius: 12px;"
            "  color: #5D3A1A;"
            "  font-size: 12px;"
            "}"
        );
    } else {
        imageLabel->setText("Click to upload image\nPNG, JPG, GIF up to 10MB");
        imageLabel->setStyleSheet(
            "QLabel {"
            "  background: #F5F0E8;"
            "  border: 2px dashed #D4C5B0;"
            "  border-radius: 12px;"
            "  color: #888;"
            "  font-size: 12px;"
            "}"
        );
    }

    formTitle->setText("Edit Product");
    formSubtitle->setText("Update product information");
    saveBtn->setText("Update Product");
    pageTitle->setText("Edit Product");
    pageSubtitle->setText("Update product information");
    stack->setCurrentWidget(formPage);
}

void MainWindow::clearForm()
{
    nameEdit->clear();
    categoryEdit->setCurrentIndex(0);
    priceEdit->clear();
    descEdit->clear();
    statusEdit->setCurrentIndex(0);
    currentImagePath.clear();
    imageLabel->setText("Click to upload image\nPNG, JPG, GIF up to 10MB");
    imageLabel->setStyleSheet(
        "QLabel {"
        "  background: #F5F0E8;"
        "  border: 2px dashed #D4C5B0;"
        "  border-radius: 12px;"
        "  color: #888;"
        "  font-size: 12px;"
        "}"
    );
}

void MainWindow::saveProduct()
{
    QString name = nameEdit->text().trimmed();
    QString cat = categoryEdit->currentText();
    double price = priceEdit->text().toDouble();
    QString desc = descEdit->toPlainText().trimmed();
    QString status = statusEdit->currentText();

    if (name.isEmpty() || cat == "Select category" || priceEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Please fill in all required fields.");
        return;
    }

    QSqlQuery q;
    if (isEditing) {
        q.prepare("UPDATE products SET name=?, category=?, price=?, description=?, image=?, status=? WHERE id=?");
        q.addBindValue(name);
        q.addBindValue(cat);
        q.addBindValue(price);
        q.addBindValue(desc);
        q.addBindValue(currentImagePath);
        q.addBindValue(status);
        q.addBindValue(editingId);
    } else {
        QString pid = generateProductId();
        q.prepare("INSERT INTO products (product_id, name, category, price, description, image, status) VALUES (?,?,?,?,?,?,?)");
        q.addBindValue(pid);
        q.addBindValue(name);
        q.addBindValue(cat);
        q.addBindValue(price);
        q.addBindValue(desc);
        q.addBindValue(currentImagePath);
        q.addBindValue(status);
    }

    if (q.exec()) {
        showProducts();
    } else {
        QMessageBox::critical(this, "Error", q.lastError().text());
    }
}

QString MainWindow::generateProductId()
{
    QSqlQuery q("SELECT product_id FROM products ORDER BY id DESC LIMIT 1");
    if (!q.exec() || !q.next()) return "PRD-001";
    QString last = q.value(0).toString();
    int num = last.mid(4).toInt() + 1;
    return QString("PRD-%1").arg(num, 3, 10, QChar('0'));
}

void MainWindow::chooseImage()
{
    QString path = QFileDialog::getOpenFileName(this, "Select Image", QDir::homePath(), "Images (*.png *.jpg *.jpeg *.gif)");
    if (!path.isEmpty()) {
        currentImagePath = path;
        QFileInfo fi(path);
        imageLabel->setText("Selected: " + fi.fileName());
        imageLabel->setStyleSheet(
            "QLabel {"
            "  background: #F5F0E8;"
            "  border: 2px solid #E67E22;"
            "  border-radius: 12px;"
            "  color: #5D3A1A;"
            "  font-size: 12px;"
            "}"
        );
    }
}

void MainWindow::goBack()
{
    showProducts();
}

void MainWindow::searchProducts()
{
    loadProducts();
}

void MainWindow::filterCategory()
{
    loadProducts();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == imageLabel && event->type() == QEvent::MouseButtonPress) {
        chooseImage();
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}
