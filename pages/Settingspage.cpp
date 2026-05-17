#include "settingspage.h"
#include "../database/databasemanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QFrame>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QFileInfo>

SettingsPage::SettingsPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadSettingsFromDB();
}

void SettingsPage::setupUI()
{
    setStyleSheet("background: #FAF6F1;");

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
    QLabel *pageTitle = new QLabel("Settings");
    pageTitle->setStyleSheet(
        "font-size: 30px; font-weight: bold; color: #1A1A1A; background: transparent;");

    QLabel *pageSub = new QLabel("Manage your café settings and preferences");
    pageSub->setStyleSheet(
        "font-size: 14px; color: #888888; background: transparent;");

    mainLayout->addWidget(pageTitle);
    mainLayout->addWidget(pageSub);
    mainLayout->addSpacing(5);

    QString labelStyle =
        "font-size: 13px; font-weight: 600; color: #374151; background: transparent;";

    QString inputStyle = R"(
        QLineEdit {
            border: 1.5px solid #E5E7EB; border-radius: 8px;
            padding: 10px 14px; font-size: 13px;
            color: #1F2937; background: #FAF6F1;
        }
        QLineEdit:focus { border: 1.5px solid #E8882F; background: white; }
    )";

    auto makeShadow = [](QWidget *w) {
        QGraphicsDropShadowEffect *s = new QGraphicsDropShadowEffect(w);
        s->setBlurRadius(20);
        s->setColor(QColor(0, 0, 0, 15));
        s->setOffset(0, 4);
        w->setGraphicsEffect(s);
    };

    QHBoxLayout *rowLayout = new QHBoxLayout();
    rowLayout->setSpacing(20);
    rowLayout->setAlignment(Qt::AlignTop);

    // ============================================
    // LEFT: Café Information
    // ============================================
    QWidget *cafeCard = new QWidget();
    cafeCard->setStyleSheet("QWidget { background: white; border-radius: 16px; }");
    makeShadow(cafeCard);

    QVBoxLayout *cafeLayout = new QVBoxLayout(cafeCard);
    cafeLayout->setContentsMargins(28, 25, 28, 25);
    cafeLayout->setSpacing(16);

    // Café Information title with icon
    QHBoxLayout *cafeTitleLayout = new QHBoxLayout();
    QLabel *cafeTitleIcon = new QLabel();
    cafeTitleIcon->setFixedSize(24, 24);
    cafeTitleIcon->setPixmap(
        QPixmap(":/icons/coffee-cup (1).png")
            .scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    cafeTitleIcon->setStyleSheet("background: transparent; border: none;");
    QLabel *cafeTitle = new QLabel("Café Information");
    cafeTitle->setStyleSheet(
        "font-size: 17px; font-weight: bold; color: #1A1A1A; background: transparent;");
    cafeTitleLayout->addWidget(cafeTitleIcon);
    cafeTitleLayout->addWidget(cafeTitle);
    cafeTitleLayout->addStretch();
    cafeLayout->addLayout(cafeTitleLayout);

    QLabel *nameLabel = new QLabel("Café Name");
    nameLabel->setStyleSheet(labelStyle);
    m_cafeNameEdit = new QLineEdit();
    m_cafeNameEdit->setStyleSheet(inputStyle);
    cafeLayout->addWidget(nameLabel);
    cafeLayout->addWidget(m_cafeNameEdit);

    QLabel *addrLabel = new QLabel("Address");
    addrLabel->setStyleSheet(labelStyle);
    m_addressEdit = new QLineEdit();
    m_addressEdit->setStyleSheet(inputStyle);
    cafeLayout->addWidget(addrLabel);
    cafeLayout->addWidget(m_addressEdit);

    QLabel *phoneLabel = new QLabel("Phone Number");
    phoneLabel->setStyleSheet(labelStyle);
    m_phoneEdit = new QLineEdit();
    m_phoneEdit->setStyleSheet(inputStyle);
    cafeLayout->addWidget(phoneLabel);
    cafeLayout->addWidget(m_phoneEdit);

    QLabel *emailLabel = new QLabel("Email");
    emailLabel->setStyleSheet(labelStyle);
    m_emailEdit = new QLineEdit();
    m_emailEdit->setStyleSheet(inputStyle);
    cafeLayout->addWidget(emailLabel);
    cafeLayout->addWidget(m_emailEdit);

    // Logo Upload
    QLabel *logoLabel = new QLabel("Café Logo");
    logoLabel->setStyleSheet(labelStyle);
    cafeLayout->addWidget(logoLabel);

    QWidget *logoBox = new QWidget();
    logoBox->setFixedHeight(150);
    logoBox->setStyleSheet(R"(
        QWidget {
            background: #FAF6F1;
            border: 2px dashed #D4C5B0;
            border-radius: 12px;
        }
    )");

    QVBoxLayout *logoBoxLayout = new QVBoxLayout(logoBox);
    logoBoxLayout->setAlignment(Qt::AlignCenter);
    logoBoxLayout->setSpacing(8);

    // Upload icon from resources
    QLabel *uploadIcon = new QLabel();
    uploadIcon->setAlignment(Qt::AlignCenter);
    uploadIcon->setFixedSize(36, 36);
    uploadIcon->setPixmap(
        QPixmap(":/icons/visible.png")
            .scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    uploadIcon->setStyleSheet("background: transparent; border: none;");

    m_logoLabel = new QLabel("Upload your café logo");
    m_logoLabel->setAlignment(Qt::AlignCenter);
    m_logoLabel->setStyleSheet(
        "color: #888; font-size: 13px; background: transparent; border: none;");

    QLabel *logoHint = new QLabel("PNG, JPG up to 5MB");
    logoHint->setAlignment(Qt::AlignCenter);
    logoHint->setStyleSheet(
        "color: #AAA; font-size: 12px; background: transparent; border: none;");

    QPushButton *chooseFileBtn = new QPushButton("Choose File");
    chooseFileBtn->setFixedSize(110, 34);
    chooseFileBtn->setCursor(Qt::PointingHandCursor);
    chooseFileBtn->setStyleSheet(R"(
        QPushButton {
            background: white; color: #5D3A1A;
            border: 1px solid #D4C5B0; border-radius: 8px;
            font-size: 13px; font-weight: 500;
        }
        QPushButton:hover { background: #FAF6F1; border-color: #E8882F; }
    )");
    connect(chooseFileBtn, &QPushButton::clicked, this, &SettingsPage::onChooseLogo);

    logoBoxLayout->addWidget(uploadIcon);
    logoBoxLayout->addWidget(m_logoLabel);
    logoBoxLayout->addWidget(logoHint);
    logoBoxLayout->addWidget(chooseFileBtn, 0, Qt::AlignCenter);
    cafeLayout->addWidget(logoBox);
    cafeLayout->addSpacing(5);

    // Save Café Button with icon
    QPushButton *saveCafeBtn = new QPushButton("  Save Settings");
    saveCafeBtn->setFixedHeight(46);
    saveCafeBtn->setCursor(Qt::PointingHandCursor);
    saveCafeBtn->setIcon(QIcon(":/icons/settings.png"));
    saveCafeBtn->setIconSize(QSize(18, 18));
    saveCafeBtn->setStyleSheet(R"(
        QPushButton {
            background: #5D3A1A; color: white;
            border: none; border-radius: 10px;
            font-size: 14px; font-weight: bold;
        }
        QPushButton:hover { background: #432812; }
    )");
    connect(saveCafeBtn, &QPushButton::clicked, this, &SettingsPage::onSaveCafeSettings);
    cafeLayout->addWidget(saveCafeBtn);

    rowLayout->addWidget(cafeCard, 1);

    // ============================================
    // RIGHT: System Settings + Change Password
    // ============================================
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(20);
    rightLayout->setAlignment(Qt::AlignTop);

    // System Settings Card
    QWidget *systemCard = new QWidget();
    systemCard->setStyleSheet("QWidget { background: white; border-radius: 16px; }");
    makeShadow(systemCard);

    QVBoxLayout *systemLayout = new QVBoxLayout(systemCard);
    systemLayout->setContentsMargins(28, 25, 28, 25);
    systemLayout->setSpacing(16);

    // System Settings title with icon
    QHBoxLayout *systemTitleLayout = new QHBoxLayout();
    QLabel *systemTitleIcon = new QLabel();
    systemTitleIcon->setFixedSize(24, 24);
    systemTitleIcon->setPixmap(
        QPixmap(":/icons/settings.png")
            .scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    systemTitleIcon->setStyleSheet("background: transparent; border: none;");
    QLabel *systemTitle = new QLabel("System Settings");
    systemTitle->setStyleSheet(
        "font-size: 17px; font-weight: bold; color: #1A1A1A; background: transparent;");
    systemTitleLayout->addWidget(systemTitleIcon);
    systemTitleLayout->addWidget(systemTitle);
    systemTitleLayout->addStretch();
    systemLayout->addLayout(systemTitleLayout);

    QLabel *currencyLabel = new QLabel("Currency");
    currencyLabel->setStyleSheet(labelStyle);
    m_currencyCombo = new QComboBox();
    m_currencyCombo->addItems({"USD ($)", "EUR (€)", "GBP (£)", "EGP (£E)", "SAR (﷼)"});
    m_currencyCombo->setStyleSheet(R"(
        QComboBox {
            border: 1.5px solid #E5E7EB; border-radius: 8px;
            padding: 10px 14px; font-size: 13px;
            color: #1F2937; background: #FAF6F1;
        }
        QComboBox:focus { border: 1.5px solid #E8882F; }
        QComboBox::drop-down { border: none; width: 25px; }
        QComboBox QAbstractItemView {
            background: white; color: #1F2937;
            selection-background-color: #E8882F;
            selection-color: white;
        }
    )");
    systemLayout->addWidget(currencyLabel);
    systemLayout->addWidget(m_currencyCombo);

    QLabel *taxLabel = new QLabel("Tax Rate (%)");
    taxLabel->setStyleSheet(labelStyle);
    m_taxRateEdit = new QLineEdit();
    m_taxRateEdit->setStyleSheet(inputStyle);
    systemLayout->addWidget(taxLabel);
    systemLayout->addWidget(m_taxRateEdit);
    systemLayout->addSpacing(5);

    QPushButton *saveSystemBtn = new QPushButton("  Save System Settings");
    saveSystemBtn->setFixedHeight(46);
    saveSystemBtn->setCursor(Qt::PointingHandCursor);
    saveSystemBtn->setIcon(QIcon(":/icons/settings.png"));
    saveSystemBtn->setIconSize(QSize(18, 18));
    saveSystemBtn->setStyleSheet(R"(
        QPushButton {
            background: #5D3A1A; color: white;
            border: none; border-radius: 10px;
            font-size: 14px; font-weight: bold;
        }
        QPushButton:hover { background: #432812; }
    )");
    connect(saveSystemBtn, &QPushButton::clicked,
            this, &SettingsPage::onSaveSystemSettings);
    systemLayout->addWidget(saveSystemBtn);
    rightLayout->addWidget(systemCard);

    // Change Password Card
    QWidget *passCard = new QWidget();
    passCard->setStyleSheet("QWidget { background: white; border-radius: 16px; }");
    makeShadow(passCard);

    QVBoxLayout *passLayout = new QVBoxLayout(passCard);
    passLayout->setContentsMargins(28, 25, 28, 25);
    passLayout->setSpacing(16);

    // Change Password title with icon
    QHBoxLayout *passTitleLayout = new QHBoxLayout();
    QLabel *passTitleIcon = new QLabel();
    passTitleIcon->setFixedSize(24, 24);
    passTitleIcon->setPixmap(
        QPixmap(":/icons/badge.png")
            .scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation)
        );
    passTitleIcon->setStyleSheet("background: transparent; border: none;");
    QLabel *passTitle = new QLabel("Change Password");
    passTitle->setStyleSheet(
        "font-size: 17px; font-weight: bold; color: #1A1A1A; background: transparent;");
    passTitleLayout->addWidget(passTitleIcon);
    passTitleLayout->addWidget(passTitle);
    passTitleLayout->addStretch();
    passLayout->addLayout(passTitleLayout);

    QLabel *currPassLabel = new QLabel("Current Password");
    currPassLabel->setStyleSheet(labelStyle);
    m_currentPassEdit = new QLineEdit();
    m_currentPassEdit->setEchoMode(QLineEdit::Password);
    m_currentPassEdit->setPlaceholderText("Enter current password");
    m_currentPassEdit->setStyleSheet(inputStyle);
    passLayout->addWidget(currPassLabel);
    passLayout->addWidget(m_currentPassEdit);

    QLabel *newPassLabel = new QLabel("New Password");
    newPassLabel->setStyleSheet(labelStyle);
    m_newPassEdit = new QLineEdit();
    m_newPassEdit->setEchoMode(QLineEdit::Password);
    m_newPassEdit->setPlaceholderText("Enter new password");
    m_newPassEdit->setStyleSheet(inputStyle);
    passLayout->addWidget(newPassLabel);
    passLayout->addWidget(m_newPassEdit);

    QLabel *confirmPassLabel = new QLabel("Confirm New Password");
    confirmPassLabel->setStyleSheet(labelStyle);
    m_confirmPassEdit = new QLineEdit();
    m_confirmPassEdit->setEchoMode(QLineEdit::Password);
    m_confirmPassEdit->setPlaceholderText("Confirm new password");
    m_confirmPassEdit->setStyleSheet(inputStyle);
    passLayout->addWidget(confirmPassLabel);
    passLayout->addWidget(m_confirmPassEdit);
    passLayout->addSpacing(5);

    QPushButton *changePassBtn = new QPushButton("Change Password");
    changePassBtn->setFixedHeight(46);
    changePassBtn->setCursor(Qt::PointingHandCursor);
    changePassBtn->setIcon(QIcon(":/icons/badge.png"));
    changePassBtn->setIconSize(QSize(18, 18));
    changePassBtn->setStyleSheet(R"(
        QPushButton {
            background: #E8882F; color: white;
            border: none; border-radius: 10px;
            font-size: 14px; font-weight: bold;
        }
        QPushButton:hover { background: #D4781F; }
    )");
    connect(changePassBtn, &QPushButton::clicked,
            this, &SettingsPage::onChangePassword);
    passLayout->addWidget(changePassBtn);

    rightLayout->addWidget(passCard);
    rightLayout->addStretch();

    rowLayout->addLayout(rightLayout, 1);
    mainLayout->addLayout(rowLayout);
    mainLayout->addStretch();

    scrollArea->setWidget(content);
    outerLayout->addWidget(scrollArea);
}

// ==============================================
// Load Settings from DB
// ==============================================

void SettingsPage::loadSettingsFromDB()
{
    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery query(db);

    if (!query.exec("SELECT SettingKey, SettingValue FROM CafeSettings")) return;

    while (query.next()) {
        QString key   = query.value("SettingKey").toString();
        QString value = query.value("SettingValue").toString();

        if      (key == "CafeName") m_cafeNameEdit->setText(value);
        else if (key == "Address")  m_addressEdit->setText(value);
        else if (key == "Phone")    m_phoneEdit->setText(value);
        else if (key == "Email")    m_emailEdit->setText(value);
        else if (key == "Currency") {
            int idx = m_currencyCombo->findText(value);
            if (idx >= 0) m_currencyCombo->setCurrentIndex(idx);
        }
        else if (key == "TaxRate")  m_taxRateEdit->setText(value);
        else if (key == "LogoPath" && !value.isEmpty()) {
            QFileInfo fi(value);
            m_logoLabel->setText("✓ " + fi.fileName());
            m_logoLabel->setStyleSheet(
                "color: #16A34A; font-size: 13px; font-weight: 600; "
                "background: transparent; border: none;");
            m_logoPath = value;
        }
    }
}

// ==============================================
// Save Setting Helper
// ==============================================

void SettingsPage::saveSettingToDB(const QString &key, const QString &value)
{
    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery query(db);

    query.prepare(R"(
        IF EXISTS (SELECT 1 FROM CafeSettings WHERE SettingKey = ?)
            UPDATE CafeSettings SET SettingValue = ? WHERE SettingKey = ?
        ELSE
            INSERT INTO CafeSettings (SettingKey, SettingValue) VALUES (?, ?)
    )");
    query.addBindValue(key);
    query.addBindValue(value);
    query.addBindValue(key);
    query.addBindValue(key);
    query.addBindValue(value);
    query.exec();
}

// ==============================================
// Slots
// ==============================================

void SettingsPage::onSaveCafeSettings()
{
    if (m_cafeNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Please enter café name.");
        return;
    }

    saveSettingToDB("CafeName", m_cafeNameEdit->text().trimmed());
    saveSettingToDB("Address",  m_addressEdit->text().trimmed());
    saveSettingToDB("Phone",    m_phoneEdit->text().trimmed());
    saveSettingToDB("Email",    m_emailEdit->text().trimmed());

    QMessageBox::information(this, "Success", "Café settings saved successfully!");
}

void SettingsPage::onSaveSystemSettings()
{
    bool ok;
    double tax = m_taxRateEdit->text().toDouble(&ok);
    if (!ok || tax < 0 || tax > 100) {
        QMessageBox::warning(this, "Validation",
                             "Please enter a valid tax rate (0-100).");
        return;
    }

    saveSettingToDB("Currency", m_currencyCombo->currentText());
    saveSettingToDB("TaxRate",  m_taxRateEdit->text().trimmed());

    QMessageBox::information(this, "Success", "System settings saved successfully!");
}

void SettingsPage::onChangePassword()
{
    QString current = m_currentPassEdit->text();
    QString newPass = m_newPassEdit->text();
    QString confirm = m_confirmPassEdit->text();

    if (current.isEmpty() || newPass.isEmpty() || confirm.isEmpty()) {
        QMessageBox::warning(this, "Validation", "Please fill in all password fields.");
        return;
    }

    if (newPass != confirm) {
        QMessageBox::warning(this, "Validation",
                             "New password and confirm password do not match.");
        return;
    }

    if (newPass.length() < 3) {
        QMessageBox::warning(this, "Validation",
                             "Password must be at least 3 characters.");
        return;
    }

    QSqlDatabase db = QSqlDatabase::database("cafe_manager_connection");
    QSqlQuery query(db);

    query.prepare("SELECT user_id FROM users WHERE password = ?");
    query.addBindValue(current);

    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "Error", "Current password is incorrect.");
        return;
    }

    int userId = query.value("user_id").toInt();

    QSqlQuery updateQuery(db);
    updateQuery.prepare("UPDATE users SET password = ? WHERE user_id = ?");
    updateQuery.addBindValue(newPass);
    updateQuery.addBindValue(userId);

    if (updateQuery.exec()) {
        m_currentPassEdit->clear();
        m_newPassEdit->clear();
        m_confirmPassEdit->clear();
        QMessageBox::information(this, "Success", "Password changed successfully!");
    } else {
        QMessageBox::critical(this, "Error",
                              "Failed to change password.\n" + updateQuery.lastError().text());
    }
}

void SettingsPage::onChooseLogo()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Choose Logo", "",
        "Images (*.png *.jpg *.jpeg)"
        );

    if (!path.isEmpty()) {
        m_logoPath = path;
        QFileInfo fi(path);
        m_logoLabel->setText("✓ " + fi.fileName());
        m_logoLabel->setStyleSheet(
            "color: #16A34A; font-size: 13px; font-weight: 600; "
            "background: transparent; border: none;");

        saveSettingToDB("LogoPath", path);
    }
}