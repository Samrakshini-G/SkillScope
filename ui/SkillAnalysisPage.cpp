#include "SkillAnalysisPage.h"
#include "../services/AuthService.h"
#include "../services/JobDescriptionParser.h"
#include "../database/CareerRepository.h"
#include "../database/UserRepository.h"
#include "Theme.h"
#include "components/ProficiencyBadge.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>

SkillAnalysisPage::SkillAnalysisPage(QWidget *parent)
    : QWidget(parent),
      m_roleCombo(nullptr),
      m_analyzeRoleBtn(nullptr),
      m_jobDescText(nullptr),
      m_analyzeJdBtn(nullptr),
      m_sampleJdBtn(nullptr),
      m_jdNoticeLabel(nullptr),
      m_targetCareerTitleLabel(nullptr),
      m_readinessPercentLabel(nullptr),
      m_readinessCircular(nullptr),
      m_readinessProgressBar(nullptr),
      m_cardTotalReq(nullptr),
      m_cardMeets(nullptr),
      m_cardInsuff(nullptr),
      m_cardMissing(nullptr),
      m_comparisonTable(nullptr),
      m_missingTable(nullptr),
      m_insufficientTable(nullptr),
      m_masteredTable(nullptr),
      m_statusLabel(nullptr) {
    setupUi();
}

void SkillAnalysisPage::setupUi() {
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
    headerCard->setObjectName("analysisHeaderCard");
    headerCard->setStyleSheet(
        "#analysisHeaderCard {"
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

    QLabel* title = new QLabel("Career Skill Gap Analysis & Industry Readiness");
    title->setStyleSheet("font-size: 20px; font-weight: 800; color: #202B45; background: transparent;");

    QLabel* subtitle = new QLabel(
        "Evaluate technical competency gaps against standardized industry career benchmarks or custom job postings. "
        "The weighted readiness calculation uses exact mathematical modeling: Gap = max(0, RequiredLevel - CurrentLevel)."
    );
    subtitle->setStyleSheet("font-size: 13px; color: #7A849B; background: transparent;");

    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    mainLayout->addWidget(headerCard);

    // ==========================================
    // 2. Input Mode Selector (Split Track A & Track B)
    // ==========================================
    QHBoxLayout* inputRow = new QHBoxLayout();
    inputRow->setSpacing(20);

    // Track A: Benchmark Career Role Selection
    QFrame* roleCard = new QFrame();
    roleCard->setObjectName("analysisRoleCard");
    roleCard->setStyleSheet(
        "#analysisRoleCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "  padding: 18px 20px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; }"
    );
    Theme::applyCardShadow(roleCard, 12, 3, 14);

    QVBoxLayout* roleLayout = new QVBoxLayout(roleCard);
    roleLayout->setSpacing(10);

    QLabel* roleHeader = new QLabel("Track A: Standard Career Benchmark");
    roleHeader->setStyleSheet("font-size: 14px; font-weight: 700; color: #202B45; background: transparent;");
    roleLayout->addWidget(roleHeader);

    QLabel* roleDesc = new QLabel("Select an industry profile to inspect required competency levels.");
    roleDesc->setStyleSheet("font-size: 11px; color: #7A849B; background: transparent;");
    roleLayout->addWidget(roleDesc);

    m_roleCombo = new QComboBox();
    m_roleCombo->setMinimumWidth(240);
    roleLayout->addWidget(m_roleCombo);

    m_analyzeRoleBtn = new QPushButton("🎯 Analyze Target Career");
    m_analyzeRoleBtn->setStyleSheet(Theme::primaryButtonStyle());
    m_analyzeRoleBtn->setCursor(Qt::PointingHandCursor);
    connect(m_analyzeRoleBtn, &QPushButton::clicked, this, &SkillAnalysisPage::onAnalyzeRole);
    roleLayout->addWidget(m_analyzeRoleBtn);
    roleLayout->addStretch();

    inputRow->addWidget(roleCard, 1);

    // Track B: Custom Job Description Parser
    QFrame* jdCard = new QFrame();
    jdCard->setObjectName("analysisJdCard");
    jdCard->setStyleSheet(
        "#analysisJdCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "  padding: 18px 20px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; }"
    );
    Theme::applyCardShadow(jdCard, 12, 3, 14);

    QVBoxLayout* jdLayout = new QVBoxLayout(jdCard);
    jdLayout->setSpacing(10);

    QLabel* jdHeader = new QLabel("Track B: Paste & Parse Job Posting");
    jdHeader->setStyleSheet("font-size: 14px; font-weight: 700; color: #202B45; background: transparent;");
    jdLayout->addWidget(jdHeader);

    m_jobDescText = new QTextEdit();
    m_jobDescText->setPlaceholderText("Paste job posting text here (e.g., 'Looking for a C++ developer with OOP, Data Structures, STL, and Git...').");
    m_jobDescText->setMaximumHeight(76);
    jdLayout->addWidget(m_jobDescText);

    QHBoxLayout* jdBtnRow = new QHBoxLayout();
    jdBtnRow->setSpacing(10);

    m_analyzeJdBtn = new QPushButton("🔍 Extract Skills & Analyze");
    m_analyzeJdBtn->setStyleSheet(Theme::primaryButtonStyle());
    m_analyzeJdBtn->setCursor(Qt::PointingHandCursor);
    connect(m_analyzeJdBtn, &QPushButton::clicked, this, &SkillAnalysisPage::onAnalyzeJobDescription);

    m_sampleJdBtn = new QPushButton("Insert Sample Posting");
    m_sampleJdBtn->setStyleSheet(Theme::secondaryButtonStyle());
    m_sampleJdBtn->setCursor(Qt::PointingHandCursor);
    connect(m_sampleJdBtn, &QPushButton::clicked, this, &SkillAnalysisPage::onLoadSampleJobDescription);

    jdBtnRow->addWidget(m_analyzeJdBtn);
    jdBtnRow->addWidget(m_sampleJdBtn);
    jdBtnRow->addStretch();
    jdLayout->addLayout(jdBtnRow);

    m_jdNoticeLabel = new QLabel("");
    m_jdNoticeLabel->setStyleSheet("font-size: 11px; color: #7A849B; background: transparent;");
    m_jdNoticeLabel->setWordWrap(true);
    jdLayout->addWidget(m_jdNoticeLabel);

    inputRow->addWidget(jdCard, 2);
    mainLayout->addLayout(inputRow);

    // ==========================================
    // 3. Career Readiness Analytics Card (with Circular Progress)
    // ==========================================
    QFrame* readinessCard = new QFrame();
    readinessCard->setObjectName("analysisReadinessCard");
    readinessCard->setStyleSheet(
        "#analysisReadinessCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 16px;"
        "  padding: 22px 26px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; }"
    );
    Theme::applyCardShadow(readinessCard, 14, 3, 16);

    QVBoxLayout* readinessLayout = new QVBoxLayout(readinessCard);
    readinessLayout->setSpacing(16);

    QHBoxLayout* topReadinessRow = new QHBoxLayout();
    topReadinessRow->setSpacing(24);

    m_readinessCircular = new CircularProgress();
    m_readinessCircular->setFixedSize(140, 140);
    topReadinessRow->addWidget(m_readinessCircular);

    QVBoxLayout* targetInfoCol = new QVBoxLayout();
    targetInfoCol->setSpacing(6);
    targetInfoCol->setAlignment(Qt::AlignVCenter);

    QLabel* targetMetaLbl = new QLabel("ANALYSIS TARGET");
    targetMetaLbl->setStyleSheet("font-size: 10px; font-weight: 800; color: #4169E1; letter-spacing: 0.6px; background: transparent;");

    m_targetCareerTitleLabel = new QLabel("Target: None");
    m_targetCareerTitleLabel->setStyleSheet("font-size: 20px; font-weight: 800; color: #202B45; background: transparent;");

    m_readinessPercentLabel = new QLabel("0.0% Overall Readiness");
    m_readinessPercentLabel->setStyleSheet("font-size: 14px; font-weight: 700; color: #20B486; background: transparent;");

    m_readinessProgressBar = new QProgressBar();
    m_readinessProgressBar->setRange(0, 100);
    m_readinessProgressBar->setValue(0);
    m_readinessProgressBar->setFixedHeight(12);

    targetInfoCol->addWidget(targetMetaLbl);
    targetInfoCol->addWidget(m_targetCareerTitleLabel);
    targetInfoCol->addWidget(m_readinessPercentLabel);
    targetInfoCol->addWidget(m_readinessProgressBar);
    topReadinessRow->addLayout(targetInfoCol, 1);

    readinessLayout->addLayout(topReadinessRow);

    // 4 Summary Metrics Cards Row
    QGridLayout* countersGrid = new QGridLayout();
    countersGrid->setSpacing(14);

    m_cardTotalReq = new MetricCard("🎯", "Total Required", "0", "Benchmarked skills", "#202B45");
    m_cardMeets = new MetricCard("✅", "Meets Requirement", "0", "Satisfies career benchmark", "#20B486");
    m_cardInsuff = new MetricCard("⚠️", "Needs Elevation", "0", "Partial proficiency", "#F3A640");
    m_cardMissing = new MetricCard("❌", "Missing Skills", "0", "Level 0 - Not yet learned", "#E56B75");

    countersGrid->addWidget(m_cardTotalReq, 0, 0);
    countersGrid->addWidget(m_cardMeets, 0, 1);
    countersGrid->addWidget(m_cardInsuff, 0, 2);
    countersGrid->addWidget(m_cardMissing, 0, 3);

    readinessLayout->addLayout(countersGrid);
    mainLayout->addWidget(readinessCard);

    // ==========================================
    // 4. Comprehensive Comparison Matrix
    // ==========================================
    QFrame* compCard = new QFrame();
    compCard->setObjectName("analysisCompCard");
    compCard->setStyleSheet(
        "#analysisCompCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "  padding: 20px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; }"
    );
    Theme::applyCardShadow(compCard, 14, 3, 16);

    QVBoxLayout* compLayout = new QVBoxLayout(compCard);
    compLayout->setSpacing(12);

    QLabel* compTitle = new QLabel("📊 Comprehensive Skill Gap Comparison Matrix");
    compTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #202B45; background: transparent;");
    compLayout->addWidget(compTitle);

    m_comparisonTable = new QTableWidget();
    m_comparisonTable->setColumnCount(6);
    m_comparisonTable->setHorizontalHeaderLabels({
        "Skill Name", "Current Level", "Required Level", "Proficiency Comparison", "Gap Size", "Status"
    });
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_comparisonTable->verticalHeader()->setVisible(false);
    m_comparisonTable->setAlternatingRowColors(true);
    m_comparisonTable->setMinimumHeight(240);
    compLayout->addWidget(m_comparisonTable);

    mainLayout->addWidget(compCard);

    // ==========================================
    // 5. Partitioned Detail Category Tabs
    // ==========================================
    QTabWidget* gapTabs = new QTabWidget();

    // Tab 1: Missing Skills
    QWidget* tabMissing = new QWidget();
    QVBoxLayout* missingLayout = new QVBoxLayout(tabMissing);
    m_missingTable = new QTableWidget();
    m_missingTable->setColumnCount(4);
    m_missingTable->setHorizontalHeaderLabels({"Skill", "Current Level", "Required Level", "Importance"});
    m_missingTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_missingTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_missingTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_missingTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_missingTable->verticalHeader()->setVisible(false);
    missingLayout->addWidget(m_missingTable);
    gapTabs->addTab(tabMissing, "❌ Missing Skills (Level 0)");

    // Tab 2: Insufficient Skills
    QWidget* tabInsuff = new QWidget();
    QVBoxLayout* insuffLayout = new QVBoxLayout(tabInsuff);
    m_insufficientTable = new QTableWidget();
    m_insufficientTable->setColumnCount(4);
    m_insufficientTable->setHorizontalHeaderLabels({"Skill", "Current Level", "Required Level", "Importance"});
    m_insufficientTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_insufficientTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_insufficientTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_insufficientTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_insufficientTable->verticalHeader()->setVisible(false);
    insuffLayout->addWidget(m_insufficientTable);
    gapTabs->addTab(tabInsuff, "⚠️ Skills Needing Elevation");

    // Tab 3: Mastered Skills
    QWidget* tabMastered = new QWidget();
    QVBoxLayout* masteredLayout = new QVBoxLayout(tabMastered);
    m_masteredTable = new QTableWidget();
    m_masteredTable->setColumnCount(4);
    m_masteredTable->setHorizontalHeaderLabels({"Skill", "Current Level", "Required Level", "Status"});
    m_masteredTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_masteredTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_masteredTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_masteredTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_masteredTable->verticalHeader()->setVisible(false);
    masteredLayout->addWidget(m_masteredTable);
    gapTabs->addTab(tabMastered, "✅ Meets or Exceeds Requirements");

    mainLayout->addWidget(gapTabs);

    // ==========================================
    // 6. Action Button (Generate Roadmap)
    // ==========================================
    QHBoxLayout* actionRow = new QHBoxLayout();
    QPushButton* generateBtn = new QPushButton("🚀 Generate STL-Prioritized Learning Roadmap");
    generateBtn->setStyleSheet(Theme::primaryButtonStyle());
    generateBtn->setCursor(Qt::PointingHandCursor);
    connect(generateBtn, &QPushButton::clicked, this, &SkillAnalysisPage::onGenerateRoadmapClicked);

    actionRow->addStretch();
    actionRow->addWidget(generateBtn);
    actionRow->addStretch();
    mainLayout->addLayout(actionRow);

    m_statusLabel = new QLabel("");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("font-weight: 700; font-size: 13px; background: transparent;");
    mainLayout->addWidget(m_statusLabel);

    mainLayout->addStretch();

    scrollArea->setWidget(contentWidget);
    outerLayout->addWidget(scrollArea);
}

void SkillAnalysisPage::refreshData() {
    CareerRepository repo;
    auto roles = repo.getAllJobRoles();

    int prevSelectedId = m_roleCombo->currentData().toInt();
    m_roleCombo->blockSignals(true);
    m_roleCombo->clear();
    for (const auto& r : roles) {
        m_roleCombo->addItem(QString::fromStdString(r.getTitle()), r.getId());
    }
    m_roleCombo->blockSignals(false);

    Student* student = AuthService::instance().getCurrentStudent();
    if (student) {
        UserRepository uRepo;
        int targetRoleId = uRepo.getTargetRole(student->getId());
        if (targetRoleId > 0) {
            setTargetRole(targetRoleId);
            return;
        }
    }

    if (prevSelectedId > 0) {
        setTargetRole(prevSelectedId);
    } else if (m_roleCombo->count() > 0) {
        onAnalyzeRole();
    }
}

void SkillAnalysisPage::setTargetRole(int roleId) {
    for (int i = 0; i < m_roleCombo->count(); ++i) {
        if (m_roleCombo->itemData(i).toInt() == roleId) {
            m_roleCombo->setCurrentIndex(i);
            onAnalyzeRole();
            break;
        }
    }
}

void SkillAnalysisPage::onAnalyzeRole() {
    if (m_roleCombo->count() == 0) return;
    int roleId = m_roleCombo->currentData().toInt();

    Student* student = AuthService::instance().getCurrentStudent();
    int studentId = student ? student->getId() : 1;

    SkillAnalysisService service;
    m_currentResult = service.analyzeGaps(studentId, roleId, true);
    displayAnalysis(m_currentResult, false);
}

void SkillAnalysisPage::onAnalyzeJobDescription() {
    QString text = m_jobDescText->toPlainText().trimmed();
    if (text.isEmpty()) {
        m_statusLabel->setText("Error: Please paste a job description into the text area to analyze.");
        m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700;");
        return;
    }

    JobDescriptionParser parser;
    auto detectedSkills = parser.parseJobDescription(text.toStdString());
    auto unrecognized = parser.extractUnrecognizedKeywords(text.toStdString());

    if (detectedSkills.empty()) {
        m_statusLabel->setText("No known technical skills were identified in the pasted text.");
        m_statusLabel->setStyleSheet("color: #F3A640; font-weight: 700;");
        return;
    }

    QString notice = QString("Identified %1 catalog skills in text.").arg(detectedSkills.size());
    if (!unrecognized.empty()) {
        notice += " Note: Unrecognized technologies detected: ";
        for (size_t i = 0; i < unrecognized.size(); ++i) {
            notice += QString::fromStdString(unrecognized[i]);
            if (i < unrecognized.size() - 1) notice += ", ";
        }
        notice += " (not in catalog).";
    }
    m_jdNoticeLabel->setText(notice);

    auto customRequirements = parser.convertToRequirements(detectedSkills);

    Student* student = AuthService::instance().getCurrentStudent();
    int studentId = student ? student->getId() : 1;

    SkillAnalysisService service;
    m_currentResult = service.analyzeParsedSkills(studentId, customRequirements, "Custom Job Description", false);
    displayAnalysis(m_currentResult, true);

    m_statusLabel->setText("✓ Job description parsed and analyzed successfully!");
    m_statusLabel->setStyleSheet("color: #20B486; font-weight: 700;");
}

void SkillAnalysisPage::onLoadSampleJobDescription() {
    QString sample =
        "Senior Software Engineer (C++ / Backend)\n\n"
        "We are seeking an advanced C++ developer with extensive experience in Object-Oriented Programming (OOP) "
        "and modern C++ standards. Required qualifications:\n"
        "- Mastery in Data Structures, Algorithms, and the C++ STL\n"
        "- Solid practical experience with Git version control and Linux environments\n"
        "- Relational database knowledge in SQL\n"
        "- Excellent problem-solving skills and teamwork in an agile engineering team.";

    m_jobDescText->setPlainText(sample);
    onAnalyzeJobDescription();
}

static void fillSubTable(QTableWidget* table, const std::vector<SkillGap>& list, bool isMastered = false) {
    table->setRowCount(static_cast<int>(list.size()));
    for (size_t i = 0; i < list.size(); ++i) {
        const auto& gap = list[i];

        QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(gap.getSkillName()));
        nameItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
        table->setItem(static_cast<int>(i), 0, nameItem);

        QTableWidgetItem* currItem = new QTableWidgetItem(QString("Level %1").arg(gap.getCurrentLevel()));
        currItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(static_cast<int>(i), 1, currItem);

        QTableWidgetItem* reqItem = new QTableWidgetItem(QString("Level %1").arg(gap.getRequiredLevel()));
        reqItem->setTextAlignment(Qt::AlignCenter);
        table->setItem(static_cast<int>(i), 2, reqItem);

        QString statusText = isMastered ? "Meets requirement" : QString::fromStdString(gap.getImportance());
        QTableWidgetItem* statItem = new QTableWidgetItem(statusText);
        statItem->setTextAlignment(Qt::AlignCenter);
        statItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        statItem->setForeground(QBrush(isMastered ? QColor("#20B486") : QColor("#F3A640")));
        table->setItem(static_cast<int>(i), 3, statItem);
    }
}

void SkillAnalysisPage::displayAnalysis(const AnalysisResult& result, bool isFromJobDescription) {
    QString titleText = QString("Target: %1").arg(QString::fromStdString(result.jobRoleTitle));
    if (isFromJobDescription) {
        titleText += " (Pasted Job Description)";
    }
    m_targetCareerTitleLabel->setText(titleText);

    double pct = result.matchPercentage;
    m_readinessPercentLabel->setText(QString("%1% Overall Readiness").arg(QString::number(pct, 'f', 1)));
    m_readinessCircular->setValue(pct);
    m_readinessProgressBar->setValue(static_cast<int>(pct));

    if (pct >= 75.0) {
        m_readinessProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #20B486; }");
    } else if (pct >= 40.0) {
        m_readinessProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #F3A640; }");
    } else {
        m_readinessProgressBar->setStyleSheet("QProgressBar::chunk { background-color: #E56B75; }");
    }

    // Update Counter Badges
    m_cardTotalReq->setValue(QString::number(result.totalRequiredSkills));
    m_cardMeets->setValue(QString::number(result.meetingCount));
    m_cardInsuff->setValue(QString::number(result.insufficientCount));
    m_cardMissing->setValue(QString::number(result.missingCount));

    // Populate Comparison Matrix
    m_comparisonTable->setRowCount(static_cast<int>(result.allGaps.size()));
    for (size_t i = 0; i < result.allGaps.size(); ++i) {
        const auto& gap = result.allGaps[i];

        // Col 0: Skill
        QTableWidgetItem* sItem = new QTableWidgetItem(QString::fromStdString(gap.getSkillName()));
        sItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
        m_comparisonTable->setItem(static_cast<int>(i), 0, sItem);

        // Col 1: Current Level
        QTableWidgetItem* cItem = new QTableWidgetItem(QString("Level %1").arg(gap.getCurrentLevel()));
        cItem->setTextAlignment(Qt::AlignCenter);
        m_comparisonTable->setItem(static_cast<int>(i), 1, cItem);

        // Col 2: Required Level
        QTableWidgetItem* rItem = new QTableWidgetItem(QString("Level %1").arg(gap.getRequiredLevel()));
        rItem->setTextAlignment(Qt::AlignCenter);
        m_comparisonTable->setItem(static_cast<int>(i), 2, rItem);

        // Col 3: Visual Comparison Progress Bar
        QProgressBar* pBar = new QProgressBar();
        pBar->setRange(0, 3);
        pBar->setValue(gap.getCurrentLevel());
        pBar->setTextVisible(false);
        pBar->setFixedHeight(8);
        if (gap.getCurrentLevel() >= gap.getRequiredLevel()) {
            pBar->setStyleSheet("QProgressBar { background-color: #E8EAF2; border-radius: 4px; } QProgressBar::chunk { background-color: #20B486; border-radius: 4px; }");
        } else if (gap.getCurrentLevel() > 0) {
            pBar->setStyleSheet("QProgressBar { background-color: #E8EAF2; border-radius: 4px; } QProgressBar::chunk { background-color: #F3A640; border-radius: 4px; }");
        } else {
            pBar->setStyleSheet("QProgressBar { background-color: #E8EAF2; border-radius: 4px; } QProgressBar::chunk { background-color: #E56B75; border-radius: 4px; }");
        }
        m_comparisonTable->setCellWidget(static_cast<int>(i), 3, pBar);

        // Col 4: Gap
        int gapVal = gap.calculateGap();
        QTableWidgetItem* gItem = new QTableWidgetItem(QString::number(gapVal));
        gItem->setTextAlignment(Qt::AlignCenter);
        gItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
        if (gapVal == 0) gItem->setForeground(QBrush(QColor("#20B486")));
        else if (gapVal == 1) gItem->setForeground(QBrush(QColor("#F3A640")));
        else gItem->setForeground(QBrush(QColor("#E56B75")));
        m_comparisonTable->setItem(static_cast<int>(i), 4, gItem);

        // Col 5: Status Pill Badge
        std::string statusStr = gap.getStatusText();
        QLabel* badge = new QLabel(QString::fromStdString(statusStr));
        badge->setAlignment(Qt::AlignCenter);
        if (statusStr == "Meets requirement") {
            badge->setStyleSheet("background-color: #E8F8F3; color: #20B486; font-weight: 700; font-size: 11px; padding: 4px 10px; border-radius: 10px;");
        } else if (statusStr == "Skill gap") {
            badge->setStyleSheet("background-color: #FEF6EC; color: #F3A640; font-weight: 700; font-size: 11px; padding: 4px 10px; border-radius: 10px;");
        } else {
            badge->setStyleSheet("background-color: #FDF0F1; color: #E56B75; font-weight: 700; font-size: 11px; padding: 4px 10px; border-radius: 10px;");
        }
        m_comparisonTable->setCellWidget(static_cast<int>(i), 5, badge);
    }

    fillSubTable(m_missingTable, result.missingSkills);
    fillSubTable(m_insufficientTable, result.insufficientSkills);
    fillSubTable(m_masteredTable, result.masteredSkills, true);
}

void SkillAnalysisPage::onGenerateRoadmapClicked() {
    if (m_currentResult.allGaps.empty()) {
        m_statusLabel->setText("No skill gaps available to generate a roadmap. Please analyze a career role first.");
        m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700;");
        return;
    }
    emit generateRoadmapRequested(m_currentResult.allGaps);
}
