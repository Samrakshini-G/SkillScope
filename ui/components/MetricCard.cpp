#include "MetricCard.h"
#include "../Theme.h"

MetricCard::MetricCard(const QString& icon,
                       const QString& title,
                       const QString& value,
                       const QString& subtitle,
                       const QString& accentColor,
                       QWidget* parent)
    : QFrame(parent),
      m_iconLabel(nullptr),
      m_titleLabel(nullptr),
      m_valueLabel(nullptr),
      m_subLabel(nullptr),
      m_iconContainer(nullptr),
      m_accentColor(accentColor) {
    setupUi(icon, title, value, subtitle, accentColor);
}

void MetricCard::setupUi(const QString& icon,
                         const QString& title,
                         const QString& value,
                         const QString& subtitle,
                         const QString& accentColor) {
    setObjectName("metricCard");
    setFrameShape(QFrame::NoFrame);

    setStyleSheet(
        "QFrame#metricCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "}"
        "QFrame#metricCard:hover {"
        "  border: 1px solid #CBD5E1;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
    );

    Theme::applyCardShadow(this, 12, 3, 14);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(8);

    // Top Row with Icon Container + Title
    QHBoxLayout* topRow = new QHBoxLayout();
    topRow->setSpacing(10);

    m_iconContainer = new QFrame(this);
    m_iconContainer->setObjectName("metricIconBox");
    m_iconContainer->setFixedSize(38, 38);
    m_iconContainer->setStyleSheet(QString(
        "QFrame#metricIconBox {"
        "  background-color: %11A;"
        "  border-radius: 10px;"
        "  border: none;"
        "}"
        "QLabel { border: none; background: transparent; }"
    ).arg(accentColor));

    QVBoxLayout* iconLayout = new QVBoxLayout(m_iconContainer);
    iconLayout->setContentsMargins(0, 0, 0, 0);
    iconLayout->setAlignment(Qt::AlignCenter);

    m_iconLabel = new QLabel(icon, m_iconContainer);
    m_iconLabel->setFont(QFont("Segoe UI Emoji", 15));
    m_iconLabel->setAlignment(Qt::AlignCenter);
    iconLayout->addWidget(m_iconLabel);

    QVBoxLayout* titlesLayout = new QVBoxLayout();
    titlesLayout->setSpacing(0);
    m_titleLabel = new QLabel(title.toUpper());
    m_titleLabel->setStyleSheet("font-size: 11px; font-weight: 700; color: #7A849B; letter-spacing: 0.6px; background: transparent; border: none;");

    titlesLayout->addWidget(m_titleLabel);

    topRow->addWidget(m_iconContainer);
    topRow->addLayout(titlesLayout);
    topRow->addStretch();

    mainLayout->addLayout(topRow);

    // Prominent Value
    m_valueLabel = new QLabel(value);
    m_valueLabel->setStyleSheet(QString(
        "font-size: 26px; font-weight: 800; color: %1; background: transparent; border: none;"
    ).arg(accentColor));
    mainLayout->addWidget(m_valueLabel);

    // Subtitle / Descriptive Text
    m_subLabel = new QLabel(subtitle);
    m_subLabel->setStyleSheet("font-size: 11px; color: #7A849B; font-weight: 500; background: transparent; border: none;");
    m_subLabel->setWordWrap(true);
    mainLayout->addWidget(m_subLabel);

    mainLayout->addStretch();
}

void MetricCard::setValue(const QString& value) {
    if (m_valueLabel) {
        m_valueLabel->setText(value);
    }
}

void MetricCard::setSubtitle(const QString& subtitle) {
    if (m_subLabel) {
        m_subLabel->setText(subtitle);
    }
}

void MetricCard::setAccentColor(const QString& color) {
    m_accentColor = color;
    if (m_valueLabel) {
        m_valueLabel->setStyleSheet(QString(
            "font-size: 26px; font-weight: 800; color: %1; background: transparent; border: none;"
        ).arg(color));
    }
    if (m_iconContainer) {
        m_iconContainer->setStyleSheet(QString(
            "QFrame#metricIconBox {"
            "  background-color: %11A;"
            "  border-radius: 10px;"
            "  border: none;"
            "}"
            "QLabel { border: none; background: transparent; }"
        ).arg(color));
    }
}
