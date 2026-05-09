#ifndef STATCARD_H
#define STATCARD_H

#include <QWidget>
#include <QLabel>

class StatCard : public QWidget
{
    Q_OBJECT

public:
    explicit StatCard(const QString &title,
                      const QString &value,
                      const QString &subtitle,
                      const QString &iconSymbol,
                      const QColor &subtitleColor = QColor("#4CAF50"),
                      QWidget *parent = nullptr);

    void setValue(const QString &value);
    void setSubtitle(const QString &subtitle);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();

    QString m_title;
    QString m_iconSymbol;
    QColor m_subtitleColor;

    QLabel *m_titleLabel;
    QLabel *m_valueLabel;
    QLabel *m_subtitleLabel;
    QLabel *m_iconLabel;
};

#endif // STATCARD_H