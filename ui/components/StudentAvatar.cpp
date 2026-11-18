#include "StudentAvatar.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QFont>
#include <QStringList>

StudentAvatar::StudentAvatar(const QString& name, int size, QWidget* parent)
    : QWidget(parent), m_size(size) {
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setFixedSize(m_size, m_size);
    setName(name);
}

void StudentAvatar::setName(const QString& name) {
    extractInitials(name);
    update();
}

void StudentAvatar::setAvatarSize(int size) {
    m_size = size;
    setFixedSize(m_size, m_size);
    update();
}

void StudentAvatar::extractInitials(const QString& name) {
    QString trimmed = name.trimmed();
    if (trimmed.isEmpty()) {
        m_initials = "ST";
        return;
    }

    QStringList parts = trimmed.split(" ", Qt::SkipEmptyParts);
    if (parts.size() >= 2) {
        m_initials = QString(parts[0][0].toUpper()) + QString(parts[parts.size() - 1][0].toUpper());
    } else if (!parts.isEmpty() && parts[0].length() >= 2) {
        m_initials = parts[0].left(2).toUpper();
    } else if (!parts.isEmpty()) {
        m_initials = parts[0].left(1).toUpper();
    } else {
        m_initials = "ST";
    }
}

void StudentAvatar::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect(1, 1, width() - 2, height() - 2);

    // Gradient background: Primary Blue (#4169E1) to Secondary Purple (#8064E8)
    QLinearGradient grad(rect.topLeft(), rect.bottomRight());
    grad.setColorAt(0.0, QColor("#4169E1"));
    grad.setColorAt(1.0, QColor("#8064E8"));

    painter.setPen(Qt::NoPen);
    painter.setBrush(grad);
    painter.drawEllipse(rect);

    // Subtle inner border
    painter.setPen(QPen(QColor(255, 255, 255, 60), 1.5));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(rect);

    // Initials Text
    painter.setPen(Qt::white);
    QFont font("Segoe UI", std::max(9, m_size / 3), QFont::Bold);
    painter.setFont(font);
    painter.drawText(rect, Qt::AlignCenter, m_initials);
}
