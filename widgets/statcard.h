#ifndef STATCARD_H
#define STATCARD_H
#include <QWidget>
#include <QLabel>
#include <QPixmap>

class StatCard : public QWidget
{
    Q_OBJECT
public:
    explicit StatCard(const QString &title,
                      const QString &value,
                      const QString &subtitle,
                      const QString &iconPath,      // ← مسار صورة بدل Emoji
                      const QColor &subtitleColor = QColor("#4CAF50"),
                      QWidget *parent = nullptr);

    void setValue(const QString &value);
    void setSubtitle(const QString &subtitle);
    void setIcon(const QString &iconPath);           // ← function جديدة

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_title;
    QString m_iconPath;
    QColor m_subtitleColor;
    QLabel *m_titleLabel;
    QLabel *m_valueLabel;
    QLabel *m_subtitleLabel;
    QLabel *m_iconLabel;
};
#endif // STATCARD_H