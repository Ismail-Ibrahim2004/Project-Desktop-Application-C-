#include "chartwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <cmath>

ChartWidget::ChartWidget(const QString &title, QWidget *parent)
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

void ChartWidget::setData(const QList<double> &data)
{
    m_data = data;
    update();
}

void ChartWidget::setLabels(const QStringList &labels)
{
    m_labels = labels;
    update();
}

void ChartWidget::paintEvent(QPaintEvent *event)
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
    double maxVal = *std::max_element(m_data.begin(), m_data.end());
    maxVal = std::ceil(maxVal / 2000.0) * 2000.0;  // Round up to nearest 2000

    // Draw grid lines and Y-axis labels
    painter.setPen(QPen(QColor(0, 0, 0, 30), 1));
    QFont labelFont("Segoe UI", 10);
    painter.setFont(labelFont);

    int ySteps = 4;
    for (int i = 0; i <= ySteps; i++) {
        double val = (maxVal / ySteps) * i;
        int y = chartBottom - (chartHeight * i / ySteps);

        // Grid line
        painter.setPen(QPen(QColor(0, 0, 0, 20), 1, Qt::DashLine));
        painter.drawLine(chartLeft, y, chartRight, y);

        // Y label
        painter.setPen(QColor("#999999"));
        painter.drawText(5, y - 8, 55, 20, Qt::AlignRight | Qt::AlignVCenter,
                         QString::number((int)val));
    }

    // Draw X-axis labels
    int pointCount = m_data.size();
    double xStep = (double)chartWidth / (pointCount - 1);

    for (int i = 0; i < m_labels.size() && i < pointCount; i++) {
        int x = chartLeft + (int)(i * xStep);
        painter.setPen(QColor("#999999"));
        painter.drawText(x - 20, chartBottom + 5, 40, 25,
                         Qt::AlignCenter, m_labels[i]);
    }

    // Draw line
    QPainterPath linePath;
    QList<QPointF> points;

    for (int i = 0; i < pointCount; i++) {
        double x = chartLeft + i * xStep;
        double y = chartBottom - (m_data[i] / maxVal) * chartHeight;
        points.append(QPointF(x, y));
    }

    if (!points.isEmpty()) {
        linePath.moveTo(points[0]);
        for (int i = 1; i < points.size(); i++) {
            // Smooth curve using cubic bezier
            QPointF prev = points[i-1];
            QPointF curr = points[i];
            double ctrlX = (prev.x() + curr.x()) / 2.0;
            linePath.cubicTo(ctrlX, prev.y(), ctrlX, curr.y(), curr.x(), curr.y());
        }

        // Draw line
        painter.setPen(QPen(QColor("#E8882F"), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(linePath);

        // Draw gradient fill under line
        QPainterPath fillPath = linePath;
        fillPath.lineTo(points.last().x(), chartBottom);
        fillPath.lineTo(points.first().x(), chartBottom);
        fillPath.closeSubpath();

        QLinearGradient gradient(0, chartTop, 0, chartBottom);
        gradient.setColorAt(0, QColor(232, 136, 47, 40));
        gradient.setColorAt(1, QColor(232, 136, 47, 5));
        painter.fillPath(fillPath, gradient);

        // Draw points
        for (const QPointF &pt : points) {
            painter.setPen(QPen(QColor("#E8882F"), 2));
            painter.setBrush(QColor("white"));
            painter.drawEllipse(pt, 5, 5);
            painter.setBrush(QColor("#E8882F"));
            painter.drawEllipse(pt, 3, 3);
        }
    }
}