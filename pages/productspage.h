#ifndef PRODUCTSPAGE_H
#define PRODUCTSPAGE_H

#include <QWidget>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
class QStackedWidget;
class QTableWidget;
class QLineEdit;
class QComboBox;
class QPushButton;
class QLabel;
class QTextEdit;
QT_END_NAMESPACE

class ProductsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ProductsPage(QWidget *parent = nullptr);
    ~ProductsPage();

    void refreshData();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void showProductsList();
    void showAddProduct();
    void showEditProduct();
    void saveProduct();
    void chooseImage();
    void goBack();
    void searchProducts();
    void filterCategory();

private:
    void setupUI();
    void setupProductsPage();
    void setupFormPage();
    void loadProducts();
    void clearForm();
    QString generateProductCode();

    // UI Components
    QStackedWidget *stack;
    QWidget *productsPage;
    QWidget *formPage;
    QTableWidget *productTable;

    // Filters
    QLineEdit *searchEdit;
    QComboBox *categoryFilter;

    // Form
    QLineEdit *nameEdit;
    QComboBox *categoryEdit;
    QLineEdit *priceEdit;
    QTextEdit *descEdit;
    QLabel *imageLabel;
    QComboBox *statusEdit;
    QPushButton *saveBtn;
    QLabel *formTitle;
    QLabel *formSubtitle;
    QLabel *pageTitle;
    QLabel *pageSubtitle;

    // State
    bool isEditing;
    int editingId;
    QString currentImagePath;
};

#endif // PRODUCTSPAGE_H