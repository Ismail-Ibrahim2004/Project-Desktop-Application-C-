#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QList>

class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWidget(const QString &title, QWidget *parent = nullptr);

    void setData(const QList<double> &data);
    void setLabels(const QStringList &labels);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_title;
    QList<double> m_data;
    QStringList m_labels;
};

#endif // CHARTWIDGET_H