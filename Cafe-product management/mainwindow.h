#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void showProducts();
    void showAddProduct();
    void showEditProduct();
    void saveProduct();
    void chooseImage();
    void goBack();
    void searchProducts();
    void filterCategory();

private:
    void setupUI();
    void setupDB();
    void loadProducts();
    void setupProductsPage();
    void setupFormPage();
    void clearForm();
    QString generateProductId();

    QSqlDatabase db;
    QStackedWidget *stack;
    QWidget *productsPage;
    QWidget *formPage;
    QTableWidget *productTable;
    QLineEdit *searchEdit;
    QComboBox *categoryFilter;
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
    bool isEditing;
    int editingId;
    QString currentImagePath;
};

#endif
