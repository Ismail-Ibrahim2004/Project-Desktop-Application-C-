#include "addemployeedialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QUuid>

AddEmployeeDialog::AddEmployeeDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Add Employee");
    setupUi();
}

AddEmployeeDialog::AddEmployeeDialog(const Employee &emp, QWidget *parent)
    : QDialog(parent), m_editMode(true)
{
    setWindowTitle("Edit Employee");
    setupUi(emp);
}

void AddEmployeeDialog::setupUi(const Employee &emp)
{
    setMinimumWidth(400);
    setObjectName("employeeDialog");

    QVBoxLayout *main = new QVBoxLayout(this);
    main->setSpacing(16);
    main->setContentsMargins(24, 24, 24, 24);

    QLabel *heading = new QLabel(m_editMode ? "Edit Employee" : "New Employee");
    heading->setObjectName("dialogHeading");
    main->addWidget(heading);

    QFormLayout *form = new QFormLayout;
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight);

    m_idEdit = new QLineEdit(emp.id.isEmpty()
        ? QString("EMP-%1").arg(QUuid::createUuid().toString(QUuid::Id128).left(4).toUpper())
        : emp.id);
    m_idEdit->setReadOnly(m_editMode);
    m_idEdit->setObjectName("formInput");

    m_nameEdit = new QLineEdit(emp.name);
    m_nameEdit->setObjectName("formInput");
    m_nameEdit->setPlaceholderText("Full name");

    m_roleCombo = new QComboBox;
    m_roleCombo->setObjectName("formInput");
    m_roleCombo->addItems({"Barista", "Cashier", "Shift Manager", "Kitchen Staff", "Delivery"});
    if (!emp.role.isEmpty()) {
        int idx = m_roleCombo->findText(emp.role);
        if (idx >= 0) m_roleCombo->setCurrentIndex(idx);
    }

    m_emailEdit = new QLineEdit(emp.email);
    m_emailEdit->setObjectName("formInput");
    m_emailEdit->setPlaceholderText("email@cafe.com");

    m_statusCombo = new QComboBox;
    m_statusCombo->setObjectName("formInput");
    m_statusCombo->addItems({"Active", "Inactive"});
    if (!emp.status.isEmpty()) {
        int idx = m_statusCombo->findText(emp.status);
        if (idx >= 0) m_statusCombo->setCurrentIndex(idx);
    }

    form->addRow("Employee ID:", m_idEdit);
    form->addRow("Name:",        m_nameEdit);
    form->addRow("Role:",        m_roleCombo);
    form->addRow("Email:",       m_emailEdit);
    form->addRow("Status:",      m_statusCombo);

    main->addLayout(form);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    buttons->setObjectName("dialogButtons");
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    main->addWidget(buttons);
}

Employee AddEmployeeDialog::getEmployee() const
{
    Employee e;
    e.id     = m_idEdit->text().trimmed();
    e.name   = m_nameEdit->text().trimmed();
    e.role   = m_roleCombo->currentText();
    e.email  = m_emailEdit->text().trimmed();
    e.status = m_statusCombo->currentText();
    return e;
}
