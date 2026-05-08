#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include "database.h"

class AddEmployeeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEmployeeDialog(QWidget *parent = nullptr);
    explicit AddEmployeeDialog(const Employee &emp, QWidget *parent = nullptr);

    Employee getEmployee() const;

private:
    void setupUi(const Employee &emp = {});

    QLineEdit *m_idEdit     = nullptr;
    QLineEdit *m_nameEdit   = nullptr;
    QComboBox *m_roleCombo  = nullptr;
    QLineEdit *m_emailEdit  = nullptr;
    QComboBox *m_statusCombo = nullptr;
    bool       m_editMode   = false;
};
