#include "sidebar.h"
#include <QPainter>
#include <QFont>
#include <QHBoxLayout>

// =====================================================
// SidebarButton
// =====================================================

SidebarButton::SidebarButton(const QString &iconPath, const QString &text, QWidget *parent)
    : QPushButton(text, parent), m_iconPath(iconPath)
{
    setFixedHeight(48);
    setCursor(Qt::PointingHandCursor);
    setCheckable(true);
    setLayoutDirection(Qt::LeftToRight);

    QFont btnFont("Segoe UI", 11);
    btnFont.setWeight(QFont::Medium);
    setFont(btnFont);

    setActive(false);
}

void SidebarButton::setActive(bool active)
{
    m_active = active;

    if (active) {
        // ===== ACTIVE STATE - Orange =====
        setStyleSheet(R"(
            QPushButton {
                border: none;
                border-radius: 10px;
                padding-left: 18px;
                padding-right: 10px;
                text-align: left;
                color: #FFFFFF;
                background-color: #E8882F;
                font-weight: 600;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: #E8882F;
            }
        )");
    } else {
        // ===== NORMAL STATE - White text =====
        setStyleSheet(R"(
            QPushButton {
                border: none;
                border-radius: 10px;
                padding-left: 18px;
                padding-right: 10px;
                text-align: left;
                color: #FFFFFF;
                background-color: transparent;
                font-weight: 500;
                font-size: 14px;
            }
            QPushButton:hover {
                background-color: rgba(255, 255, 255, 0.10);
            }
        )");
    }
    update();
}

void SidebarButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
}

// =====================================================
// Sidebar
// =====================================================

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void Sidebar::setupUI()
{
    setFixedWidth(265);
    setLayoutDirection(Qt::LeftToRight);

    // ===== Sidebar Background - LIGHTER Brown like the original =====
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor("#6B4423"));  // ⭐ بني أفتح
    setPalette(pal);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 22, 15, 22);
    mainLayout->setSpacing(5);

    // ============================================
    // 🪪 Logo Section
    // ============================================
    QWidget *logoContainer = new QWidget();
    logoContainer->setStyleSheet("background: transparent;");
    QHBoxLayout *logoLayout = new QHBoxLayout(logoContainer);
    logoLayout->setContentsMargins(5, 5, 5, 5);
    logoLayout->setSpacing(12);

    // Coffee Cup Icon (Orange Background)
    QLabel *logoIcon = new QLabel("☕");
    logoIcon->setFixedSize(45, 45);
    logoIcon->setAlignment(Qt::AlignCenter);
    QFont iconFont;
    iconFont.setPointSize(20);
    logoIcon->setFont(iconFont);
    logoIcon->setStyleSheet(R"(
        QLabel {
            background-color: #E8882F;
            border-radius: 10px;
            color: white;
        }
    )");

    // Title Container
    QWidget *textContainer = new QWidget();
    textContainer->setStyleSheet("background: transparent;");
    QVBoxLayout *logoTextLayout = new QVBoxLayout(textContainer);
    logoTextLayout->setContentsMargins(0, 0, 0, 0);
    logoTextLayout->setSpacing(0);

    // "Cafe Manager" Title
    QLabel *titleLabel = new QLabel("Cafe Manager");
    QFont titleFont("Segoe UI", 14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #FFFFFF; background: transparent;");

    // "Admin" Subtitle
    QLabel *subtitleLabel = new QLabel("Admin");
    QFont subFont("Segoe UI", 10);
    subtitleLabel->setFont(subFont);
    subtitleLabel->setStyleSheet("color: rgba(255, 255, 255, 0.7); background: transparent;");

    logoTextLayout->addWidget(titleLabel);
    logoTextLayout->addWidget(subtitleLabel);

    logoLayout->addWidget(logoIcon);
    logoLayout->addWidget(textContainer);
    logoLayout->addStretch();

    mainLayout->addWidget(logoContainer);
    mainLayout->addSpacing(30);

    // ============================================
    // 📋 Menu Buttons (with clearer icons)
    // ============================================
    m_menuLayout = new QVBoxLayout();
    m_menuLayout->setSpacing(4);
    m_menuLayout->setContentsMargins(0, 0, 0, 0);

    createMenuButton("", "  ▦   Dashboard", Dashboard);
    createMenuButton("", "  ☕  Products", Products);
    createMenuButton("", "  🛒  Orders", Orders);
    createMenuButton("", "  📦  Inventory", Inventory);
    createMenuButton("", "  👥  Employees", Employees);
    createMenuButton("", "  📊  Reports", Reports);
    createMenuButton("", "  ⚙   Settings", Settings);

    mainLayout->addLayout(m_menuLayout);

    // Set first button active
    if (!m_buttons.isEmpty()) {
        m_buttons[0]->setActive(true);
    }

    mainLayout->addStretch();

    // ============================================
    // 🚪 Logout Button - WHITE color
    // ============================================
    QPushButton *logoutBtn = new QPushButton("  ⮕   Logout");
    logoutBtn->setFixedHeight(48);
    logoutBtn->setCursor(Qt::PointingHandCursor);
    QFont logoutFont("Segoe UI", 11);
    logoutFont.setWeight(QFont::Medium);
    logoutBtn->setFont(logoutFont);

    logoutBtn->setStyleSheet(R"(
        QPushButton {
            border: none;
            border-radius: 10px;
            padding-left: 18px;
            padding-right: 10px;
            text-align: left;
            color: #FFFFFF;
            background-color: transparent;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: rgba(255, 100, 100, 0.20);
            color: #FFAAAA;
        }
    )");

    connect(logoutBtn, &QPushButton::clicked, this, &Sidebar::logoutClicked);
    mainLayout->addWidget(logoutBtn);
}

void Sidebar::createMenuButton(const QString &icon, const QString &text, Page page)
{
    Q_UNUSED(icon);
    SidebarButton *btn = new SidebarButton("", text);
    m_buttons.append(btn);
    m_menuLayout->addWidget(btn);

    connect(btn, &QPushButton::clicked, this, [this, page]() {
        setActivePage(page);
        emit pageChanged(static_cast<int>(page));
    });
}

void Sidebar::setActivePage(Page page)
{
    m_currentPage = page;
    for (int i = 0; i < m_buttons.size(); i++) {
        m_buttons[i]->setActive(i == static_cast<int>(page));
    }
}