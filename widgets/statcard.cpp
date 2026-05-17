#include "statcard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>

StatCard::StatCard(const QString &title,
                   const QString &value,
                   const QString &subtitle,
                   const QString &iconPath,
                   const QColor &subtitleColor,
                   QWidget *parent)
    : QWidget(parent),
    m_title(title),
    m_iconPath(iconPath),
    m_subtitleColor(subtitleColor)
{
    setMinimumSize(220, 130);
    setMaximumHeight(150);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 25));
    shadow->setOffset(0, 4);
    setGraphicsEffect(shadow);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(22, 18, 22, 18);
    mainLayout->setSpacing(6);

    // Top row: title + icon
    QHBoxLayout *topLayout = new QHBoxLayout();

    m_titleLabel = new QLabel(title);
    m_titleLabel->setStyleSheet("color: #888888; font-size: 14px; font-weight: 500; background: transparent;");

    m_iconLabel = new QLabel();
    m_iconLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_iconLabel->setFixedSize(36, 36);
    m_iconLabel->setStyleSheet(
        "background: #FFF0E0; border-radius: 8px; padding: 4px;"
        );

    QPixmap pix(iconPath);
    if (!pix.isNull()) {
        m_iconLabel->setPixmap(
            pix.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
    }

    topLayout->addWidget(m_titleLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_iconLabel);
    mainLayout->addLayout(topLayout);

    // Value
    m_valueLabel = new QLabel(value);
    m_valueLabel->setStyleSheet("color: #1A1A1A; font-size: 30px; font-weight: bold; background: transparent;");
    mainLayout->addWidget(m_valueLabel);

    // Subtitle
    m_subtitleLabel = new QLabel(subtitle);
    m_subtitleLabel->setStyleSheet(
        QString("color: %1; font-size: 13px; font-weight: 400; background: transparent;")
            .arg(subtitleColor.name())
        );
    mainLayout->addWidget(m_subtitleLabel);
    mainLayout->addStretch();
}

void StatCard::setValue(const QString &value)
{
    m_valueLabel->setText(value);
}

void StatCard::setSubtitle(const QString &subtitle)
{
    m_subtitleLabel->setText(subtitle);
}

void StatCard::setIcon(const QString &iconPath)
{
    QPixmap pix(iconPath);
    if (!pix.isNull()) {
        m_iconLabel->setPixmap(
            pix.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
    }
}

void StatCard::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Card background
    QPainterPath path;
    path.addRoundedRect(rect(), 16, 16);
    painter.fillPath(path, QColor("#FFFFFF"));

    // Subtle border
    painter.setPen(QPen(QColor(0, 0, 0, 15), 1));
    painter.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 16, 16);
}