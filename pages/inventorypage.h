#ifndef INVENTORYPAGE_H
#define INVENTORYPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialog>
#include <QComboBox>
#include <QDoubleSpinBox>

// =============================================
// InventoryDialog
// =============================================
class InventoryDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InventoryDialog(QWidget *parent = nullptr, bool isEdit = false);

    QString getName()       const;
    double  getQuantity()   const;
    QString getUnit()       const;
    double  getMinQuantity()const;
    QString getSupplier()   const;

    void setData(int id, const QString &name, double quantity,
                 const QString &unit, double minQuantity, const QString &supplier);
    int getItemId() const { return m_itemId; }

private:
    void setupUI(bool isEdit);

    int           m_itemId = -1;
    QLineEdit    *m_nameEdit;
    QDoubleSpinBox *m_quantitySpin;
    QComboBox    *m_unitCombo;
    QDoubleSpinBox *m_minQuantitySpin;
    QLineEdit    *m_supplierEdit;
};

// =============================================
// InventoryPage
// =============================================
class InventoryPage : public QWidget
{
    Q_OBJECT
public:
    explicit InventoryPage(QWidget *parent = nullptr);
    void loadInventory(const QString &searchTerm = "");

private slots:
    void onAddItem();
    void onEditItem(int itemId);
    void onDeleteItem(int itemId, const QString &name);
    void onSearchChanged(const QString &text);

private:
    void setupUI();
    void setupTable();
    void addTableRow(int row, int id, const QString &name, double quantity,
                     const QString &unit, double minQuantity,
                     const QString &supplier, const QString &lastRestocked);
    void updateLowStockBanner();

    QLineEdit    *m_searchEdit;
    QTableWidget *m_table;
    QLabel       *m_lowStockLabel;
    QWidget      *m_lowStockBanner;
};

#endif // INVENTORYPAGE_H
