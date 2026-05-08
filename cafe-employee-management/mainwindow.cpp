#include "mainwindow.h"
#include "addemployeedialog.h"
#include "database.h"

#include <QMessageBox>
#include <QHeaderView>
#include <QFrame>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Café Employee Management");

    if (!Database::instance().initialize()) {
        QMessageBox::critical(this, "Database Error",
                              "Failed to initialize the database.");
    }

    setupUi();
    showPage("Dashboard");
}

void MainWindow::setupUi()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    root->addWidget(buildSidebar());

    // Stacked widget holds all pages
    m_stack = new QStackedWidget;

    m_dashboard = new DashboardWidget;
    m_stack->addWidget(m_dashboard);              // index 0 — Dashboard

    m_stack->addWidget(buildEmployeesPage());     // index 1 — Employees

    root->addWidget(m_stack, 1);
}

// ── Sidebar ────────────────────────────────────────────────

QWidget* MainWindow::buildSidebar()
{
    QWidget *sidebar = new QWidget;
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(220);

    QVBoxLayout *lay = new QVBoxLayout(sidebar);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    QLabel *logo = new QLabel("  Brew & Co.");
    logo->setObjectName("sidebarLogo");
    logo->setFixedHeight(72);
    lay->addWidget(logo);

    QFrame *sep = new QFrame;
    sep->setObjectName("sidebarSep");
    sep->setFrameShape(QFrame::HLine);
    sep->setFixedHeight(1);
    lay->addWidget(sep);

    lay->addSpacing(12);

    const QStringList pages = {"Dashboard", "Products", "Orders", "Inventory",
                               "Employees", "Reports",  "Settings"};
    for (const auto &name : pages) {
        QPushButton *btn = makeNavButton(name);
        m_navButtons[name] = btn;
        lay->addWidget(btn);

        connect(btn, &QPushButton::clicked, this, [this, name]() {
            showPage(name);
        });
    }

    lay->addStretch();

    QPushButton *logout = makeNavButton("Logout");
    logout->setObjectName("logoutBtn");
    m_navButtons["Logout"] = logout;
    lay->addWidget(logout);
    lay->addSpacing(16);

    return sidebar;
}

QPushButton* MainWindow::makeNavButton(const QString &label)
{
    QPushButton *btn = new QPushButton("  " + label);
    btn->setObjectName("navBtn");
    btn->setFixedHeight(48);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFlat(true);
    return btn;
}

void MainWindow::setActiveNav(const QString &pageName)
{
    for (auto it = m_navButtons.constBegin(); it != m_navButtons.constEnd(); ++it) {
        bool active = (it.key() == pageName);
        it.value()->setObjectName(active ? "navBtnActive" : "navBtn");
        it.value()->style()->unpolish(it.value());
        it.value()->style()->polish(it.value());
    }
}

void MainWindow::showPage(const QString &pageName)
{
    m_activePage = pageName;
    setActiveNav(pageName);

    if (pageName == "Dashboard") {
        m_dashboard->refresh();
        m_stack->setCurrentIndex(0);
    } else if (pageName == "Employees") {
        onSearch(m_search ? m_search->text() : QString());
        m_stack->setCurrentIndex(1);
    }
    // Other pages: stay on current (no-op placeholder)
}

// ── Employees page ─────────────────────────────────────────

QWidget* MainWindow::buildEmployeesPage()
{
    QWidget *page = new QWidget;
    page->setObjectName("content");

    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(32, 28, 32, 28);
    lay->setSpacing(20);

    lay->addWidget(buildHeader());
    lay->addWidget(buildSearchBar());

    m_table = buildTable();
    lay->addWidget(m_table, 1);

    return page;
}

QWidget* MainWindow::buildHeader()
{
    QWidget *header = new QWidget;
    QHBoxLayout *lay = new QHBoxLayout(header);
    lay->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *titleLay = new QVBoxLayout;
    titleLay->setSpacing(4);
    QLabel *title = new QLabel("Employee Management");
    title->setObjectName("pageTitle");
    QLabel *sub = new QLabel("Manage your cafe staff");
    sub->setObjectName("pageSubtitle");
    titleLay->addWidget(title);
    titleLay->addWidget(sub);

    QPushButton *addBtn = new QPushButton("+ Add Employee");
    addBtn->setObjectName("addBtn");
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setFixedHeight(42);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddEmployee);

    lay->addLayout(titleLay);
    lay->addStretch();
    lay->addWidget(addBtn);

    return header;
}

QWidget* MainWindow::buildSearchBar()
{
    QWidget *bar = new QWidget;
    bar->setObjectName("searchBar");

    QHBoxLayout *lay = new QHBoxLayout(bar);
    lay->setContentsMargins(14, 0, 14, 0);

    QLabel *icon = new QLabel("Search");
    icon->setObjectName("searchIcon");

    m_search = new QLineEdit;
    m_search->setObjectName("searchInput");
    m_search->setPlaceholderText("Search by name or ID...");
    m_search->setFrame(false);
    connect(m_search, &QLineEdit::textChanged, this, &MainWindow::onSearch);

    lay->addWidget(icon);
    lay->addWidget(m_search, 1);

    bar->setFixedHeight(48);
    return bar;
}

QTableWidget* MainWindow::buildTable()
{
    QTableWidget *table = new QTableWidget;
    table->setObjectName("employeeTable");

    QStringList headers = {"Employee ID", "Name", "Role", "Email", "Status", "Actions"};
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    table->setColumnWidth(5, 120);

    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setAlternatingRowColors(true);
    table->setShowGrid(false);
    table->verticalHeader()->setDefaultSectionSize(56);

    return table;
}

void MainWindow::populateRow(int row, const Employee &emp)
{
    auto cell = [&](int col, const QString &text) {
        QTableWidgetItem *item = new QTableWidgetItem(text);
        item->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        m_table->setItem(row, col, item);
    };

    cell(0, emp.id);
    cell(1, emp.name);

    QLabel *roleBadge = new QLabel(emp.role);
    roleBadge->setObjectName("roleBadge");
    roleBadge->setAlignment(Qt::AlignCenter);
    roleBadge->setContentsMargins(10, 4, 10, 4);
    m_table->setCellWidget(row, 2, roleBadge);

    cell(3, emp.email);

    QLabel *statusBadge = new QLabel(emp.status);
    statusBadge->setObjectName(emp.status == "Active" ? "statusActive" : "statusInactive");
    statusBadge->setAlignment(Qt::AlignCenter);
    statusBadge->setContentsMargins(10, 4, 10, 4);
    m_table->setCellWidget(row, 4, statusBadge);

    QWidget *actions = new QWidget;
    QHBoxLayout *aLay = new QHBoxLayout(actions);
    aLay->setContentsMargins(8, 8, 8, 8);
    aLay->setSpacing(8);

    QPushButton *editBtn = new QPushButton("Edit");
    editBtn->setObjectName("editBtn");
    editBtn->setCursor(Qt::PointingHandCursor);
    editBtn->setFixedHeight(30);

    QPushButton *delBtn = new QPushButton("Del");
    delBtn->setObjectName("deleteBtn");
    delBtn->setCursor(Qt::PointingHandCursor);
    delBtn->setFixedHeight(30);

    QString id = emp.id;
    connect(editBtn, &QPushButton::clicked, this, [this, id]() { onEditEmployee(id); });
    connect(delBtn,  &QPushButton::clicked, this, [this, id]() { onDeleteEmployee(id); });

    aLay->addWidget(editBtn);
    aLay->addWidget(delBtn);
    m_table->setCellWidget(row, 5, actions);
}

void MainWindow::refreshTable(const QList<Employee> &employees)
{
    m_table->setRowCount(0);
    m_table->setRowCount(employees.size());
    for (int i = 0; i < employees.size(); ++i)
        populateRow(i, employees[i]);
}

void MainWindow::onSearch(const QString &text)
{
    QList<Employee> result = text.trimmed().isEmpty()
        ? Database::instance().fetchAllEmployees()
        : Database::instance().searchEmployees(text.trimmed());
    refreshTable(result);
}

void MainWindow::onAddEmployee()
{
    AddEmployeeDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        Employee emp = dlg.getEmployee();
        if (!emp.id.isEmpty() && !emp.name.isEmpty()) {
            if (Database::instance().addEmployee(emp)) {
                onSearch(m_search->text());
                m_dashboard->refresh();
            } else {
                QMessageBox::warning(this, "Error",
                    "Could not add employee. The ID may already exist.");
            }
        }
    }
}

void MainWindow::onEditEmployee(const QString &id)
{
    QList<Employee> all = Database::instance().fetchAllEmployees();
    Employee found;
    bool ok = false;
    for (const auto &e : all) {
        if (e.id == id) { found = e; ok = true; break; }
    }
    if (!ok) return;

    AddEmployeeDialog dlg(found, this);
    if (dlg.exec() == QDialog::Accepted) {
        Employee updated = dlg.getEmployee();
        if (Database::instance().updateEmployee(updated)) {
            onSearch(m_search->text());
            m_dashboard->refresh();
        } else {
            QMessageBox::warning(this, "Error", "Could not update employee.");
        }
    }
}

void MainWindow::onDeleteEmployee(const QString &id)
{
    int ret = QMessageBox::question(this, "Confirm Delete",
                                    QString("Delete employee %1?").arg(id),
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (Database::instance().deleteEmployee(id)) {
            onSearch(m_search->text());
            m_dashboard->refresh();
        } else {
            QMessageBox::warning(this, "Error", "Could not delete employee.");
        }
    }
}
