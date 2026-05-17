#ifndef EMPLOYEESPAGE_H
#define EMPLOYEESPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>
#include <QComboBox>
#include <QCheckBox>

// =============================================
// EmployeeDialog
// =============================================
class EmployeeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit EmployeeDialog(QWidget *parent = nullptr, bool isEdit = false);

    QString getName()     const;
    QString getRole()     const;
    QString getEmail()    const;
    QString getPhone()    const;
    bool    getIsActive() const;

    void setData(int id, const QString &name, const QString &role,
                 const QString &email, const QString &phone, bool isActive);

    int getEmployeeId() const { return m_employeeId; }

private:
    void setupUI(bool isEdit);

    int         m_employeeId = -1;
    QLineEdit  *m_nameEdit;
    QComboBox  *m_roleCombo;
    QLineEdit  *m_emailEdit;
    QLineEdit  *m_phoneEdit;
    QCheckBox  *m_activeCheck;
};

// =============================================
// EmployeesPage
// =============================================
class EmployeesPage : public QWidget
{
    Q_OBJECT
public:
    explicit EmployeesPage(QWidget *parent = nullptr);
    void loadEmployees(const QString &searchTerm = "");

private slots:
    void onAddEmployee();
    void onEditEmployee(int employeeId);
    void onDeleteEmployee(int employeeId, const QString &name);
    void onSearchChanged(const QString &text);

private:
    void setupUI();
    void setupTable();
    void addTableRow(int row, int id, const QString &name,
                     const QString &role, const QString &email,
                     bool isActive);

    QLineEdit    *m_searchEdit;
    QTableWidget *m_table;
    QLabel       *m_countLabel;
};

#endif // EMPLOYEESPAGE_H