#include "CircularProgress.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <cmath>

CircularProgress::CircularProgress(QWidget* parent)
    : QWidget(parent),
      m_value(0.0),
      m_thickness(12),
      m_subText("Readiness"),
      m_useCustomColor(false),
      m_customColor(QColor("#4169E1")) {
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

void CircularProgress::setValue(double percentage) {
    m_value = std::max(0.0, std::min(100.0, percentage));
    update();
}

void CircularProgress::setSubText(const QString& text) {
    m_subText = text;
    update();
}

void CircularProgress::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int side = std::min(width(), height());
    int padding = m_thickness + 6;
    QRectF rect(
        (width() - side) / 2.0 + padding / 2.0,
        (height() - side) / 2.0 + padding / 2.0,
        side - padding,
        side - padding
    );

    // Determine color
    QColor progressColor;
    if (m_useCustomColor) {
        progressColor = m_customColor;
    } else {
        if (m_value >= 75.0) {
            progressColor = QColor("#20B486"); // Success Green
        } else if (m_value >= 40.0) {
            progressColor = QColor("#F3A640"); // Warning Orange
        } else {
            progressColor = QColor("#E56B75"); // Error Red
        }
    }

    // 1. Draw Background Track Circle
    QPen trackPen(QColor("#E8EAF2"), m_thickness, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(trackPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawArc(rect, 0, 360 * 16);

    // 2. Draw Progress Arc
    if (m_value > 0.0) {
        QPen progressPen(progressColor, m_thickness, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(progressPen);
        // Qt draws angles in 1/16ths of a degree. Positive angle is counterclockwise.
        // Start from top (90 degrees = 90 * 16)
        int startAngle = 90 * 16;
        int spanAngle = static_cast<int>(-m_value * 3.6 * 16);
        painter.drawArc(rect, startAngle, spanAngle);
    }

    // 3. Draw Center Text
    painter.setPen(QColor("#202B45"));
    QFont valFont("Segoe UI", std::max(12, side / 7), QFont::Bold);
    painter.setFont(valFont);

    QString textVal = QString::number(m_value, 'f', 1) + "%";
    QRectF textRect = rect;
    textRect.setBottom(rect.center().y() + 4);
    painter.drawText(textRect, Qt::AlignBottom | Qt::AlignHCenter, textVal);

    if (!m_subText.isEmpty()) {
        painter.setPen(QColor("#7A849B"));
        QFont subFont("Segoe UI", std::max(9, side / 14), QFont::DemiBold);
        painter.setFont(subFont);
        QRectF subRect = rect;
        subRect.setTop(rect.center().y() + 6);
        painter.drawText(subRect, Qt::AlignTop | Qt::AlignHCenter, m_subText);
    }
}
