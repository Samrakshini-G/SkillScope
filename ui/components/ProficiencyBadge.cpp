#include "ProficiencyBadge.h"
#include <QPainter>
#include <QPainterPath>

ProficiencyBadge::ProficiencyBadge(int level, bool showMeter, QWidget* parent)
    : QWidget(parent), m_level(level), m_showMeter(showMeter), m_label(nullptr) {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(6, 2, 6, 2);
    layout->setSpacing(6);

    m_label = new QLabel(this);
    m_label->setStyleSheet("font-size: 11px; font-weight: 700; background: transparent;");
    layout->addWidget(m_label);

    if (m_showMeter) {
        layout->addSpacing(34); // Space reserved for custom painted 3-bar meter
    }

    setLevel(level);
}

QString ProficiencyBadge::levelName(int level) {
    switch (level) {
        case 0: return "Not Learned";
        case 1: return "Beginner";
        case 2: return "Intermediate";
        case 3: return "Advanced";
        default: return "Unknown";
    }
}

QString ProficiencyBadge::levelColor(int level) {
    switch (level) {
        case 0: return "#7A849B";
        case 1: return "#4169E1"; // Primary Blue
        case 2: return "#8064E8"; // Secondary Purple
        case 3: return "#20B486"; // Success Green
        default: return "#7A849B";
    }
}

QString ProficiencyBadge::levelBgColor(int level) {
    switch (level) {
        case 0: return "#F1F5F9";
        case 1: return "#EEF2FF";
        case 2: return "#F3F0FD";
        case 3: return "#E8F8F3";
        default: return "#F1F5F9";
    }
}

void ProficiencyBadge::setLevel(int level) {
    m_level = std::max(0, std::min(3, level));
    updateAppearance();
    update();
}

void ProficiencyBadge::updateAppearance() {
    QString fg = levelColor(m_level);
    QString bg = levelBgColor(m_level);

    m_label->setText(QString("L%1 • %2").arg(m_level).arg(levelName(m_level)));
    m_label->setStyleSheet(QString("color: %1; font-size: 11px; font-weight: 700; background: transparent;").arg(fg));

    setStyleSheet(QString(
        "ProficiencyBadge {"
        "  background-color: %1;"
        "  border: 1px solid %220;"
        "  border-radius: 12px;"
        "}"
    ).arg(bg, fg));
}

void ProficiencyBadge::paintEvent(QPaintEvent* /*event*/) {
    if (!m_showMeter) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw 3 mini vertical bars on the right
    int barWidth = 4;
    int spacing = 3;
    int totalWidth = 3 * barWidth + 2 * spacing;
    int startX = width() - totalWidth - 8;
    int centerY = height() / 2;

    QColor activeColor(levelColor(m_level));
    QColor inactiveColor(QColor("#D5DAE5"));

    for (int i = 1; i <= 3; ++i) {
        int barHeight = 6 + i * 2; // Progressive heights: 8, 10, 12
        int x = startX + (i - 1) * (barWidth + spacing);
        int y = centerY - barHeight / 2;

        painter.setPen(Qt::NoPen);
        painter.setBrush(i <= m_level ? activeColor : inactiveColor);
        painter.drawRoundedRect(QRectF(x, y, barWidth, barHeight), 2, 2);
    }
}
