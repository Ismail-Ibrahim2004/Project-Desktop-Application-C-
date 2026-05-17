#include "productspage.h"
#include "../database/databasemanager.h"

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
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDir>
#include <QFileInfo>
#include <QStackedWidget>
#include <QTextEdit>
#include <QVariant>
#include <QAbstractItemView>
#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QScrollArea>
#include <QFormLayout>

ProductsPage::ProductsPage(QWidget *parent)
    : QWidget(parent),
    isEditing(false),
    editingId(-1)
{
    setupUI();
    showProductsList();
}

ProductsPage::~ProductsPage() {}

void ProductsPage::refreshData()
{
    loadProducts();
}

// =====================================================
// Setup UI
// =====================================================

void ProductsPage::setupUI()
{
    setStyleSheet("background: #FAF6F1;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    stack = new QStackedWidget();
    setupProductsPage();
    setupFormPage();

    mainLayout->addWidget(stack);

    pageTitle = new QLabel(this);
    pageSubtitle = new QLabel(this);
    pageTitle->hide();
    pageSubtitle->hide();
}

// =====================================================
// Products List Page
// =====================================================

void ProductsPage::setupProductsPage()
{
    productsPage = new QWidget();
    productsPage->setStyleSheet("background: #FAF6F1;");

    QVBoxLayout *layout = new QVBoxLayout(productsPage);
    layout->setContentsMargins(32, 24, 32, 24);
    layout->setSpacing(20);

    QLabel *headerTitle = new QLabel("Products Management");
    headerTitle->setStyleSheet("color: #1A1A1A; font-size: 28px; font-weight: bold; background: transparent;");

    QLabel *headerSub = new QLabel("Manage your cafe menu items");
    headerSub->setStyleSheet("color: #888; font-size: 14px; background: transparent;");

    layout->addWidget(headerTitle);
    layout->addWidget(headerSub);
    layout->addSpacing(10);

    // ===== Top bar =====
    QWidget *topBar = new QWidget();
    topBar->setStyleSheet("background: transparent;");
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(12);

    // Search with icon from resources
    QWidget *searchWrapper = new QWidget();
    searchWrapper->setFixedHeight(42);
    searchWrapper->setFixedWidth(400);
    searchWrapper->setStyleSheet(R"(
        QWidget {
            background: white;
            border: 1px solid #D4C5B0;
            border-radius: 8px;
        }
    )");
    QHBoxLayout *searchWrapLayout = new QHBoxLayout(searchWrapper);
    searchWrapLayout->setContentsMargins(10, 0, 10, 0);
    searchWrapLayout->setSpacing(8);

    QLabel *searchIconLabel = new QLabel();
    searchIconLabel->setFixedSize(18, 18);
    searchIconLabel->setPixmap(
        QPixmap(":/icons/search-interface-symbol.png")
            .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    searchIconLabel->setStyleSheet("background: transparent; border: none;");

    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Search products...");
    searchEdit->setStyleSheet(R"(
        QLineEdit {
            background: transparent;
            border: none;
            font-size: 13px;
            color: #5D3A1A;
        }
    )");
    connect(searchEdit, &QLineEdit::textChanged, this, &ProductsPage::searchProducts);

    searchWrapLayout->addWidget(searchIconLabel);
    searchWrapLayout->addWidget(searchEdit);

    categoryFilter = new QComboBox();
    categoryFilter->setFixedHeight(42);
    categoryFilter->addItem("All Categories");
    categoryFilter->addItems(QStringList() << "Coffee" << "Tea" << "Desserts" << "Cold Drinks" << "Beverages" << "Pastry" << "Dessert");
    categoryFilter->setStyleSheet(R"(
        QComboBox {
            background: white;
            border: 1px solid #D4C5B0;
            border-radius: 8px;
            padding-left: 14px;
            padding-right: 14px;
            font-size: 13px;
            color: #5D3A1A;
            min-width: 180px;
        }
        QComboBox:focus { border: 1px solid #E67E22; }
        QComboBox::drop-down { border: none; width: 30px; }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #888;
            margin-right: 10px;
        }
    )");
    categoryFilter->view()->setStyleSheet(R"(
        QAbstractItemView {
            background-color: white;
            border: 1px solid #D4C5B0;
            border-radius: 6px;
            outline: none;
            padding: 4px;
            color: #5D3A1A;
        }
        QAbstractItemView::item {
            background-color: white;
            color: #5D3A1A;
            padding: 8px 14px;
            min-height: 28px;
            border: none;
        }
        QAbstractItemView::item:hover { background-color: #FAF6F1; color: #5D3A1A; }
        QAbstractItemView::item:selected { background-color: #E67E22; color: white; }
    )");
    connect(categoryFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ProductsPage::filterCategory);

    topLayout->addWidget(searchWrapper);
    topLayout->addWidget(categoryFilter);
    topLayout->addStretch();

    QPushButton *addBtn = new QPushButton("  Add Product");
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setFixedHeight(42);
    addBtn->setIcon(QIcon(":/icons/plus.png"));
    addBtn->setIconSize(QSize(16, 16));
    addBtn->setStyleSheet(R"(
        QPushButton {
            background: #E67E22;
            color: white;
            border: none;
            border-radius: 8px;
            padding-left: 24px;
            padding-right: 24px;
            font-size: 13px;
            font-weight: 600;
        }
        QPushButton:hover { background: #D35400; }
    )");
    connect(addBtn, &QPushButton::clicked, this, &ProductsPage::showAddProduct);
    topLayout->addWidget(addBtn);

    layout->addWidget(topBar);

    // ===== Table Card =====
    QFrame *tableCard = new QFrame();
    tableCard->setStyleSheet(R"(
        QFrame {
            background: white;
            border: 1px solid #E5E0D8;
            border-radius: 12px;
        }
    )");
    tableCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(tableCard);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 3);
    tableCard->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(tableCard);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

    QLabel *tableTitle = new QLabel("All Products");
    tableTitle->setStyleSheet("color: #5D3A1A; font-size: 15px; font-weight: 600; padding: 18px 24px; background: transparent; border: none;");
    cardLayout->addWidget(tableTitle);

    productTable = new QTableWidget();
    productTable->setColumnCount(6);
    productTable->setHorizontalHeaderLabels(
        QStringList() << "Product ID" << "Product Name" << "Category" << "Price" << "Status" << "Actions");
    productTable->setStyleSheet(R"(
        QTableWidget {
            background: white;
            border: none;
            gridline-color: transparent;
            outline: none;
        }
        QTableWidget::item {
            padding: 12px 24px;
            color: #5D3A1A;
            font-size: 13px;
            border-bottom: 1px solid #F0EDE6;
        }
        QTableWidget::item:selected {
            background: #FAF6F1;
            color: #5D3A1A;
        }
        QHeaderView::section {
            background: white;
            color: #999;
            font-weight: 600;
            font-size: 11px;
            border: none;
            border-bottom: 1px solid #F0EDE6;
            padding: 14px 24px;
        }
    )");
    productTable->horizontalHeader()->setStretchLastSection(true);
    productTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    productTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    productTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    productTable->setColumnWidth(0, 130);
    productTable->setColumnWidth(5, 140);
    productTable->verticalHeader()->setVisible(false);
    productTable->verticalHeader()->setDefaultSectionSize(55);
    productTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    productTable->setSelectionMode(QAbstractItemView::SingleSelection);
    productTable->setShowGrid(false);
    productTable->setFocusPolicy(Qt::NoFocus);
    productTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    cardLayout->addWidget(productTable);
    layout->addWidget(tableCard);

    stack->addWidget(productsPage);
}

// =====================================================
// Form Page (Add/Edit)
// =====================================================

void ProductsPage::setupFormPage()
{
    formPage = new QWidget();
    formPage->setStyleSheet("background: #FAF6F1;");

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: #FAF6F1; border: none; }");

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: #FAF6F1;");

    QVBoxLayout *layout = new QVBoxLayout(scrollContent);
    layout->setContentsMargins(32, 24, 32, 24);
    layout->setSpacing(16);

    // Back Button with icon
    QPushButton *backBtn = new QPushButton("  Back to Products");
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setFixedHeight(35);
    backBtn->setFixedWidth(200);
    backBtn->setIcon(QIcon(":/icons/logout.png"));
    backBtn->setIconSize(QSize(16, 16));
    backBtn->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            color: #888;
            border: none;
            text-align: left;
            font-size: 13px;
            padding: 0;
        }
        QPushButton:hover { color: #5D3A1A; }
    )");
    connect(backBtn, &QPushButton::clicked, this, &ProductsPage::goBack);
    layout->addWidget(backBtn);

    // Form Card
    QFrame *formCard = new QFrame();
    formCard->setStyleSheet(R"(
        QFrame {
            background: white;
            border: 1px solid #E5E0D8;
            border-radius: 12px;
        }
    )");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(formCard);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 15));
    shadow->setOffset(0, 3);
    formCard->setGraphicsEffect(shadow);

    QVBoxLayout *formLayout = new QVBoxLayout(formCard);
    formLayout->setContentsMargins(40, 30, 40, 30);
    formLayout->setSpacing(18);

    // Form Title with icon
    QHBoxLayout *formTitleLayout = new QHBoxLayout();
    QLabel *formTitleIcon = new QLabel();
    formTitleIcon->setFixedSize(28, 28);
    formTitleIcon->setPixmap(
        QPixmap(":/icons/product-management.png")
            .scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    formTitleIcon->setStyleSheet("background: transparent; border: none;");

    formTitle = new QLabel("Add New Product");
    formTitle->setStyleSheet("color: #5D3A1A; font-size: 24px; font-weight: bold; background: transparent; border: none;");

    formTitleLayout->addWidget(formTitleIcon);
    formTitleLayout->addWidget(formTitle);
    formTitleLayout->addStretch();
    formLayout->addLayout(formTitleLayout);

    formSubtitle = new QLabel("Add a new item to your menu");
    formSubtitle->setStyleSheet("color: #888; font-size: 13px; background: transparent; border: none;");
    formLayout->addWidget(formSubtitle);
    formLayout->addSpacing(10);

    QLabel *infoTitle = new QLabel("Product Information");
    infoTitle->setStyleSheet("color: #5D3A1A; font-size: 15px; font-weight: 600; background: transparent; border: none;");
    formLayout->addWidget(infoTitle);

    QString labelStyle = "color: #5D3A1A; font-size: 13px; font-weight: 500; background: transparent; border: none;";

    QString lineEditStyle = R"(
        QLineEdit {
            background: #FAF6F1;
            border: 1px solid #D4C5B0;
            border-radius: 8px;
            padding-left: 14px;
            padding-right: 14px;
            font-size: 13px;
            color: #5D3A1A;
        }
        QLineEdit:focus {
            border: 1px solid #E67E22;
            background: white;
        }
    )";

    QString textEditStyle = R"(
        QTextEdit {
            background: #FAF6F1;
            border: 1px solid #D4C5B0;
            border-radius: 8px;
            padding: 10px 14px;
            font-size: 13px;
            color: #5D3A1A;
        }
        QTextEdit:focus {
            border: 1px solid #E67E22;
            background: white;
        }
    )";

    QString comboStyle = R"(
        QComboBox {
            background: #FAF6F1;
            border: 1px solid #D4C5B0;
            border-radius: 8px;
            padding-left: 14px;
            padding-right: 14px;
            font-size: 13px;
            color: #5D3A1A;
        }
        QComboBox:focus { border: 1px solid #E67E22; background: white; }
        QComboBox::drop-down { border: none; width: 30px; }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #888;
            margin-right: 10px;
        }
    )";

    QString comboViewStyle = R"(
        QAbstractItemView {
            background-color: white;
            border: 1px solid #D4C5B0;
            border-radius: 6px;
            outline: none;
            padding: 4px;
            color: #5D3A1A;
        }
        QAbstractItemView::item {
            background-color: white;
            color: #5D3A1A;
            padding: 8px 14px;
            min-height: 28px;
            border: none;
        }
        QAbstractItemView::item:hover { background-color: #FAF6F1; color: #5D3A1A; }
        QAbstractItemView::item:selected { background-color: #E67E22; color: white; }
    )";

    // Product Name
    QLabel *nameLabel = new QLabel("Product Name *");
    nameLabel->setStyleSheet(labelStyle);
    formLayout->addWidget(nameLabel);
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("e.g., Cappuccino");
    nameEdit->setFixedHeight(42);
    nameEdit->setStyleSheet(lineEditStyle);
    formLayout->addWidget(nameEdit);

    // Category
    QLabel *catLabel = new QLabel("Category *");
    catLabel->setStyleSheet(labelStyle);
    formLayout->addWidget(catLabel);
    categoryEdit = new QComboBox();
    categoryEdit->setFixedHeight(42);
    categoryEdit->addItem("Select category");
    categoryEdit->addItems(QStringList() << "Coffee" << "Tea" << "Desserts" << "Cold Drinks" << "Beverages" << "Pastry" << "Dessert");
    categoryEdit->setStyleSheet(comboStyle);
    categoryEdit->view()->setStyleSheet(comboViewStyle);
    formLayout->addWidget(categoryEdit);

    // Price
    QLabel *priceLabel = new QLabel("Price ($) *");
    priceLabel->setStyleSheet(labelStyle);
    formLayout->addWidget(priceLabel);
    priceEdit = new QLineEdit();
    priceEdit->setPlaceholderText("0.00");
    priceEdit->setFixedHeight(42);
    priceEdit->setStyleSheet(lineEditStyle);
    formLayout->addWidget(priceEdit);

    // Description
    QLabel *descLabel = new QLabel("Description");
    descLabel->setStyleSheet(labelStyle);
    formLayout->addWidget(descLabel);
    descEdit = new QTextEdit();
    descEdit->setPlaceholderText("Enter product description...");
    descEdit->setFixedHeight(90);
    descEdit->setStyleSheet(textEditStyle);
    formLayout->addWidget(descEdit);

    // Status
    QLabel *statusLabel = new QLabel("Status");
    statusLabel->setStyleSheet(labelStyle);
    formLayout->addWidget(statusLabel);
    statusEdit = new QComboBox();
    statusEdit->setFixedHeight(42);
    statusEdit->addItems(QStringList() << "active" << "inactive");
    statusEdit->setStyleSheet(comboStyle);
    statusEdit->view()->setStyleSheet(comboViewStyle);
    formLayout->addWidget(statusEdit);

    // Image Upload
    QLabel *imgLabel = new QLabel("Product Image");
    imgLabel->setStyleSheet(labelStyle);
    formLayout->addWidget(imgLabel);

    imageLabel = new QLabel();
    imageLabel->setFixedHeight(140);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet(R"(
        QLabel {
            background: #FAF6F1;
            border: 2px dashed #D4C5B0;
            border-radius: 12px;
            color: #888;
            font-size: 13px;
        }
    )");

    // Upload icon inside image area
    QVBoxLayout *imageLabelLayout = new QVBoxLayout(imageLabel);
    imageLabelLayout->setAlignment(Qt::AlignCenter);
    QLabel *uploadIcon = new QLabel();
    uploadIcon->setFixedSize(32, 32);
    uploadIcon->setPixmap(
        QPixmap(":/icons/visible.png")
            .scaled(28, 28, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    uploadIcon->setStyleSheet("background: transparent; border: none;");
    uploadIcon->setAlignment(Qt::AlignCenter);
    imageLabelLayout->addWidget(uploadIcon);

    imageLabel->setText("");
    imageLabel->setCursor(Qt::PointingHandCursor);
    imageLabel->setWordWrap(true);
    imageLabel->installEventFilter(this);
    formLayout->addWidget(imageLabel);

    formLayout->addSpacing(10);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    saveBtn = new QPushButton("Add Product");
    saveBtn->setCursor(Qt::PointingHandCursor);
    saveBtn->setFixedHeight(45);
    saveBtn->setIcon(QIcon(":/icons/plus.png"));
    saveBtn->setIconSize(QSize(16, 16));
    saveBtn->setStyleSheet(R"(
        QPushButton {
            background: #6B4423;
            color: white;
            border: none;
            border-radius: 8px;
            padding-left: 32px;
            padding-right: 32px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover { background: #5D3A1A; }
    )");
    connect(saveBtn, &QPushButton::clicked, this, &ProductsPage::saveProduct);

    QPushButton *cancelBtn = new QPushButton("Cancel");
    cancelBtn->setCursor(Qt::PointingHandCursor);
    cancelBtn->setFixedHeight(45);
    cancelBtn->setStyleSheet(R"(
        QPushButton {
            background: white;
            color: #5D3A1A;
            border: 1px solid #D4C5B0;
            border-radius: 8px;
            padding-left: 32px;
            padding-right: 32px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover { background: #FAF6F1; }
    )");
    connect(cancelBtn, &QPushButton::clicked, this, &ProductsPage::goBack);

    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch();
    formLayout->addLayout(btnLayout);

    layout->addWidget(formCard);
    layout->addStretch();

    scrollArea->setWidget(scrollContent);

    QVBoxLayout *formPageLayout = new QVBoxLayout(formPage);
    formPageLayout->setContentsMargins(0, 0, 0, 0);
    formPageLayout->addWidget(scrollArea);

    stack->addWidget(formPage);
}

// =====================================================
// Load Products
// =====================================================

void ProductsPage::loadProducts()
{
    productTable->setRowCount(0);

    if (!DatabaseManager::instance().isConnected()) {
        QMessageBox::warning(this, "Database", "Not connected to database!");
        return;
    }

    QString search = searchEdit->text().trimmed();
    QString cat = categoryFilter->currentText();

    QString sql = "SELECT ProductID, ProductCode, ProductName, Category, Price, Status FROM Products WHERE 1=1";
    QStringList conditions;
    QVariantList values;

    if (!search.isEmpty()) {
        conditions << "(ProductName LIKE ? OR ProductCode LIKE ?)";
        values << QVariant("%" + search + "%");
        values << QVariant("%" + search + "%");
    }
    if (cat != "All Categories") {
        conditions << "Category = ?";
        values << QVariant(cat);
    }

    for (const QString &cond : conditions)
        sql += " AND " + cond;
    sql += " ORDER BY ProductID";

    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery q(db);
    q.prepare(sql);
    for (const QVariant &v : values)
        q.addBindValue(v);

    if (!q.exec()) {
        QMessageBox::critical(this, "Database Error", q.lastError().text());
        return;
    }

    int row = 0;
    while (q.next()) {
        productTable->insertRow(row);
        int id = q.value("ProductID").toInt();

        QTableWidgetItem *item0 = new QTableWidgetItem(q.value("ProductCode").toString());
        QTableWidgetItem *item1 = new QTableWidgetItem(q.value("ProductName").toString());
        QTableWidgetItem *item2 = new QTableWidgetItem(q.value("Category").toString());
        QTableWidgetItem *item3 = new QTableWidgetItem(QString("$%1").arg(q.value("Price").toDouble(), 0, 'f', 2));
        QString status = q.value("Status").toString();

        productTable->setItem(row, 0, item0);
        productTable->setItem(row, 1, item1);
        productTable->setItem(row, 2, item2);
        productTable->setItem(row, 3, item3);

        // Status Badge
        QWidget *statusWidget = new QWidget();
        statusWidget->setStyleSheet("background: transparent;");
        QHBoxLayout *statusLayout = new QHBoxLayout(statusWidget);
        statusLayout->setContentsMargins(15, 0, 15, 0);
        statusLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QLabel *statusBadge = new QLabel(status);
        statusBadge->setAlignment(Qt::AlignCenter);
        statusBadge->setFixedSize(80, 26);
        if (status == "active") {
            statusBadge->setStyleSheet("background:#D4EDDA; color:#155724; border-radius:13px; font-size:11px; font-weight:600;");
        } else {
            statusBadge->setStyleSheet("background:#E2E3E5; color:#383D41; border-radius:13px; font-size:11px; font-weight:600;");
        }
        statusLayout->addWidget(statusBadge);
        statusLayout->addStretch();
        productTable->setCellWidget(row, 4, statusWidget);

        // Actions with icons
        QWidget *actionWidget = new QWidget();
        actionWidget->setStyleSheet("background: transparent;");
        QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(10, 0, 15, 0);
        actionLayout->setSpacing(8);
        actionLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QPushButton *editBtn = new QPushButton();
        editBtn->setFixedSize(32, 32);
        editBtn->setCursor(Qt::PointingHandCursor);
        editBtn->setToolTip("Edit Product");
        editBtn->setIcon(QIcon(":/icons/edit.png"));
        editBtn->setIconSize(QSize(16, 16));
        editBtn->setStyleSheet(R"(
            QPushButton {
                background: #F3F4F6;
                border: 1px solid #E5E7EB;
                border-radius: 8px;
            }
            QPushButton:hover { background: #E8882F; border-color: #E8882F; }
        )");
        connect(editBtn, &QPushButton::clicked, this, &ProductsPage::showEditProduct);
        editBtn->setProperty("productId", id);

        QPushButton *delBtn = new QPushButton();
        delBtn->setFixedSize(32, 32);
        delBtn->setCursor(Qt::PointingHandCursor);
        delBtn->setToolTip("Delete Product");
        delBtn->setIcon(QIcon(":/icons/trash.png"));
        delBtn->setIconSize(QSize(16, 16));
        delBtn->setStyleSheet(R"(
            QPushButton {
                background: #F3F4F6;
                border: 1px solid #E5E7EB;
                border-radius: 8px;
            }
            QPushButton:hover { background: #FEE2E2; border-color: #FECACA; }
        )");
        connect(delBtn, &QPushButton::clicked, this, &ProductsPage::showEditProduct);
        delBtn->setProperty("productId", id);
        delBtn->setProperty("deleteMode", true);

        actionLayout->addWidget(editBtn);
        actionLayout->addWidget(delBtn);
        actionLayout->addStretch();
        productTable->setCellWidget(row, 5, actionWidget);

        row++;
    }
}

// =====================================================
// Navigation
// =====================================================

void ProductsPage::showProductsList()
{
    stack->setCurrentWidget(productsPage);
    loadProducts();
}

void ProductsPage::showAddProduct()
{
    isEditing = false;
    editingId = -1;
    clearForm();
    formTitle->setText("Add New Product");
    formSubtitle->setText("Add a new item to your menu");
    saveBtn->setText("Add Product");
    saveBtn->setIcon(QIcon(":/icons/plus.png"));
    saveBtn->setIconSize(QSize(16, 16));
    stack->setCurrentWidget(formPage);
}

void ProductsPage::showEditProduct()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int id = btn->property("productId").toInt();
    bool delMode = btn->property("deleteMode").toBool();

    if (delMode) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Confirm Delete");
        msgBox.setText("Are you sure you want to delete this product?");
        msgBox.setInformativeText("This action cannot be undone.");
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setStyleSheet(
            "QMessageBox { background-color: #FFFFFF; }"
            "QMessageBox QLabel { color: #1A1A1A; font-size: 14px; background-color: #FFFFFF; min-width: 350px; padding: 10px; }"
            "QMessageBox QPushButton { background-color: #E67E22; color: #FFFFFF; border: none; border-radius: 6px; padding: 10px 24px; font-size: 13px; font-weight: 600; min-width: 90px; margin: 4px; }"
            "QMessageBox QPushButton:hover { background-color: #D35400; }"
            );

        if (msgBox.exec() != QMessageBox::Yes) return;

        QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");

        QSqlQuery checkQuery(db);
        checkQuery.prepare("SELECT COUNT(*) FROM OrderDetails WHERE ProductID = ?");
        checkQuery.addBindValue(id);

        int orderCount = 0;
        if (checkQuery.exec() && checkQuery.next())
            orderCount = checkQuery.value(0).toInt();

        if (orderCount > 0) {
            QMessageBox warningBox(this);
            warningBox.setWindowTitle("Product in Use");
            warningBox.setText(QString("This product is used in %1 order(s).").arg(orderCount));
            warningBox.setInformativeText(
                "Do you want to:\n\n"
                "• YES: Delete the product AND all related order records\n"
                "• NO: Cancel the deletion"
                );
            warningBox.setIcon(QMessageBox::Warning);
            warningBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            warningBox.setDefaultButton(QMessageBox::No);
            warningBox.setStyleSheet(
                "QMessageBox { background-color: #FFFFFF; }"
                "QMessageBox QLabel { color: #1A1A1A; font-size: 13px; background-color: #FFFFFF; min-width: 400px; padding: 10px; }"
                "QMessageBox QPushButton { background-color: #E67E22; color: #FFFFFF; border: none; border-radius: 6px; padding: 10px 24px; font-size: 13px; font-weight: 600; min-width: 90px; margin: 4px; }"
                "QMessageBox QPushButton:hover { background-color: #D35400; }"
                );
            if (warningBox.exec() != QMessageBox::Yes) return;
        }

        db.transaction();

        QSqlQuery deleteOrderDetails(db);
        deleteOrderDetails.prepare("DELETE FROM OrderDetails WHERE ProductID = ?");
        deleteOrderDetails.addBindValue(id);
        if (!deleteOrderDetails.exec()) {
            db.rollback();
            QMessageBox::critical(this, "Error", "Failed to delete order details:\n" + deleteOrderDetails.lastError().text());
            return;
        }

        QSqlQuery deleteProduct(db);
        deleteProduct.prepare("DELETE FROM Products WHERE ProductID = ?");
        deleteProduct.addBindValue(id);
        if (!deleteProduct.exec()) {
            db.rollback();
            QMessageBox::critical(this, "Error", "Failed to delete product:\n" + deleteProduct.lastError().text());
            return;
        }

        if (!db.commit()) {
            db.rollback();
            QMessageBox::critical(this, "Error", "Failed to commit transaction");
            return;
        }

        QMessageBox::information(this, "Success",
                                 orderCount > 0
                                     ? QString("Product deleted successfully!\nAlso removed %1 related order record(s).").arg(orderCount)
                                     : "Product deleted successfully!"
                                 );

        loadProducts();
        return;
    }

    // Edit Mode
    isEditing = true;
    editingId = id;

    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery q(db);
    q.prepare("SELECT ProductCode, ProductName, Category, Price, Description, ImagePath, Status FROM Products WHERE ProductID = ?");
    q.addBindValue(editingId);

    if (!q.exec() || !q.next()) {
        QMessageBox::critical(this, "Error", "Failed to load product");
        return;
    }

    nameEdit->setText(q.value("ProductName").toString());
    int catIndex = categoryEdit->findText(q.value("Category").toString());
    if (catIndex >= 0) categoryEdit->setCurrentIndex(catIndex);
    priceEdit->setText(QString::number(q.value("Price").toDouble(), 'f', 2));
    descEdit->setPlainText(q.value("Description").toString());
    statusEdit->setCurrentText(q.value("Status").toString());
    currentImagePath = q.value("ImagePath").toString();

    if (!currentImagePath.isEmpty()) {
        QFileInfo fi(currentImagePath);
        imageLabel->setText("Selected: " + fi.fileName());
        imageLabel->setStyleSheet("QLabel { background:#FAF6F1; border:2px solid #E67E22; border-radius:12px; color:#5D3A1A; font-size:13px; }");
    } else {
        imageLabel->setText("Click to upload image\nPNG, JPG, GIF up to 10MB");
        imageLabel->setStyleSheet("QLabel { background:#FAF6F1; border:2px dashed #D4C5B0; border-radius:12px; color:#888; font-size:13px; }");
    }

    formTitle->setText("Edit Product");
    formSubtitle->setText("Update product information");
    saveBtn->setText("Update Product");
    saveBtn->setIcon(QIcon(":/icons/edit.png"));
    saveBtn->setIconSize(QSize(16, 16));
    stack->setCurrentWidget(formPage);
}

void ProductsPage::clearForm()
{
    nameEdit->clear();
    categoryEdit->setCurrentIndex(0);
    priceEdit->clear();
    descEdit->clear();
    statusEdit->setCurrentIndex(0);
    currentImagePath.clear();
    imageLabel->setText("Click to upload image\nPNG, JPG, GIF up to 10MB");
    imageLabel->setStyleSheet(R"(
        QLabel {
            background: #FAF6F1;
            border: 2px dashed #D4C5B0;
            border-radius: 12px;
            color: #888;
            font-size: 13px;
        }
    )");
}

// =====================================================
// Save Product
// =====================================================

void ProductsPage::saveProduct()
{
    QString name = nameEdit->text().trimmed();
    QString cat = categoryEdit->currentText();
    bool ok;
    double price = priceEdit->text().toDouble(&ok);
    QString desc = descEdit->toPlainText().trimmed();
    QString status = statusEdit->currentText();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Validation", "Please enter product name");
        nameEdit->setFocus();
        return;
    }
    if (cat == "Select category") {
        QMessageBox::warning(this, "Validation", "Please select a category");
        categoryEdit->setFocus();
        return;
    }
    if (priceEdit->text().isEmpty() || !ok || price <= 0) {
        QMessageBox::warning(this, "Validation", "Please enter a valid price");
        priceEdit->setFocus();
        return;
    }

    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery q(db);

    if (isEditing) {
        q.prepare("UPDATE Products SET ProductName=?, Category=?, Price=?, Description=?, ImagePath=?, Status=? WHERE ProductID=?");
        q.addBindValue(name);
        q.addBindValue(cat);
        q.addBindValue(price);
        q.addBindValue(desc);
        q.addBindValue(currentImagePath);
        q.addBindValue(status);
        q.addBindValue(editingId);
    } else {
        QString code = generateProductCode();
        q.prepare("INSERT INTO Products (ProductCode, ProductName, Category, Price, Description, ImagePath, Status) VALUES (?, ?, ?, ?, ?, ?, ?)");
        q.addBindValue(code);
        q.addBindValue(name);
        q.addBindValue(cat);
        q.addBindValue(price);
        q.addBindValue(desc);
        q.addBindValue(currentImagePath);
        q.addBindValue(status);
    }

    if (q.exec()) {
        QMessageBox::information(this, "Success",
                                 isEditing ? "Product updated successfully!" : "Product added successfully!");
        showProductsList();
    } else {
        QMessageBox::critical(this, "Error", "Failed to save:\n" + q.lastError().text());
    }
}

QString ProductsPage::generateProductCode()
{
    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery q(db);
    if (!q.exec("SELECT TOP 1 ProductCode FROM Products ORDER BY ProductID DESC") || !q.next())
        return "PRD-001";
    QString last = q.value(0).toString();
    int num = last.mid(4).toInt() + 1;
    return QString("PRD-%1").arg(num, 3, 10, QChar('0'));
}

void ProductsPage::chooseImage()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Select Image", QDir::homePath(), "Images (*.png *.jpg *.jpeg *.gif)");

    if (!path.isEmpty()) {
        currentImagePath = path;
        QFileInfo fi(path);
        imageLabel->setText("Selected: " + fi.fileName());
        imageLabel->setStyleSheet(R"(
            QLabel {
                background: #FAF6F1;
                border: 2px solid #E67E22;
                border-radius: 12px;
                color: #5D3A1A;
                font-size: 13px;
            }
        )");
    }
}

void ProductsPage::goBack()      { showProductsList(); }
void ProductsPage::searchProducts() { loadProducts(); }
void ProductsPage::filterCategory() { loadProducts(); }

bool ProductsPage::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == imageLabel && event->type() == QEvent::MouseButtonPress) {
        chooseImage();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}