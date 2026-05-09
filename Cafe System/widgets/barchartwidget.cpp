#include "barchartwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <cmath>

BarChartWidget::BarChartWidget(const QString &title, QWidget *parent)
    : QWidget(parent), m_title(title)
{
    setMinimumSize(400, 350);

    m_labels = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 20));
    shadow->setOffset(0, 4);
    setGraphicsEffect(shadow);
}

void BarChartWidget::setData(const QList<int> &data)
{
    m_data = data;
    update();
}

void BarChartWidget::setLabels(const QStringList &labels)
{
    m_labels = labels;
    update();
}

void BarChartWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    // Background card
    QPainterPath bgPath;
    bgPath.addRoundedRect(rect(), 16, 16);
    painter.fillPath(bgPath, QColor("#FFFFFF"));
    painter.setPen(QPen(QColor(0, 0, 0, 15), 1));
    painter.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 16, 16);

    // Title
    QFont titleFont("Segoe UI", 16, QFont::DemiBold);
    painter.setFont(titleFont);
    painter.setPen(QColor("#333333"));
    painter.drawText(25, 40, m_title);

    if (m_data.isEmpty()) return;

    // Chart area
    int chartLeft = 65;
    int chartTop = 65;
    int chartRight = width() - 30;
    int chartBottom = height() - 50;
    int chartWidth = chartRight - chartLeft;
    int chartHeight = chartBottom - chartTop;

    // Find max value
    int maxVal = *std::max_element(m_data.begin(), m_data.end());
    maxVal = (int)(std::ceil(maxVal / 2000.0) * 2000.0);

    // Draw grid lines and Y-axis labels
    QFont labelFont("Segoe UI", 10);
    painter.setFont(labelFont);

    int ySteps = 4;
    for (int i = 0; i <= ySteps; i++) {
        double val = ((double)maxVal / ySteps) * i;
        int y = chartBottom - (chartHeight * i / ySteps);

        painter.setPen(QPen(QColor(0, 0, 0, 20), 1, Qt::DashLine));
        painter.drawLine(chartLeft, y, chartRight, y);

        painter.setPen(QColor("#999999"));
        painter.drawText(5, y - 8, 55, 20, Qt::AlignRight | Qt::AlignVCenter,
                         QString::number((int)val));
    }

    // Draw bars
    int barCount = m_data.size();
    double totalBarWidth = chartWidth / barCount;
    double barWidth = totalBarWidth * 0.55;
    double gap = (totalBarWidth - barWidth) / 2.0;

    for (int i = 0; i < barCount; i++) {
        double x = chartLeft + i * totalBarWidth + gap;
        double barHeight = ((double)m_data[i] / maxVal) * chartHeight;
        double y = chartBottom - barHeight;

        // Bar with gradient
        QLinearGradient barGradient(x, y, x, chartBottom);
        barGradient.setColorAt(0, QColor("#5C3D1A"));
        barGradient.setColorAt(1, QColor("#3D2810"));

        QPainterPath barPath;
        barPath.addRoundedRect(QRectF(x, y, barWidth, barHeight), 4, 4);
        painter.fillPath(barPath, barGradient);

        // X label
        painter.setPen(QColor("#999999"));
        if (i < m_labels.size()) {
            painter.drawText((int)x - 5, chartBottom + 5, (int)barWidth + 10, 25,
                             Qt::AlignCenter, m_labels[i]);
        }
    }
}