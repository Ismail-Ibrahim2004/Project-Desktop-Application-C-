#ifndef BARCHARTWIDGET_H
#define BARCHARTWIDGET_H

#include <QWidget>
#include <QList>

class BarChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BarChartWidget(const QString &title, QWidget *parent = nullptr);

    void setData(const QList<int> &data);
    void setLabels(const QStringList &labels);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_title;
    QList<int> m_data;
    QStringList m_labels;
};

#endif // BARCHARTWIDGET_H