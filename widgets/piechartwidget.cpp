#include "piechartwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QtMath>

PieChartWidget::PieChartWidget(const QString &title, QWidget *parent)
    : QWidget(parent), m_title(title)
{
    setMinimumSize(400, 400);

    m_colors = {
        QColor("#5D3A1A"),   // Coffee
        QColor("#C9A88B"),   // Tea
        QColor("#E67E22"),   // Desserts
        QColor("#8B5A2B")    // Cold Drinks
    };

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 20));
    shadow->setOffset(0, 4);
    setGraphicsEffect(shadow);
}

void PieChartWidget::setData(const QList<QPair<QString, double>> &data)
{
    m_data = data;
    update();
}

void PieChartWidget::setColors(const QList<QColor> &colors)
{
    m_colors = colors;
    update();
}

void PieChartWidget::paintEvent(QPaintEvent *event)
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
    QFont titleFont("Segoe UI", 14, QFont::DemiBold);
    painter.setFont(titleFont);
    painter.setPen(QColor("#333333"));
    painter.drawText(25, 35, m_title);

    if (m_data.isEmpty()) return;

    double total = 0;
    for (const auto &d : m_data) total += d.second;
    if (total == 0) return;

    int chartSize = qMin(width(), height()) - 140;
    int centerX = width() / 2;
    int centerY = height() / 2 + 10;

    QRectF pieRect(centerX - chartSize / 2, centerY - chartSize / 2, chartSize, chartSize);

    int startAngle = 90 * 16;

    for (int i = 0; i < m_data.size(); i++) {
        double percentage = (m_data[i].second / total) * 100.0;
        int spanAngle = -static_cast<int>((m_data[i].second / total) * 360 * 16);

        QColor color = m_colors[i % m_colors.size()];
        painter.setBrush(color);
        painter.setPen(QPen(Qt::white, 2));
        painter.drawPie(pieRect, startAngle, spanAngle);

        double midAngleRad = qDegreesToRadians(
            (90.0 - ((startAngle + spanAngle / 2.0) / 16.0))
            );

        double labelDistance = chartSize / 2.0 + 35;
        double labelX = centerX + labelDistance * qSin(midAngleRad);
        double labelY = centerY - labelDistance * qCos(midAngleRad);

        QString labelText = QString("%1 %2%")
                                .arg(m_data[i].first)
                                .arg(QString::number(percentage, 'f', 0));

        QFont labelFont("Segoe UI", 11);
        painter.setFont(labelFont);
        painter.setPen(QColor("#333333"));

        QFontMetrics fm(labelFont);
        int textWidth = fm.horizontalAdvance(labelText);

        QRectF textRect;
        if (qSin(midAngleRad) > 0.1) {
            textRect = QRectF(labelX - 5, labelY - 10, textWidth + 10, 20);
            painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, labelText);
        } else if (qSin(midAngleRad) < -0.1) {
            textRect = QRectF(labelX - textWidth - 5, labelY - 10, textWidth + 10, 20);
            painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, labelText);
        } else {
            textRect = QRectF(labelX - textWidth / 2.0, labelY - 10, textWidth, 20);
            painter.drawText(textRect, Qt::AlignCenter, labelText);
        }

        startAngle += spanAngle;
    }
}