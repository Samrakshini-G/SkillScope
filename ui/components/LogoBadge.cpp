#include "LogoBadge.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>

LogoBadge::LogoBadge(int size, QWidget* parent)
    : QWidget(parent), m_size(size) {
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setFixedSize(m_size, m_size);
}

void LogoBadge::setBadgeSize(int size) {
    m_size = size;
    setFixedSize(m_size, m_size);
    update();
}

void LogoBadge::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect(0, 0, width(), height());
    int radius = std::max(8, m_size / 3);

    // 1. Gradient Background (Royal Blue #4169E1 to Purple #8064E8)
    QLinearGradient grad(rect.topLeft(), rect.bottomRight());
    grad.setColorAt(0.0, QColor("#4169E1"));
    grad.setColorAt(1.0, QColor("#8064E8"));

    painter.setPen(Qt::NoPen);
    painter.setBrush(grad);
    painter.drawRoundedRect(rect, radius, radius);

    // 2. Vector Target Rings (White & Crisp)
    QPointF center = rect.center();
    double baseR = m_size * 0.35;

    // Outer Target Ring
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(255, 255, 255, 220), std::max(1.8, m_size * 0.05)));
    painter.drawEllipse(center, baseR, baseR);

    // Middle Ring
    double midR = baseR * 0.62;
    painter.setPen(QPen(QColor(37, 185, 154, 240), std::max(1.6, m_size * 0.045))); // Teal #25B99A
    painter.drawEllipse(center, midR, midR);

    // Center Bullseye Dot
    double innerR = baseR * 0.28;
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#FFFFFF"));
    painter.drawEllipse(center, innerR, innerR);
}
