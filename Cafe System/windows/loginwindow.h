#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QRadioButton>

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void handleLogin();

private:
    QLineEdit *userEdit;
    QLineEdit *passEdit;
    QRadioButton *adminRadio;
    QRadioButton *empRadio;

    QString currentUsername;   // ⭐ لحفظ اسم المستخدم الحالي
};

#endif // LOGINWINDOW_H