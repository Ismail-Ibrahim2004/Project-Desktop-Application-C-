#include "mainwindow.h"
#include <QHeaderView>
#include <QPushButton>
#include <QFrame>
#include <QApplication>
#include <QPrinter>
#include <QPrintDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setupSidebar(mainLayout);

    stackedWidget = new QStackedWidget();
    stackedWidget->addWidget(createOrderPage());
    stackedWidget->addWidget(createReceiptPage());
    mainLayout->addWidget(stackedWidget);

    setCentralWidget(centralWidget);
    resize(1200, 800);
}

void MainWindow::setupSidebar(QHBoxLayout *layout) {
    QFrame *sidebar = new QFrame();
    sidebar->setFixedWidth(250);
    sidebar->setStyleSheet("background-color: #2D1B18; border: none;");
    QVBoxLayout *sLayout = new QVBoxLayout(sidebar);
    sLayout->setContentsMargins(10, 20, 10, 20);

    QLabel *logo = new QLabel("☕ Café Manager");
    logo->setStyleSheet("color: white; font-size: 24px; font-weight: bold; padding: 20px;");
    sLayout->addWidget(logo);

    QStringList items = {"Dashboard", "POS System", "Orders", "Menu", "Settings"};
    for(const QString &txt : items) {
        QPushButton *btn = new QPushButton(txt);
        btn->setStyleSheet("QPushButton { color: #D7CCC8; border: none; text-align: left; padding: 15px 25px; font-size: 16px; }"
                           "QPushButton:hover { background-color: #4E342E; border-radius: 8px; }");
        sLayout->addWidget(btn);
    }

    sLayout->addStretch();

    QPushButton *logoutBtn = new QPushButton("↪ Logout");
    logoutBtn->setStyleSheet("QPushButton { color: #FF7675; background: transparent; text-align: left; padding: 15px 25px; font-size: 16px; font-weight: bold; border-top: 1px solid #4E342E; }");
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogout);
    sLayout->addWidget(logoutBtn);

    layout->addWidget(sidebar);
}

QWidget* MainWindow::createOrderPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 30, 30, 30);

    QLabel *title = new QLabel("Order Items");
    title->setStyleSheet("font-size: 30px; font-weight: bold; color: #2D1B18;");
    layout->addWidget(title);

    orderTable = new QTableWidget(0, 4);
    orderTable->setHorizontalHeaderLabels({"Item", "Qty", "Price", "Total"});
    orderTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // تعديل ألوان الهيدر عشان تظهر سوداء واضحة (علاج الصورة الباهتة)
    orderTable->setStyleSheet(
        "QTableWidget { background-color: white; color: black; border-radius: 10px; gridline-color: #EEE; }"
        "QHeaderView::section { background-color: #F5F5F5; color: #000000; font-weight: bold; padding: 10px; border: 1px solid #DDD; }"
        );

    connect(orderTable, &QTableWidget::cellChanged, this, [this](int row, int col){
        if(col == 1 || col == 2) {
            float qty = orderTable->item(row, 1) ? orderTable->item(row, 1)->text().toFloat() : 0;
            float price = orderTable->item(row, 2) ? orderTable->item(row, 2)->text().toFloat() : 0;
            orderTable->blockSignals(true);
            QTableWidgetItem *totalItem = new QTableWidgetItem(QString::number(qty * price, 'f', 2));
            totalItem->setForeground(Qt::black);
            orderTable->setItem(row, 3, totalItem);
            orderTable->blockSignals(false);
            updateGrandTotal();
        }
    });

    layout->addWidget(orderTable);

    QHBoxLayout *bottom = new QHBoxLayout();
    totalLabel = new QLabel("Total: 0.00 EGP");
    totalLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2D1B18;");

    QPushButton *addBtn = new QPushButton("+ Add Item");
    addBtn->setFixedSize(140, 45);
    addBtn->setStyleSheet("background-color: #A67B5B; color: white; border-radius: 8px; font-weight: bold;");
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addItem);

    QPushButton *saveBtn = new QPushButton("Save & Print");
    saveBtn->setFixedSize(160, 45);
    saveBtn->setStyleSheet("background-color: #2D1B18; color: white; border-radius: 8px; font-weight: bold;");
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::showReceiptPage);

    bottom->addWidget(totalLabel);
    bottom->addStretch();
    bottom->addWidget(addBtn);
    bottom->addWidget(saveBtn);
    layout->addLayout(bottom);

    page->setStyleSheet("background-color: #FDF5E6;");
    return page;
}

QWidget* MainWindow::createReceiptPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(50, 40, 50, 40);

    QLabel *successMsg = new QLabel("✅ Receipt Generated Successfully!");
    successMsg->setStyleSheet("background-color: #E8F5E9; color: #2E7D32; padding: 15px; border-radius: 10px; font-weight: bold;");
    successMsg->setAlignment(Qt::AlignCenter);
    layout->addWidget(successMsg);

    receiptPreview = new QTextEdit();
    receiptPreview->setReadOnly(true);
    receiptPreview->setMinimumHeight(450);
    receiptPreview->setStyleSheet("QTextEdit { background-color: white; border-radius: 15px; padding: 30px; border: 1px solid #DDD; color: black; }");
    layout->addWidget(receiptPreview);

    QHBoxLayout *btns = new QHBoxLayout();
    btns->setSpacing(20);

    QPushButton *printBtn = new QPushButton("🖨 Print Receipt");
    printBtn->setFixedSize(200, 50);
    printBtn->setStyleSheet("background-color: #2D1B18; color: white; border-radius: 10px; font-weight: bold;");
    connect(printBtn, &QPushButton::clicked, this, [this](){
        QPrinter printer;
        QPrintDialog dialog(&printer, this);
        if (dialog.exec() == QDialog::Accepted) { receiptPreview->print(&printer); }
    });

    QPushButton *newOrderBtn = new QPushButton("New Order");
    newOrderBtn->setFixedSize(200, 50);
    newOrderBtn->setStyleSheet("background-color: #A67B5B; color: white; border-radius: 10px; font-weight: bold;");
    connect(newOrderBtn, &QPushButton::clicked, this, &MainWindow::showOrderPage);

    btns->addStretch();
    btns->addWidget(printBtn);
    btns->addWidget(newOrderBtn);
    btns->addStretch();
    layout->addLayout(btns);

    page->setStyleSheet("background-color: #FDF5E6;");
    return page;
}

// باقي الدوال المساعدة
void MainWindow::addItem() {
    int row = orderTable->rowCount();
    orderTable->insertRow(row);
    QStringList defaults = {"Item Name", "1", "0.00", "0.00"};
    for(int i=0; i<4; ++i) {
        QTableWidgetItem *it = new QTableWidgetItem(defaults[i]);
        it->setForeground(Qt::black);
        orderTable->setItem(row, i, it);
    }
}

void MainWindow::updateGrandTotal() {
    float total = 0;
    for(int i=0; i<orderTable->rowCount(); ++i)
        if(orderTable->item(i,3)) total += orderTable->item(i,3)->text().toFloat();
    totalLabel->setText(QString("Total: %1 EGP").arg(QString::number(total, 'f', 2)));
}

void MainWindow::showReceiptPage() {
    receiptPreview->setHtml(generateReceiptHtml());
    stackedWidget->setCurrentIndex(1);
}

void MainWindow::showOrderPage() {
    orderTable->setRowCount(0);
    totalLabel->setText("Total: 0.00 EGP");
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::onLogout() { qApp->quit(); }

QString MainWindow::generateReceiptHtml() {
    QString rows = "";
    float total = 0;
    for(int i=0; i<orderTable->rowCount(); ++i) {
        QString n = orderTable->item(i,0)->text();
        QString q = orderTable->item(i,1)->text();
        QString t = orderTable->item(i,3)->text();
        total += t.toFloat();
        rows += QString("<tr><td>%1</td><td align='center'>%2</td><td align='right'>%3 EGP</td></tr>").arg(n).arg(q).arg(t);
    }
    return QString(R"(<html><body style='color:#2D1B18;'>
        <h2 align='center'>☕ Café Manager</h2>
        <hr><table width='100%'>%1</table><hr>
        <h3 align='right'>Grand Total: %2 EGP</h3>
        <p align='center'>Thank you!</p></body></html>)").arg(rows).arg(QString::number(total, 'f', 2));
}

MainWindow::~MainWindow() {}





https://github.com/areeggahmed-sudo/Areeg/tree/main