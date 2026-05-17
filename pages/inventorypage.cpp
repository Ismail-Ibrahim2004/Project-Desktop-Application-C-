#include "inventorypage.h"
#include "../database/databasemanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFrame>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QGraphicsDropShadowEffect>
#include <QScrollArea>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QComboBox>

// ==============================================
// InventoryDialog Implementation
// ==============================================

InventoryDialog::InventoryDialog(QWidget *parent, bool isEdit)
    : QDialog(parent)
{
    setupUI(isEdit);
}

void InventoryDialog::setupUI(bool isEdit)
{
    setWindowTitle(isEdit ? "Edit Item" : "Add New Item");
    setFixedSize(420, 460);
    setModal(true);

    setStyleSheet(R"(
        QDialog { background-color: #FFFFFF; }
        QLabel {
            font-size: 13px; font-weight: 600;
            color: #374151; background: transparent;
        }
        QLineEdit {
            border: 1.5px solid #E5E7EB; border-radius: 8px;
            padding: 10px 14px; font-size: 13px;
            color: #1F2937; background: #FFFFFF;
        }
        QLineEdit:focus { border: 1.5px solid #E8882F; }
        QDoubleSpinBox {
            border: 1.5px solid #E5E7EB; border-radius: 8px;
            padding: 8px 14px; font-size: 13px;
            color: #1F2937; background: #FFFFFF;
        }
        QDoubleSpinBox:focus { border: 1.5px solid #E8882F; }
        QComboBox {
            border: 1.5px solid #E5E7EB; border-radius: 8px;
            padding: 8px 14px; font-size: 13px;
            color: #1F2937; background: #FFFFFF;
        }
        QComboBox:focus { border: 1.5px solid #E8882F; }
        QComboBox::drop-down { border: none; width: 20px; }
        QComboBox QAbstractItemView {
            background: white; color: #1F2937;
            selection-background-color: #E8882F;
            selection-color: white; border: 1px solid #E5E7EB;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(18);

    // Title with icon
    QHBoxLayout *titleLayout = new QHBoxLayout();
    QLabel *titleIcon = new QLabel();
    titleIcon->setFixedSize(28, 28);
    titleIcon->setPixmap(
        QPixmap(isEdit ? ":/icons/edit.png" : ":/icons/plus.png")
            .scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    titleIcon->setStyleSheet("background: transparent;");

    QLabel *title = new QLabel(isEdit ? "Edit Item" : "Add New Item");
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #1F2937;");

    titleLayout->addWidget(titleIcon);
    titleLayout->addWidget(title);
    titleLayout->addStretch();
    mainLayout->addLayout(titleLayout);

    QFormLayout *form = new QFormLayout();
    form->setSpacing(14);

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("e.g. Coffee Beans");

    m_quantitySpin = new QDoubleSpinBox();
    m_quantitySpin->setRange(0, 99999);
    m_quantitySpin->setDecimals(1);
    m_quantitySpin->setValue(0);

    m_unitCombo = new QComboBox();
    m_unitCombo->addItems({"kg", "liters", "pieces", "bottles", "packs", "boxes", "grams"});

    m_minQuantitySpin = new QDoubleSpinBox();
    m_minQuantitySpin->setRange(0, 99999);
    m_minQuantitySpin->setDecimals(1);
    m_minQuantitySpin->setValue(0);

    m_supplierEdit = new QLineEdit();
    m_supplierEdit->setPlaceholderText("e.g. Bean Masters Co.");

    form->addRow("Item Name *",  m_nameEdit);
    form->addRow("Quantity *",   m_quantitySpin);
    form->addRow("Unit *",       m_unitCombo);
    form->addRow("Min Quantity", m_minQuantitySpin);
    form->addRow("Supplier",     m_supplierEdit);

    mainLayout->addLayout(form);
    mainLayout->addStretch();

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    QPushButton *cancelBtn = new QPushButton("Cancel");
    cancelBtn->setFixedHeight(42);
    cancelBtn->setStyleSheet(R"(
        QPushButton {
            background: #F3F4F6; color: #6B7280;
            border: none; border-radius: 8px;
            font-size: 14px; font-weight: 600;
        }
        QPushButton:hover { background: #E5E7EB; }
    )");

    QPushButton *saveBtn = new QPushButton(isEdit ? "Save Changes" : "Add Item");
    saveBtn->setFixedHeight(42);
    saveBtn->setIcon(QIcon(isEdit ? ":/icons/edit.png" : ":/icons/plus.png"));
    saveBtn->setIconSize(QSize(16, 16));
    saveBtn->setStyleSheet(R"(
        QPushButton {
            background: #E8882F; color: white;
            border: none; border-radius: 8px;
            font-size: 14px; font-weight: 600;
        }
        QPushButton:hover { background: #D4781F; }
    )");

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    mainLayout->addLayout(btnLayout);

    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(saveBtn, &QPushButton::clicked, this, [this]() {
        if (m_nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Validation", "Please enter item name.");
            return;
        }
        accept();
    });
}

QString InventoryDialog::getName()        const { return m_nameEdit->text().trimmed(); }
double  InventoryDialog::getQuantity()    const { return m_quantitySpin->value(); }
QString InventoryDialog::getUnit()        const { return m_unitCombo->currentText(); }
double  InventoryDialog::getMinQuantity() const { return m_minQuantitySpin->value(); }
QString InventoryDialog::getSupplier()    const { return m_supplierEdit->text().trimmed(); }

void InventoryDialog::setData(int id, const QString &name, double quantity,
                              const QString &unit, double minQuantity,
                              const QString &supplier)
{
    m_itemId = id;
    m_nameEdit->setText(name);
    m_quantitySpin->setValue(quantity);
    int idx = m_unitCombo->findText(unit);
    if (idx >= 0) m_unitCombo->setCurrentIndex(idx);
    m_minQuantitySpin->setValue(minQuantity);
    m_supplierEdit->setText(supplier);
}

// ==============================================
// InventoryPage Implementation
// ==============================================

InventoryPage::InventoryPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadInventory();
}

void InventoryPage::setupUI()
{
    setStyleSheet("background-color: #FAF6F1;");

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: #FAF6F1; border: none; }");

    QWidget *content = new QWidget();
    content->setStyleSheet("background: #FAF6F1;");

    QVBoxLayout *mainLayout = new QVBoxLayout(content);
    mainLayout->setContentsMargins(40, 35, 40, 35);
    mainLayout->setSpacing(20);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QVBoxLayout *titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(4);

    QLabel *pageTitle = new QLabel("Inventory Management");
    pageTitle->setStyleSheet(
        "font-size: 30px; font-weight: bold; color: #1A1A1A; background: transparent;");

    QLabel *pageSub = new QLabel("Track and manage your stock items");
    pageSub->setStyleSheet(
        "font-size: 14px; color: #888888; background: transparent;");

    titleLayout->addWidget(pageTitle);
    titleLayout->addWidget(pageSub);

    QPushButton *addBtn = new QPushButton("  Add Item");
    addBtn->setFixedSize(140, 42);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setIcon(QIcon(":/icons/plus.png"));
    addBtn->setIconSize(QSize(16, 16));
    addBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #E8882F; color: white;
            border: none; border-radius: 10px;
            font-size: 14px; font-weight: bold;
        }
        QPushButton:hover { background-color: #D4781F; }
        QPushButton:pressed { background-color: #C06810; }
    )");
    connect(addBtn, &QPushButton::clicked, this, &InventoryPage::onAddItem);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(addBtn);
    mainLayout->addLayout(headerLayout);

    // Low Stock Banner with icon
    m_lowStockBanner = new QWidget();
    m_lowStockBanner->setStyleSheet(R"(
        QWidget {
            background: #FFF7ED;
            border: 1px solid #FDBA74;
            border-radius: 12px;
        }
    )");

    QHBoxLayout *bannerLayout = new QHBoxLayout(m_lowStockBanner);
    bannerLayout->setContentsMargins(20, 15, 20, 15);
    bannerLayout->setSpacing(12);

    QLabel *warningIcon = new QLabel();
    warningIcon->setFixedSize(24, 24);
    warningIcon->setPixmap(
        QPixmap(":/icons/warning.png")
            .scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    warningIcon->setStyleSheet("background: transparent; border: none;");

    m_lowStockLabel = new QLabel("0 items are running low on stock");
    m_lowStockLabel->setStyleSheet(
        "color: #92400E; font-size: 14px; font-weight: 500; background: transparent; border: none;");

    bannerLayout->addWidget(warningIcon);
    bannerLayout->addWidget(m_lowStockLabel);
    bannerLayout->addStretch();

    mainLayout->addWidget(m_lowStockBanner);

    // Search Bar with icon
    QWidget *searchCard = new QWidget();
    searchCard->setStyleSheet(R"(
        QWidget {
            background: white; border-radius: 12px;
            border: 1px solid rgba(0,0,0,0.07);
        }
    )");

    QGraphicsDropShadowEffect *searchShadow = new QGraphicsDropShadowEffect();
    searchShadow->setBlurRadius(15);
    searchShadow->setColor(QColor(0, 0, 0, 18));
    searchShadow->setOffset(0, 3);
    searchCard->setGraphicsEffect(searchShadow);

    QHBoxLayout *searchLayout = new QHBoxLayout(searchCard);
    searchLayout->setContentsMargins(16, 12, 16, 12);
    searchLayout->setSpacing(10);

    QLabel *searchIcon = new QLabel();
    searchIcon->setFixedSize(20, 20);
    searchIcon->setPixmap(
        QPixmap(":/icons/search-interface-symbol.png")
            .scaled(18, 18, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    searchIcon->setStyleSheet("background: transparent; border: none;");

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Search by item name or supplier...");
    m_searchEdit->setStyleSheet(R"(
        QLineEdit {
            border: none; font-size: 14px;
            color: #333333; background: transparent; padding: 2px;
        }
        QLineEdit:focus { outline: none; }
    )");

    searchLayout->addWidget(searchIcon);
    searchLayout->addWidget(m_searchEdit);
    mainLayout->addWidget(searchCard);

    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &InventoryPage::onSearchChanged);

    // Table Card
    QWidget *tableCard = new QWidget();
    tableCard->setStyleSheet("QWidget { background: white; border-radius: 16px; }");

    QGraphicsDropShadowEffect *tableShadow = new QGraphicsDropShadowEffect();
    tableShadow->setBlurRadius(20);
    tableShadow->setColor(QColor(0, 0, 0, 15));
    tableShadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(tableShadow);

    QVBoxLayout *tableCardLayout = new QVBoxLayout(tableCard);
    tableCardLayout->setContentsMargins(25, 20, 25, 20);
    tableCardLayout->setSpacing(15);

    QLabel *tableTitle = new QLabel("Stock Items");
    tableTitle->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #1A1A1A; background: transparent;");
    tableCardLayout->addWidget(tableTitle);

    setupTable();
    tableCardLayout->addWidget(m_table);

    mainLayout->addWidget(tableCard);
    mainLayout->addStretch();

    scrollArea->setWidget(content);
    outerLayout->addWidget(scrollArea);
}

void InventoryPage::setupTable()
{
    m_table = new QTableWidget();
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({
        "Item Name", "Quantity", "Supplier", "Status", "Last Updated", "Actions"
    });

    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setAlternatingRowColors(false);
    m_table->verticalHeader()->setVisible(false);
    m_table->setShowGrid(false);
    m_table->setFocusPolicy(Qt::NoFocus);
    m_table->setFrameShape(QFrame::NoFrame);
    m_table->verticalHeader()->setDefaultSectionSize(52);

    m_table->horizontalHeader()->setStretchLastSection(false);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->setColumnWidth(1, 120);
    m_table->setColumnWidth(2, 180);
    m_table->setColumnWidth(3, 120);
    m_table->setColumnWidth(4, 130);
    m_table->setColumnWidth(5, 100);

    m_table->setStyleSheet(R"(
        QTableWidget {
            background: transparent; border: none;
            font-size: 14px; color: #374151;
        }
        QTableWidget::item {
            padding: 8px 12px;
            border-bottom: 1px solid #F3F4F6;
            background: transparent;
        }
        QTableWidget::item:selected {
            background-color: #FFF3E8; color: #1A1A1A;
        }
        QHeaderView::section {
            background: transparent; color: #6B7280;
            font-size: 13px; font-weight: 600;
            padding: 10px 12px; border: none;
            border-bottom: 2px solid #F3F4F6;
        }
    )");
}

void InventoryPage::loadInventory(const QString &searchTerm)
{
    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery query(db);

    QString sql = R"(
        SELECT InventoryID, ItemName, Quantity, Unit, MinQuantity,
               ISNULL(Supplier, '—') as Supplier,
               CONVERT(VARCHAR(10), ISNULL(LastRestocked, CreatedAt), 120) as LastUpdated
        FROM Inventory
    )";

    if (!searchTerm.isEmpty()) {
        sql += QString(" WHERE ItemName LIKE '%%1%' OR Supplier LIKE '%%1%'")
        .arg(searchTerm);
    }

    sql += " ORDER BY InventoryID";

    m_table->setRowCount(0);
    int row = 0;

    if (query.exec(sql)) {
        while (query.next()) {
            int     id          = query.value("InventoryID").toInt();
            QString name        = query.value("ItemName").toString();
            double  quantity    = query.value("Quantity").toDouble();
            QString unit        = query.value("Unit").toString();
            double  minQuantity = query.value("MinQuantity").toDouble();
            QString supplier    = query.value("Supplier").toString();
            QString lastUpdated = query.value("LastUpdated").toString();

            addTableRow(row, id, name, quantity, unit, minQuantity, supplier, lastUpdated);
            row++;
        }
    }

    updateLowStockBanner();
}

void InventoryPage::addTableRow(int row, int id, const QString &name,
                                double quantity, const QString &unit,
                                double minQuantity, const QString &supplier,
                                const QString &lastRestocked)
{
    m_table->insertRow(row);

    // Column 0: Item Name
    QTableWidgetItem *nameItem = new QTableWidgetItem(name);
    nameItem->setForeground(QColor("#1A1A1A"));
    m_table->setItem(row, 0, nameItem);

    // Column 1: Quantity
    QTableWidgetItem *qtyItem = new QTableWidgetItem(
        QString("%1 %2").arg(quantity, 0, 'f', 0).arg(unit));
    qtyItem->setTextAlignment(Qt::AlignCenter);
    qtyItem->setForeground(QColor("#374151"));
    m_table->setItem(row, 1, qtyItem);

    // Column 2: Supplier
    QTableWidgetItem *supplierItem = new QTableWidgetItem(supplier);
    supplierItem->setForeground(QColor("#6B7280"));
    m_table->setItem(row, 2, supplierItem);

    // Column 3: Status Badge
    bool isLowStock = quantity <= minQuantity;

    QWidget *statusWidget = new QWidget();
    statusWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *statusLayout = new QHBoxLayout(statusWidget);
    statusLayout->setContentsMargins(8, 4, 8, 4);
    statusLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *statusBadge = new QLabel(isLowStock ? "Low Stock" : "In Stock");
    statusBadge->setAlignment(Qt::AlignCenter);
    statusBadge->setFixedHeight(26);

    QFontMetrics fm(statusBadge->font());
    int textWidth = fm.horizontalAdvance(statusBadge->text()) + 24;
    statusBadge->setFixedWidth(textWidth);

    statusBadge->setStyleSheet(isLowStock
                                   ? "background:#FEF3C7; color:#D97706; border-radius:13px; font-size:12px; font-weight:600;"
                                   : "background:#DCFCE7; color:#16A34A; border-radius:13px; font-size:12px; font-weight:600;");

    statusLayout->addWidget(statusBadge);
    m_table->setCellWidget(row, 3, statusWidget);

    // Column 4: Last Updated
    QTableWidgetItem *dateItem = new QTableWidgetItem(lastRestocked);
    dateItem->setForeground(QColor("#6B7280"));
    dateItem->setTextAlignment(Qt::AlignCenter);
    m_table->setItem(row, 4, dateItem);

    // Column 5: Actions with icons
    QWidget *actionsWidget = new QWidget();
    actionsWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
    actionsLayout->setContentsMargins(8, 0, 8, 0);
    actionsLayout->setSpacing(8);
    actionsLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QPushButton *editBtn = new QPushButton();
    editBtn->setFixedSize(32, 32);
    editBtn->setCursor(Qt::PointingHandCursor);
    editBtn->setToolTip("Edit Item");
    editBtn->setIcon(QIcon(":/icons/edit.png"));
    editBtn->setIconSize(QSize(16, 16));
    editBtn->setStyleSheet(R"(
        QPushButton {
            background: #F3F4F6; border: 1px solid #E5E7EB; border-radius: 8px;
        }
        QPushButton:hover { background: #E8882F; border-color: #E8882F; }
    )");

    QPushButton *deleteBtn = new QPushButton();
    deleteBtn->setFixedSize(32, 32);
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setToolTip("Delete Item");
    deleteBtn->setIcon(QIcon(":/icons/trash.png"));
    deleteBtn->setIconSize(QSize(16, 16));
    deleteBtn->setStyleSheet(R"(
        QPushButton {
            background: #F3F4F6; border: 1px solid #E5E7EB; border-radius: 8px;
        }
        QPushButton:hover { background: #FEE2E2; border-color: #FECACA; }
    )");

    connect(editBtn, &QPushButton::clicked, this, [this, id]() {
        onEditItem(id);
    });
    connect(deleteBtn, &QPushButton::clicked, this, [this, id, name]() {
        onDeleteItem(id, name);
    });

    actionsLayout->addWidget(editBtn);
    actionsLayout->addWidget(deleteBtn);
    actionsLayout->addStretch();
    m_table->setCellWidget(row, 5, actionsWidget);
}

void InventoryPage::updateLowStockBanner()
{
    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery query(db);

    query.exec("SELECT COUNT(*) FROM Inventory WHERE Quantity <= MinQuantity");
    int lowCount = 0;
    if (query.next()) lowCount = query.value(0).toInt();

    if (lowCount > 0) {
        m_lowStockBanner->show();
        m_lowStockLabel->setText(
            QString("<b>%1 items</b> are running low on stock").arg(lowCount));
    } else {
        m_lowStockBanner->hide();
    }
}

// ==============================================
// CRUD Operations
// ==============================================

void InventoryPage::onAddItem()
{
    InventoryDialog dialog(this, false);

    if (dialog.exec() == QDialog::Accepted) {
        QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
        QSqlQuery query(db);

        query.prepare(R"(
            INSERT INTO Inventory (ItemName, Quantity, Unit, MinQuantity, Supplier, LastRestocked)
            VALUES (?, ?, ?, ?, ?, GETDATE())
        )");
        query.addBindValue(dialog.getName());
        query.addBindValue(dialog.getQuantity());
        query.addBindValue(dialog.getUnit());
        query.addBindValue(dialog.getMinQuantity());
        query.addBindValue(dialog.getSupplier());

        if (query.exec()) {
            loadInventory(m_searchEdit->text());
            QMessageBox::information(this, "Success",
                                     QString("Item '%1' added successfully!").arg(dialog.getName()));
        } else {
            QMessageBox::critical(this, "Error",
                                  "Failed to add item.\n" + query.lastError().text());
        }
    }
}

void InventoryPage::onEditItem(int itemId)
{
    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery query(db);
    query.prepare("SELECT * FROM Inventory WHERE InventoryID = ?");
    query.addBindValue(itemId);

    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Error", "Could not load item data.");
        return;
    }

    InventoryDialog dialog(this, true);
    dialog.setData(
        itemId,
        query.value("ItemName").toString(),
        query.value("Quantity").toDouble(),
        query.value("Unit").toString(),
        query.value("MinQuantity").toDouble(),
        query.value("Supplier").toString()
        );

    if (dialog.exec() == QDialog::Accepted) {
        QSqlDatabase db2 = QSqlDatabase::database("cafe_manager_connection");
        QSqlQuery updateQuery(db2);
        updateQuery.prepare(R"(
            UPDATE Inventory
            SET ItemName = ?, Quantity = ?, Unit = ?,
                MinQuantity = ?, Supplier = ?, LastRestocked = GETDATE()
            WHERE InventoryID = ?
        )");
        updateQuery.addBindValue(dialog.getName());
        updateQuery.addBindValue(dialog.getQuantity());
        updateQuery.addBindValue(dialog.getUnit());
        updateQuery.addBindValue(dialog.getMinQuantity());
        updateQuery.addBindValue(dialog.getSupplier());
        updateQuery.addBindValue(itemId);

        if (updateQuery.exec()) {
            loadInventory(m_searchEdit->text());
        } else {
            QMessageBox::critical(this, "Error",
                                  "Failed to update item.\n" + updateQuery.lastError().text());
        }
    }
}

void InventoryPage::onDeleteItem(int itemId, const QString &name)
{
    QMessageBox::StandardButton reply = QMessageBox::warning(
        this, "Confirm Delete",
        QString("Are you sure you want to delete '%1'?\nThis action cannot be undone.").arg(name),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
        QSqlQuery query(db);
        query.prepare("DELETE FROM Inventory WHERE InventoryID = ?");
        query.addBindValue(itemId);

        if (query.exec()) {
            loadInventory(m_searchEdit->text());
        } else {
            QMessageBox::critical(this, "Error",
                                  "Failed to delete item.\n" + query.lastError().text());
        }
    }
}

void InventoryPage::onSearchChanged(const QString &text)
{
    loadInventory(text);
}