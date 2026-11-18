#include "CareerSelectionPage.h"
#include "../services/AuthService.h"
#include "../database/CareerRepository.h"
#include "../database/UserRepository.h"
#include "Theme.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>

CareerSelectionPage::CareerSelectionPage(QWidget *parent)
    : QWidget(parent),
      m_searchEdit(nullptr),
      m_rolesGrid(nullptr),
      m_statusLabel(nullptr),
      m_currentTargetRoleId(0) {
    setupUi();
}

void CareerSelectionPage::setupUi() {
    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QWidget* contentWidget = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout(contentWidget);
    mainLayout->setContentsMargins(32, 24, 32, 28);
    mainLayout->setSpacing(22);

    // ==========================================
    // 1. Header Card
    // ==========================================
    QFrame* headerCard = new QFrame();
    headerCard->setObjectName("careerHeaderCard");
    headerCard->setStyleSheet(
        "#careerHeaderCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "  padding: 20px 24px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; }"
    );
    Theme::applyCardShadow(headerCard, 14, 3, 16);

    QVBoxLayout* headerLayout = new QVBoxLayout(headerCard);
    headerLayout->setSpacing(4);

    QLabel* title = new QLabel("Career Role Tracks & Competency Benchmarks");
    title->setStyleSheet("font-size: 20px; font-weight: 800; color: #202B45; background: transparent;");

    QLabel* subtitle = new QLabel(
        "Explore standardized industry career tracks, inspect required skill levels, and select your active target. "
        "Selecting a role saves it as your primary academic target for gap calculations."
    );
    subtitle->setStyleSheet("font-size: 13px; color: #7A849B; background: transparent;");

    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    mainLayout->addWidget(headerCard);

    // ==========================================
    // 2. Search and Status Toolbar
    // ==========================================
    QFrame* searchCard = new QFrame();
    searchCard->setObjectName("careerSearchCard");
    searchCard->setStyleSheet(
        "#careerSearchCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 12px;"
        "  padding: 14px 20px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; }"
    );
    Theme::applyCardShadow(searchCard, 10, 2, 12);

    QHBoxLayout* searchLayout = new QHBoxLayout(searchCard);
    searchLayout->setSpacing(14);

    QLabel* searchLabel = new QLabel("🔍 Search Tracks:");
    searchLabel->setStyleSheet("font-weight: 700; font-size: 12px; color: #202B45; background: transparent;");

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Filter career roles by title, technology, or keywords...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &CareerSelectionPage::onSearchChanged);

    m_statusLabel = new QLabel("");
    m_statusLabel->setStyleSheet("font-weight: 700; font-size: 12px; background: transparent;");

    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(m_searchEdit, 2);
    searchLayout->addWidget(m_statusLabel, 1);

    mainLayout->addWidget(searchCard);

    // ==========================================
    // 3. Roles Grid
    // ==========================================
    m_rolesGrid = new QGridLayout();
    m_rolesGrid->setSpacing(20);
    mainLayout->addLayout(m_rolesGrid);

    mainLayout->addStretch();

    scrollArea->setWidget(contentWidget);
    outerLayout->addWidget(scrollArea);
}

QWidget* CareerSelectionPage::createRoleCard(const JobRole& role, bool isCurrentTarget) {
    QFrame* card = new QFrame();
    card->setObjectName("roleCard");
    QString borderStyle = isCurrentTarget
        ? "border: 2px solid #4169E1; background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FFFFFF, stop:1 #F9FAFF);"
        : "border: 1px solid #E8EAF2; background-color: #FFFFFF;";

    card->setStyleSheet(QString(
        "#roleCard {"
        "  %1"
        "  border-radius: 14px;"
        "  padding: 20px;"
        "}"
        "#roleCard:hover {"
        "  border-color: #4169E1;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; }"
    ).arg(borderStyle));
    Theme::applyCardShadow(card, 14, 3, 16);

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setSpacing(12);

    // Top Title Row
    QHBoxLayout* titleRow = new QHBoxLayout();
    titleRow->setSpacing(10);

    QLabel* titleLabel = new QLabel(QString::fromStdString(role.getTitle()));
    titleLabel->setStyleSheet("font-size: 17px; font-weight: 800; color: #202B45; background: transparent;");
    titleRow->addWidget(titleLabel);
    titleRow->addStretch();

    if (isCurrentTarget) {
        QLabel* badge = new QLabel("★ My Target Career");
        badge->setStyleSheet(
            "background-color: #4169E1; color: #FFFFFF; font-weight: 800; font-size: 11px; padding: 5px 12px; border-radius: 12px; border: none;"
        );
        titleRow->addWidget(badge);
    }
    layout->addLayout(titleRow);

    // Description
    QLabel* descLabel = new QLabel(QString::fromStdString(role.getDescription()));
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("font-size: 12px; color: #7A849B; min-height: 40px; background: transparent;");
    layout->addWidget(descLabel);

    // Divider
    QFrame* div = new QFrame();
    div->setFrameShape(QFrame::HLine);
    div->setStyleSheet("background-color: #F0F2F8; max-height: 1px;");
    layout->addWidget(div);

    // Required Skills Section
    CareerRepository repo;
    auto reqs = repo.getRequirementsForRole(role.getId());

    QLabel* reqsHeader = new QLabel(QString("Required Competencies (%1 skills):").arg(reqs.size()));
    reqsHeader->setStyleSheet("font-size: 12px; font-weight: 700; color: #202B45; background: transparent;");
    layout->addWidget(reqsHeader);

    // Tags flow layout represented as formatted text
    QString reqText = "";
    for (size_t i = 0; i < reqs.size(); ++i) {
        QString importance = QString::fromStdString(reqs[i].getImportance());
        QString impColor = (importance == "Critical") ? "#E56B75" : (importance == "High" ? "#8064E8" : "#4169E1");
        reqText += QString("<span style='color: #202B45; font-weight: 700;'>%1</span> <span style='color: %2; font-size: 10px;'>[L%3 • %4]</span>")
            .arg(QString::fromStdString(reqs[i].getSkillName()))
            .arg(impColor)
            .arg(reqs[i].getRequiredLevel())
            .arg(importance);
        if (i < reqs.size() - 1) reqText += "  •  ";
    }
    QLabel* reqsBody = new QLabel(reqText);
    reqsBody->setWordWrap(true);
    reqsBody->setStyleSheet("font-size: 11px; line-height: 1.6; background: transparent;");
    layout->addWidget(reqsBody);

    layout->addStretch();

    // Action Buttons
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->setSpacing(10);

    int roleId = role.getId();

    QPushButton* setTargetBtn = new QPushButton(isCurrentTarget ? "✓ Active Target" : "⭐ Set as Target");
    setTargetBtn->setEnabled(!isCurrentTarget);
    setTargetBtn->setStyleSheet(
        isCurrentTarget
        ? "QPushButton { background-color: #F1F5F9; color: #7A849B; font-weight: 700; padding: 9px 16px; border-radius: 8px; border: none; font-size: 12px; }"
        : Theme::secondaryButtonStyle()
    );
    setTargetBtn->setCursor(isCurrentTarget ? Qt::ArrowCursor : Qt::PointingHandCursor);
    connect(setTargetBtn, &QPushButton::clicked, this, [this, roleId]() {
        onSetTargetRole(roleId);
    });

    QPushButton* analyzeBtn = new QPushButton("🎯 Analyze Skill Gap");
    analyzeBtn->setStyleSheet(Theme::primaryButtonStyle());
    analyzeBtn->setCursor(Qt::PointingHandCursor);
    connect(analyzeBtn, &QPushButton::clicked, this, [this, roleId]() {
        emit roleSelectedForAnalysis(roleId);
    });

    btnRow->addWidget(setTargetBtn);
    btnRow->addWidget(analyzeBtn);
    layout->addLayout(btnRow);

    return card;
}

void CareerSelectionPage::renderRoleCards(const std::vector<JobRole>& roles) {
    QLayoutItem* item;
    while ((item = m_rolesGrid->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    int row = 0;
    int col = 0;
    for (const auto& role : roles) {
        bool isTarget = (role.getId() == m_currentTargetRoleId);
        QWidget* card = createRoleCard(role, isTarget);
        m_rolesGrid->addWidget(card, row, col);
        col++;
        if (col >= 2) {
            col = 0;
            row++;
        }
    }
}

void CareerSelectionPage::refreshData() {
    Student* student = AuthService::instance().getCurrentStudent();
    if (student) {
        UserRepository userRepo;
        m_currentTargetRoleId = userRepo.getTargetRole(student->getId());
        student->setTargetRoleId(m_currentTargetRoleId);
    }

    CareerRepository repo;
    m_cachedRoles = repo.getAllJobRoles();

    onSearchChanged(m_searchEdit->text());
}

void CareerSelectionPage::onSearchChanged(const QString& query) {
    QString q = query.trimmed().toLower();
    if (q.isEmpty()) {
        renderRoleCards(m_cachedRoles);
        return;
    }

    std::vector<JobRole> filtered;
    for (const auto& r : m_cachedRoles) {
        QString titleStr = QString::fromStdString(r.getTitle()).toLower();
        QString descStr = QString::fromStdString(r.getDescription()).toLower();
        if (titleStr.contains(q) || descStr.contains(q)) {
            filtered.push_back(r);
        }
    }
    renderRoleCards(filtered);
}

void CareerSelectionPage::onSetTargetRole(int roleId) {
    Student* student = AuthService::instance().getCurrentStudent();
    if (!student) return;

    UserRepository repo;
    if (repo.setTargetRole(student->getId(), roleId)) {
        student->setTargetRoleId(roleId);
        m_currentTargetRoleId = roleId;
        m_statusLabel->setText("✓ Target career saved successfully!");
        m_statusLabel->setStyleSheet("color: #20B486; font-weight: 700;");
        refreshData();
    } else {
        m_statusLabel->setText("Failed to save target career to database.");
        m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700;");
    }
}
