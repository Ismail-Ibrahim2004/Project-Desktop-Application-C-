#include "employeespage.h"
#include "../database/databasemanager.h"

#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>

// ==============================================
// EmployeeDialog Implementation
// ==============================================

EmployeeDialog::EmployeeDialog(QWidget *parent, bool isEdit)
    : QDialog(parent)
{
    setupUI(isEdit);
}

void EmployeeDialog::setupUI(bool isEdit)
{
    setWindowTitle(isEdit ? "Edit Employee" : "Add New Employee");
    setFixedSize(420, 440);
    setModal(true);

    setStyleSheet(R"(
        QDialog {
            background-color: #FFFFFF;
        }
        QLabel {
            font-size: 13px;
            font-weight: 600;
            color: #374151;
            background: transparent;
        }
        QLineEdit {
            border: 1.5px solid #E5E7EB;
            border-radius: 8px;
            padding: 10px 14px;
            font-size: 13px;
            color: #1F2937;
            background: #FFFFFF;
        }
        QLineEdit:focus {
            border: 1.5px solid #E8882F;
        }
        QComboBox {
            border: 1.5px solid #E5E7EB;
            border-radius: 8px;
            padding: 8px 14px;
            font-size: 13px;
            color: #1F2937;
            background: #FFFFFF;
        }
        QComboBox:focus {
            border: 1.5px solid #E8882F;
        }
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        QComboBox QAbstractItemView {
            background: white;
            color: #1F2937;
            selection-background-color: #E8882F;
            selection-color: white;
            border: 1px solid #E5E7EB;
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

    QLabel *title = new QLabel(isEdit ? "Edit Employee" : "Add New Employee");
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #1F2937;");

    titleLayout->addWidget(titleIcon);
    titleLayout->addWidget(title);
    titleLayout->addStretch();
    mainLayout->addLayout(titleLayout);

    QFormLayout *form = new QFormLayout();
    form->setSpacing(14);

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("e.g. John Doe");

    m_roleCombo = new QComboBox();
    m_roleCombo->addItems({"Barista", "Cashier", "Manager", "Waiter", "Chef"});

    m_emailEdit = new QLineEdit();
    m_emailEdit->setPlaceholderText("e.g. john@cafe.com");

    m_phoneEdit = new QLineEdit();
    m_phoneEdit->setPlaceholderText("e.g. +1 234 567 8900");

    m_activeCheck = new QCheckBox("Active Employee");
    m_activeCheck->setChecked(true);

    form->addRow("Full Name *", m_nameEdit);
    form->addRow("Role *", m_roleCombo);
    form->addRow("Email", m_emailEdit);
    form->addRow("Phone", m_phoneEdit);
    form->addRow("", m_activeCheck);

    mainLayout->addLayout(form);
    mainLayout->addStretch();

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    QPushButton *cancelBtn = new QPushButton("Cancel");
    cancelBtn->setFixedHeight(42);
    cancelBtn->setStyleSheet(R"(
        QPushButton {
            background: #F3F4F6;
            color: #6B7280;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover { background: #E5E7EB; }
    )");

    QPushButton *saveBtn = new QPushButton(isEdit ? "Save Changes" : "Add Employee");
    saveBtn->setFixedHeight(42);
    saveBtn->setIcon(QIcon(isEdit ? ":/icons/edit.png" : ":/icons/plus.png"));
    saveBtn->setIconSize(QSize(16, 16));
    saveBtn->setStyleSheet(R"(
        QPushButton {
            background: #E8882F;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton:hover { background: #D4781F; }
    )");

    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(saveBtn);
    mainLayout->addLayout(btnLayout);

    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(saveBtn, &QPushButton::clicked, this, [this]() {
        if (m_nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Validation", "Please enter employee name.");
            return;
        }
        accept();
    });
}

QString EmployeeDialog::getName()     const { return m_nameEdit->text().trimmed(); }
QString EmployeeDialog::getRole()     const { return m_roleCombo->currentText(); }
QString EmployeeDialog::getEmail()    const { return m_emailEdit->text().trimmed(); }
QString EmployeeDialog::getPhone()    const { return m_phoneEdit->text().trimmed(); }
bool    EmployeeDialog::getIsActive() const { return m_activeCheck->isChecked(); }

void EmployeeDialog::setData(int id, const QString &name, const QString &role,
                             const QString &email, const QString &phone, bool isActive)
{
    m_employeeId = id;
    m_nameEdit->setText(name);
    int idx = m_roleCombo->findText(role);
    if (idx >= 0) m_roleCombo->setCurrentIndex(idx);
    m_emailEdit->setText(email);
    m_phoneEdit->setText(phone);
    m_activeCheck->setChecked(isActive);
}

// ==============================================
// EmployeesPage Implementation
// ==============================================

EmployeesPage::EmployeesPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadEmployees();
}

void EmployeesPage::setupUI()
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
    mainLayout->setSpacing(25);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QVBoxLayout *titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(4);

    QLabel *pageTitle = new QLabel("Employee Management");
    pageTitle->setStyleSheet(
        "font-size: 30px; font-weight: bold; color: #1A1A1A; background: transparent;");

    QLabel *pageSub = new QLabel("Manage your café staff");
    pageSub->setStyleSheet(
        "font-size: 14px; color: #888888; background: transparent;");

    titleLayout->addWidget(pageTitle);
    titleLayout->addWidget(pageSub);

    QPushButton *addBtn = new QPushButton("  Add Employee");
    addBtn->setFixedSize(160, 42);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setIcon(QIcon(":/icons/plus.png"));
    addBtn->setIconSize(QSize(16, 16));
    addBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #E8882F;
            color: white;
            border: none;
            border-radius: 10px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover { background-color: #D4781F; }
        QPushButton:pressed { background-color: #C06810; }
    )");
    connect(addBtn, &QPushButton::clicked, this, &EmployeesPage::onAddEmployee);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(addBtn);
    mainLayout->addLayout(headerLayout);

    // Search Bar
    QWidget *searchCard = new QWidget();
    searchCard->setStyleSheet(R"(
        QWidget {
            background: white;
            border-radius: 12px;
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

    // Search Icon من الـ Resources
    QLabel *searchIcon = new QLabel();
    searchIcon->setFixedSize(20, 20);
    searchIcon->setPixmap(
        QPixmap(":/icons/search-interface-symbol.png")
            .scaled(18, 18, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    searchIcon->setStyleSheet("background: transparent;");

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Search employees...");
    m_searchEdit->setStyleSheet(R"(
        QLineEdit {
            border: none;
            font-size: 14px;
            color: #333333;
            background: transparent;
            padding: 2px;
        }
        QLineEdit:focus { outline: none; }
    )");

    searchLayout->addWidget(searchIcon);
    searchLayout->addWidget(m_searchEdit);
    mainLayout->addWidget(searchCard);

    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &EmployeesPage::onSearchChanged);

    // Table Card
    QWidget *tableCard = new QWidget();
    tableCard->setStyleSheet(R"(
        QWidget {
            background: white;
            border-radius: 16px;
        }
    )");

    QGraphicsDropShadowEffect *tableShadow = new QGraphicsDropShadowEffect();
    tableShadow->setBlurRadius(20);
    tableShadow->setColor(QColor(0, 0, 0, 15));
    tableShadow->setOffset(0, 4);
    tableCard->setGraphicsEffect(tableShadow);

    QVBoxLayout *tableCardLayout = new QVBoxLayout(tableCard);
    tableCardLayout->setContentsMargins(25, 20, 25, 20);
    tableCardLayout->setSpacing(15);

    QHBoxLayout *tableTitleLayout = new QHBoxLayout();
    QLabel *tableTitle = new QLabel("All Employees");
    tableTitle->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: #1A1A1A; background: transparent;");

    m_countLabel = new QLabel("0 employees");
    m_countLabel->setStyleSheet(
        "font-size: 13px; color: #888888; background: transparent;");

    tableTitleLayout->addWidget(tableTitle);
    tableTitleLayout->addStretch();
    tableTitleLayout->addWidget(m_countLabel);
    tableCardLayout->addLayout(tableTitleLayout);

    setupTable();
    tableCardLayout->addWidget(m_table);

    mainLayout->addWidget(tableCard);
    mainLayout->addStretch();

    scrollArea->setWidget(content);
    outerLayout->addWidget(scrollArea);
}

void EmployeesPage::setupTable()
{
    m_table = new QTableWidget();
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({
        "Employee ID", "Name", "Role", "Email", "Status", "Actions"
    });

    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setAlternatingRowColors(false);
    m_table->verticalHeader()->setVisible(false);
    m_table->setShowGrid(false);
    m_table->setFocusPolicy(Qt::NoFocus);
    m_table->setFrameShape(QFrame::NoFrame);
    m_table->verticalHeader()->setDefaultSectionSize(56);

    m_table->horizontalHeader()->setStretchLastSection(false);
    m_table->setColumnWidth(0, 130);
    m_table->setColumnWidth(1, 200);
    m_table->setColumnWidth(2, 140);
    m_table->setColumnWidth(3, 250);
    m_table->setColumnWidth(4, 120);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);

    m_table->setStyleSheet(R"(
        QTableWidget {
            background: transparent;
            border: none;
            font-size: 14px;
            color: #374151;
        }
        QTableWidget::item {
            padding: 8px 12px;
            border-bottom: 1px solid #F3F4F6;
            background: transparent;
        }
        QTableWidget::item:selected {
            background-color: #FFF3E8;
            color: #1A1A1A;
        }
        QHeaderView::section {
            background: transparent;
            color: #6B7280;
            font-size: 13px;
            font-weight: 600;
            padding: 10px 12px;
            border: none;
            border-bottom: 2px solid #F3F4F6;
        }
    )");
}

void EmployeesPage::loadEmployees(const QString &searchTerm)
{
    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery query(db);

    QString sql = R"(
        SELECT EmployeeID, EmployeeName, Role, Email, Phone, IsActive
        FROM Employees
    )";

    if (!searchTerm.isEmpty()) {
        sql += QString(" WHERE EmployeeName LIKE '%%1%' OR Email LIKE '%%1%'")
        .arg(searchTerm);
    }

    sql += " ORDER BY EmployeeID";

    m_table->setRowCount(0);
    int row = 0;

    if (query.exec(sql)) {
        while (query.next()) {
            int     id       = query.value("EmployeeID").toInt();
            QString name     = query.value("EmployeeName").toString();
            QString role     = query.value("Role").toString();
            QString email    = query.value("Email").toString();
            bool    isActive = query.value("IsActive").toBool();

            if (role.isEmpty())
                role = "Employee";

            addTableRow(row, id, name, role, email, isActive);
            row++;
        }
        m_countLabel->setText(QString("%1 employees").arg(row));
    }
}

void EmployeesPage::addTableRow(int row, int id, const QString &name,
                                const QString &role, const QString &email,
                                bool isActive)
{
    m_table->insertRow(row);

    // Column 0: Employee ID
    QString empIdStr = QString("EMP-%1").arg(id, 3, 10, QChar('0'));
    QTableWidgetItem *idItem = new QTableWidgetItem(empIdStr);
    idItem->setForeground(QColor("#E8882F"));
    QFont idFont;
    idFont.setBold(true);
    idItem->setFont(idFont);
    idItem->setData(Qt::UserRole, id);
    m_table->setItem(row, 0, idItem);

    // Column 1: Name
    QTableWidgetItem *nameItem = new QTableWidgetItem(name);
    QFont nameFont;
    nameFont.setBold(true);
    nameItem->setFont(nameFont);
    nameItem->setForeground(QColor("#1A1A1A"));
    m_table->setItem(row, 1, nameItem);

    // Column 2: Role Badge
    QWidget *roleWidget = new QWidget();
    roleWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *roleLayout = new QHBoxLayout(roleWidget);
    roleLayout->setContentsMargins(12, 6, 12, 6);
    roleLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *roleBadge = new QLabel(role);
    roleBadge->setAlignment(Qt::AlignCenter);

    QString badgeStyle;
    if (role == "Manager") {
        badgeStyle = "background:#FEF3C7; color:#D97706; border:1px solid #FDE68A;";
    } else if (role == "Barista") {
        badgeStyle = "background:#DBEAFE; color:#1D4ED8; border:1px solid #BFDBFE;";
    } else if (role == "Cashier") {
        badgeStyle = "background:#F3E8FF; color:#7C3AED; border:1px solid #E9D5FF;";
    } else {
        badgeStyle = "background:#F3F4F6; color:#6B7280; border:1px solid #E5E7EB;";
    }

    roleBadge->setStyleSheet(QString(R"(
        QLabel {
            %1
            border-radius: 12px;
            font-size: 12px;
            font-weight: 600;
            padding: 3px 12px;
        }
    )").arg(badgeStyle));

    roleLayout->addWidget(roleBadge);
    m_table->setCellWidget(row, 2, roleWidget);

    // Column 3: Email
    QTableWidgetItem *emailItem = new QTableWidgetItem(email.isEmpty() ? "—" : email);
    emailItem->setForeground(QColor("#6B7280"));
    m_table->setItem(row, 3, emailItem);

    // Column 4: Status Badge
    QWidget *statusWidget = new QWidget();
    statusWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *statusLayout = new QHBoxLayout(statusWidget);
    statusLayout->setContentsMargins(12, 6, 12, 6);
    statusLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *statusBadge = new QLabel(isActive ? "active" : "inactive");
    statusBadge->setAlignment(Qt::AlignCenter);
    statusBadge->setFixedSize(70, 26);
    statusBadge->setStyleSheet(isActive
                                   ? "background:#DCFCE7; color:#16A34A; border-radius:13px; font-size:12px; font-weight:600;"
                                   : "background:#FEE2E2; color:#DC2626; border-radius:13px; font-size:12px; font-weight:600;");

    statusLayout->addWidget(statusBadge);
    m_table->setCellWidget(row, 4, statusWidget);

    // Column 5: Actions
    QWidget *actionsWidget = new QWidget();
    actionsWidget->setStyleSheet("background: transparent;");
    QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
    actionsLayout->setContentsMargins(12, 0, 12, 0);
    actionsLayout->setSpacing(8);
    actionsLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Edit Button
    QPushButton *editBtn = new QPushButton();
    editBtn->setFixedSize(32, 32);
    editBtn->setCursor(Qt::PointingHandCursor);
    editBtn->setToolTip("Edit Employee");
    editBtn->setIcon(QIcon(":/icons/edit.png"));
    editBtn->setIconSize(QSize(16, 16));
    editBtn->setStyleSheet(R"(
        QPushButton {
            background: #F3F4F6;
            border: 1px solid #E5E7EB;
            border-radius: 8px;
        }
        QPushButton:hover {
            background: #E8882F;
            border-color: #E8882F;
        }
    )");

    // Delete Button
    QPushButton *deleteBtn = new QPushButton();
    deleteBtn->setFixedSize(32, 32);
    deleteBtn->setCursor(Qt::PointingHandCursor);
    deleteBtn->setToolTip("Delete Employee");
    deleteBtn->setIcon(QIcon(":/icons/trash.png"));
    deleteBtn->setIconSize(QSize(16, 16));
    deleteBtn->setStyleSheet(R"(
        QPushButton {
            background: #F3F4F6;
            border: 1px solid #E5E7EB;
            border-radius: 8px;
        }
        QPushButton:hover {
            background: #FEE2E2;
            border-color: #FECACA;
        }
    )");

    connect(editBtn, &QPushButton::clicked, this, [this, id]() {
        onEditEmployee(id);
    });
    connect(deleteBtn, &QPushButton::clicked, this, [this, id, name]() {
        onDeleteEmployee(id, name);
    });

    actionsLayout->addWidget(editBtn);
    actionsLayout->addWidget(deleteBtn);
    actionsLayout->addStretch();
    m_table->setCellWidget(row, 5, actionsWidget);
}

// ==============================================
// CRUD Operations
// ==============================================

void EmployeesPage::onAddEmployee()
{
    EmployeeDialog dialog(this, false);

    if (dialog.exec() == QDialog::Accepted) {
        DatabaseManager &db = DatabaseManager::instance();
        bool ok = db.insertEmployee(dialog.getName(), dialog.getIsActive());

        if (ok) {
            QSqlDatabase db2 = QSqlDatabase::database("cafe_manager_connection");
            QSqlQuery updateQuery(db2);
            updateQuery.prepare(R"(
                UPDATE Employees
                SET Email = ?, Phone = ?, Role = ?
                WHERE EmployeeID = (SELECT MAX(EmployeeID) FROM Employees)
            )");
            updateQuery.addBindValue(dialog.getEmail());
            updateQuery.addBindValue(dialog.getPhone());
            updateQuery.addBindValue(dialog.getRole());
            updateQuery.exec();

            loadEmployees(m_searchEdit->text());

            QMessageBox::information(
                this, "Success",
                QString("Employee '%1' added successfully!").arg(dialog.getName())
                );
        } else {
            QMessageBox::critical(
                this, "Error",
                "Failed to add employee.\n" + db.lastError()
                );
        }
    }
}

void EmployeesPage::onEditEmployee(int employeeId)
{
    QSqlDatabase dbConn = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery query(dbConn);
    query.prepare("SELECT * FROM Employees WHERE EmployeeID = ?");
    query.addBindValue(employeeId);

    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "Error", "Could not load employee data.");
        return;
    }

    EmployeeDialog dialog(this, true);
    dialog.setData(
        employeeId,
        query.value("EmployeeName").toString(),
        query.value("Role").toString(),
        query.value("Email").toString(),
        query.value("Phone").toString(),
        query.value("IsActive").toBool()
        );

    if (dialog.exec() == QDialog::Accepted) {
        DatabaseManager &db = DatabaseManager::instance();
        bool ok = db.updateEmployee(
            employeeId,
            dialog.getName(),
            dialog.getIsActive(),
            dialog.getRole()
            );

        if (ok) {
            QSqlDatabase dbConn2 = QSqlDatabase::database("cafe_manager_connection");
            QSqlQuery updateQuery(dbConn2);
            updateQuery.prepare(R"(
                UPDATE Employees SET Email = ?, Phone = ?, Role = ?
                WHERE EmployeeID = ?
            )");
            updateQuery.addBindValue(dialog.getEmail());
            updateQuery.addBindValue(dialog.getPhone());
            updateQuery.addBindValue(dialog.getRole());
            updateQuery.addBindValue(employeeId);
            updateQuery.exec();

            loadEmployees(m_searchEdit->text());
        } else {
            QMessageBox::critical(
                this, "Error",
                "Failed to update employee.\n" + db.lastError()
                );
        }
    }
}

void EmployeesPage::onDeleteEmployee(int employeeId, const QString &name)
{
    QMessageBox::StandardButton reply = QMessageBox::warning(
        this, "Confirm Delete",
        QString("Are you sure you want to delete employee '%1'?\n"
                "This action cannot be undone.").arg(name),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        DatabaseManager &db = DatabaseManager::instance();
        if (db.deleteEmployee(employeeId)) {
            loadEmployees(m_searchEdit->text());
        } else {
            QMessageBox::critical(
                this, "Error",
                "Failed to delete employee.\n" + db.lastError()
                );
        }
    }
}

void EmployeesPage::onSearchChanged(const QString &text)
{
    loadEmployees(text);
}