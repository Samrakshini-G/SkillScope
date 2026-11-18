#include "EmptyStateWidget.h"
#include "../Theme.h"

EmptyStateWidget::EmptyStateWidget(const QString& icon,
                                   const QString& title,
                                   const QString& description,
                                   const QString& actionText,
                                   QWidget* parent)
    : QWidget(parent),
      m_iconLabel(nullptr),
      m_titleLabel(nullptr),
      m_descLabel(nullptr),
      m_actionBtn(nullptr) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 32, 24, 32);
    layout->setSpacing(12);
    layout->setAlignment(Qt::AlignCenter);

    m_iconLabel = new QLabel(icon, this);
    m_iconLabel->setStyleSheet("font-size: 36px; background: transparent;");
    m_iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_iconLabel);

    m_titleLabel = new QLabel(title, this);
    m_titleLabel->setStyleSheet("font-size: 16px; font-weight: 700; color: #202B45; background: transparent;");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_titleLabel);

    m_descLabel = new QLabel(description, this);
    m_descLabel->setStyleSheet("font-size: 13px; color: #7A849B; background: transparent; line-height: 1.4;");
    m_descLabel->setWordWrap(true);
    m_descLabel->setAlignment(Qt::AlignCenter);
    m_descLabel->setMaximumWidth(460);
    layout->addWidget(m_descLabel);

    if (!actionText.isEmpty()) {
        m_actionBtn = new QPushButton(actionText, this);
        m_actionBtn->setStyleSheet(Theme::primaryButtonStyle());
        m_actionBtn->setCursor(Qt::PointingHandCursor);
        connect(m_actionBtn, &QPushButton::clicked, this, &EmptyStateWidget::actionClicked);
        layout->addSpacing(6);
        layout->addWidget(m_actionBtn, 0, Qt::AlignCenter);
    }
}

void EmptyStateWidget::setActionText(const QString& actionText) {
    if (m_actionBtn) {
        m_actionBtn->setText(actionText);
        m_actionBtn->setVisible(!actionText.isEmpty());
    }
}
