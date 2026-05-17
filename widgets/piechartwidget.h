#ifndef PIECHARTWIDGET_H
#define PIECHARTWIDGET_H

#include <QWidget>
#include <QList>
#include <QPair>
#include <QColor>

class PieChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PieChartWidget(const QString &title, QWidget *parent = nullptr);

    void setData(const QList<QPair<QString, double>> &data);
    void setColors(const QList<QColor> &colors);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_title;
    QList<QPair<QString, double>> m_data;
    QList<QColor> m_colors;
};

#endif // PIECHARTWIDGET_H