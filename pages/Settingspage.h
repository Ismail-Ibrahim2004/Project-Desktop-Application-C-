#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>

class SettingsPage : public QWidget
{
    Q_OBJECT
public:
    explicit SettingsPage(QWidget *parent = nullptr);

private slots:
    void onSaveCafeSettings();
    void onSaveSystemSettings();
    void onChangePassword();
    void onChooseLogo();

private:
    void setupUI();
    void loadSettingsFromDB();
    void saveSettingToDB(const QString &key, const QString &value);

    // Café Information
    QLineEdit   *m_cafeNameEdit;
    QLineEdit   *m_addressEdit;
    QLineEdit   *m_phoneEdit;
    QLineEdit   *m_emailEdit;
    QLabel      *m_logoLabel;
    QString      m_logoPath;

    // System Settings
    QComboBox   *m_currencyCombo;
    QLineEdit   *m_taxRateEdit;

    // Change Password
    QLineEdit   *m_currentPassEdit;
    QLineEdit   *m_newPassEdit;
    QLineEdit   *m_confirmPassEdit;
};

#endif // SETTINGSPAGE_H