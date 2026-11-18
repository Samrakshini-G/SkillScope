#include "RoadmapPage.h"
#include "../database/CareerRepository.h"
#include "../database/LearningRepository.h"
#include "../database/SkillRepository.h"
#include "../database/UserRepository.h"
#include "../services/AuthService.h"
#include "../services/ProgressService.h"
#include "../services/RecommendationService.h"
#include "../services/RoadmapService.h"
#include "../services/SkillAnalysisService.h"
#include "Theme.h"

#include <QDesktopServices>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QUrl>
#include <QVBoxLayout>
#include <unordered_set>

RoadmapPage::RoadmapPage(QWidget *parent)
    : QWidget(parent), m_roleTitleLabel(nullptr), m_readinessLabel(nullptr),
      m_cardTotal(nullptr), m_cardNotStarted(nullptr),
      m_cardInProgress(nullptr), m_cardCompleted(nullptr),
      m_cardDuration(nullptr), m_milestonesLayout(nullptr),
      m_milestonesContainer(nullptr), m_resourceSkillFilter(nullptr),
      m_resourceTypeFilter(nullptr), m_resourcesLayout(nullptr),
      m_resourcesContainer(nullptr), m_priorityQueueExplanation(nullptr),
      m_statusLabel(nullptr), m_generateBtn(nullptr), m_refreshBtn(nullptr),
      m_saveBtn(nullptr), m_clearBtn(nullptr) {
  setupUi();
}

void RoadmapPage::setupUi() {
  QVBoxLayout *outerLayout = new QVBoxLayout(this);
  outerLayout->setContentsMargins(0, 0, 0, 0);

  QScrollArea *scrollArea = new QScrollArea();
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);

  QWidget *contentWidget = new QWidget();
  QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
  mainLayout->setContentsMargins(32, 24, 32, 28);
  mainLayout->setSpacing(22);

  // ==========================================
  // 1. Header Card with Target & Readiness
  // ==========================================
  QFrame *headerCard = new QFrame();
  headerCard->setStyleSheet("QFrame {"
                            "  background-color: #FFFFFF;"
                            "  border: 1px solid #E8EAF2;"
                            "  border-radius: 14px;"
                            "  padding: 20px 24px;"
                            "}");
  Theme::applyCardShadow(headerCard, 14, 3, 16);

  QVBoxLayout *headerLayout = new QVBoxLayout(headerCard);
  headerLayout->setSpacing(6);

  QLabel *title =
      new QLabel("Personalized Learning Roadmap & Milestone Journey");
  title->setStyleSheet("font-size: 20px; font-weight: 800; color: #202B45; "
                       "background: transparent;");

  QLabel *subtitle = new QLabel(
      "Sequenced using an STL binary max-heap (std::priority_queue) "
      "parameterized by normalized Gap, Importance, and Urgency weights.");
  subtitle->setStyleSheet(
      "font-size: 13px; color: #7A849B; background: transparent;");

  QHBoxLayout *metaRow = new QHBoxLayout();
  metaRow->setSpacing(16);

  m_roleTitleLabel = new QLabel("Target Track: C++ Software Developer");
  m_roleTitleLabel->setStyleSheet("font-size: 14px; font-weight: 700; color: "
                                  "#4169E1; background: transparent;");

  m_readinessLabel = new QLabel("Career Readiness: 0.0%");
  m_readinessLabel->setStyleSheet("font-size: 14px; font-weight: 700; color: "
                                  "#20B486; background: transparent;");

  metaRow->addWidget(m_roleTitleLabel);
  metaRow->addWidget(m_readinessLabel);
  metaRow->addStretch();

  headerLayout->addWidget(title);
  headerLayout->addWidget(subtitle);
  headerLayout->addLayout(metaRow);
  mainLayout->addWidget(headerCard);

  // ==========================================
  // 2. Summary Metric Cards Row
  // ==========================================
  QGridLayout *metricsGrid = new QGridLayout();
  metricsGrid->setSpacing(16);

  m_cardTotal = new MetricCard("🗺️", "Total Milestones", "0", "Scheduled tasks",
                               "#4169E1");
  m_cardNotStarted =
      new MetricCard("⏳", "Not Started", "0", "Pending initiation", "#7A849B");
  m_cardInProgress =
      new MetricCard("⚙️", "In Progress", "0", "Currently active", "#F3A640");
  m_cardCompleted =
      new MetricCard("✅", "Completed", "0", "Milestones achieved", "#20B486");
  m_cardDuration = new MetricCard("📅", "Est. Duration", "0 Wks",
                                  "Paced schedule", "#8064E8");

  metricsGrid->addWidget(m_cardTotal, 0, 0);
  metricsGrid->addWidget(m_cardNotStarted, 0, 1);
  metricsGrid->addWidget(m_cardInProgress, 0, 2);
  metricsGrid->addWidget(m_cardCompleted, 0, 3);
  metricsGrid->addWidget(m_cardDuration, 0, 4);

  mainLayout->addLayout(metricsGrid);

  // ==========================================
  // 3. Priority Queue Specification Card
  // ==========================================
  QFrame *infoBanner = new QFrame();
  infoBanner->setStyleSheet("QFrame {"
                            "  background: qlineargradient(x1:0, y1:0, x2:1, "
                            "y2:0, stop:0 #EEF2FF, stop:1 #F5F6FC);"
                            "  border: 1px solid #D7E1FD;"
                            "  border-radius: 12px;"
                            "  padding: 16px 20px;"
                            "}");
  QVBoxLayout *infoLayout = new QVBoxLayout(infoBanner);
  infoLayout->setSpacing(4);

  QLabel *infoTitle =
      new QLabel("⚙️ STL Priority Queue Algorithmic Specification");
  infoTitle->setStyleSheet("font-size: 13px; font-weight: 800; color: #4169E1; "
                           "background: transparent;");

  m_priorityQueueExplanation = new QLabel(
      "Priority Formula: Score = (0.5 x Gap Score) + (0.3 x Importance Score) "
      "+ (0.2 x Urgency Score)\n"
      "Normalized to 0.0-1.0: Gap Score = Gap/3.0, Importance Score = "
      "Weight/3.0, Urgency Score = 0.5 default.\n"
      "Maintained deterministically using std::priority_queue<SkillGap, "
      "std::vector<SkillGap>, SkillGapPriorityComparator>.");
  m_priorityQueueExplanation->setStyleSheet(
      "font-size: 12px; color: #202B45; line-height: 1.4; background: "
      "transparent;");

  infoLayout->addWidget(infoTitle);
  infoLayout->addWidget(m_priorityQueueExplanation);
  mainLayout->addWidget(infoBanner);

  // ==========================================
  // 4. Action Controls Toolbar
  // ==========================================
  QHBoxLayout *actionToolbar = new QHBoxLayout();
  actionToolbar->setSpacing(12);

  m_generateBtn = new QPushButton("⚡  Generate Roadmap");
  m_generateBtn->setStyleSheet(Theme::primaryButtonStyle());
  m_generateBtn->setCursor(Qt::PointingHandCursor);
  connect(m_generateBtn, &QPushButton::clicked, this,
          &RoadmapPage::onGenerateRoadmap);

  m_refreshBtn = new QPushButton("🔄  Refresh");
  m_refreshBtn->setStyleSheet(Theme::secondaryButtonStyle());
  m_refreshBtn->setCursor(Qt::PointingHandCursor);
  connect(m_refreshBtn, &QPushButton::clicked, this,
          &RoadmapPage::onRefreshRoadmap);

  m_saveBtn = new QPushButton("💾  Save to Active Plan");
  m_saveBtn->setStyleSheet(Theme::successButtonStyle());
  m_saveBtn->setCursor(Qt::PointingHandCursor);
  connect(m_saveBtn, &QPushButton::clicked, this, &RoadmapPage::onSaveRoadmap);

  m_clearBtn = new QPushButton("🗑️  Clear Tasks");
  m_clearBtn->setStyleSheet(Theme::dangerButtonStyle());
  m_clearBtn->setCursor(Qt::PointingHandCursor);
  connect(m_clearBtn, &QPushButton::clicked, this,
          &RoadmapPage::onClearRoadmap);

  m_statusLabel = new QLabel("");
  m_statusLabel->setStyleSheet(
      "font-weight: 700; font-size: 12px; background: transparent;");

  actionToolbar->addWidget(m_generateBtn);
  actionToolbar->addWidget(m_refreshBtn);
  actionToolbar->addWidget(m_saveBtn);
  actionToolbar->addWidget(m_clearBtn);
  actionToolbar->addWidget(m_statusLabel);
  actionToolbar->addStretch();
  mainLayout->addLayout(actionToolbar);

  // ==========================================
  // 5. Learning Journey Milestones (Connected Cards)
  // ==========================================
  QLabel *journeyTitle =
      new QLabel("🚀 Connected Learning Milestones (Priority Sequenced)");
  journeyTitle->setStyleSheet(
      "font-size: 17px; font-weight: 800; color: #202B45; padding-top: 6px;");
  mainLayout->addWidget(journeyTitle);

  m_milestonesContainer = new QWidget();
  m_milestonesLayout = new QVBoxLayout(m_milestonesContainer);
  m_milestonesLayout->setContentsMargins(0, 0, 0, 0);
  m_milestonesLayout->setSpacing(14);
  mainLayout->addWidget(m_milestonesContainer);

  // ==========================================
  // 6. Curated Learning Resources Section
  // ==========================================
  QFrame *resSectionCard = new QFrame();
  resSectionCard->setStyleSheet("QFrame {"
                                "  background-color: #FFFFFF;"
                                "  border: 1px solid #E8EAF2;"
                                "  border-radius: 14px;"
                                "  padding: 20px;"
                                "}");
  Theme::applyCardShadow(resSectionCard, 14, 3, 16);

  QVBoxLayout *resSectionLayout = new QVBoxLayout(resSectionCard);
  resSectionLayout->setSpacing(16);

  QHBoxLayout *resHeader = new QHBoxLayout();
  QLabel *resTitle =
      new QLabel("📚 Curated Learning Resources (Video, Article, Practice)");
  resTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #202B45; "
                          "background: transparent;");
  resHeader->addWidget(resTitle);
  resHeader->addStretch();

  QLabel *filterSkillLbl = new QLabel("Skill:");
  filterSkillLbl->setStyleSheet("font-weight: 700; font-size: 12px; color: "
                                "#202B45; background: transparent;");
  m_resourceSkillFilter = new QComboBox();
  m_resourceSkillFilter->addItem("All Skills", 0);
  m_resourceSkillFilter->setMinimumWidth(180);
  connect(m_resourceSkillFilter,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &RoadmapPage::onFilterResources);

  QLabel *filterTypeLbl = new QLabel("Type:");
  filterTypeLbl->setStyleSheet("font-weight: 700; font-size: 12px; color: "
                               "#202B45; background: transparent;");
  m_resourceTypeFilter = new QComboBox();
  m_resourceTypeFilter->addItems({"All Types", "Video", "Article", "Practice"});
  m_resourceTypeFilter->setMinimumWidth(130);
  connect(m_resourceTypeFilter,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &RoadmapPage::onFilterResources);

  resHeader->addWidget(filterSkillLbl);
  resHeader->addWidget(m_resourceSkillFilter);
  resHeader->addSpacing(8);
  resHeader->addWidget(filterTypeLbl);
  resHeader->addWidget(m_resourceTypeFilter);
  resSectionLayout->addLayout(resHeader);

  m_resourcesContainer = new QWidget();
  m_resourcesLayout = new QVBoxLayout(m_resourcesContainer);
  m_resourcesLayout->setContentsMargins(0, 0, 0, 0);
  m_resourcesLayout->setSpacing(10);
  resSectionLayout->addWidget(m_resourcesContainer);

  mainLayout->addWidget(resSectionCard);

  mainLayout->addStretch();

  scrollArea->setWidget(contentWidget);
  outerLayout->addWidget(scrollArea);
}

void RoadmapPage::renderMilestoneCards() {
  // Clear old milestone widgets
  QLayoutItem *item;
  while ((item = m_milestonesLayout->takeAt(0)) != nullptr) {
    if (item->widget())
      delete item->widget();
    delete item;
  }

  if (m_currentTasks.empty()) {
    QFrame *emptyCard = new QFrame();
    emptyCard->setStyleSheet("background-color: #FFFFFF; border: 1px dashed "
                             "#E8EAF2; border-radius: 12px; padding: 32px;");
    QVBoxLayout *el = new QVBoxLayout(emptyCard);
    el->setAlignment(Qt::AlignCenter);

    QLabel *eIcon = new QLabel("🗺️");
    eIcon->setStyleSheet("font-size: 32px; background: transparent;");
    eIcon->setAlignment(Qt::AlignCenter);

    QLabel *eText = new QLabel("No learning roadmap generated yet.");
    eText->setStyleSheet("font-size: 14px; font-weight: 700; color: #202B45; "
                         "background: transparent;");
    eText->setAlignment(Qt::AlignCenter);

    QLabel *eSub = new QLabel("Click 'Generate Roadmap' above to sequence "
                              "tasks based on your current skill gaps.");
    eSub->setStyleSheet(
        "font-size: 12px; color: #7A849B; background: transparent;");
    eSub->setAlignment(Qt::AlignCenter);

    el->addWidget(eIcon);
    el->addWidget(eText);
    el->addWidget(eSub);
    m_milestonesLayout->addWidget(emptyCard);
    return;
  }

  for (size_t i = 0; i < m_currentTasks.size(); ++i) {
    const auto &task = m_currentTasks[i];

    // Find matching gap for metadata
    SkillGap matchGap;
    for (const auto &g : m_currentGaps) {
      if (g.getSkillId() == task.getSkillId()) {
        matchGap = g;
        break;
      }
    }

    // Determine phase based on index & priority
    QString phaseStr;
    QString phaseColor;
    if (i < 2) {
      phaseStr = "Phase 1: Foundation (High Priority)";
      phaseColor = "#4169E1";
    } else if (i < 4) {
      phaseStr = "Phase 2: Core Competency";
      phaseColor = "#8064E8";
    } else {
      phaseStr = "Phase 3: Advanced Mastery";
      phaseColor = "#20B486";
    }

    bool isCompleted = (task.getStatus() == "Completed");
    bool isInProgress = (task.getStatus() == "In Progress");

    QFrame *card = new QFrame();
    QString borderStyle =
        isCompleted
            ? "border: 1px solid #A7F3D0; background-color: #FFFFFF;"
            : (isInProgress
                   ? "border: 1px solid #D7E1FD; background-color: #FFFFFF;"
                   : "border: 1px solid #E8EAF2; background-color: #FFFFFF;");

    card->setStyleSheet(QString("QFrame {"
                                "  %1"
                                "  border-radius: 14px;"
                                "  padding: 18px 20px;"
                                "}")
                            .arg(borderStyle));
    Theme::applyCardShadow(card, 12, 3, 14);

    QHBoxLayout *cardLayout = new QHBoxLayout(card);
    cardLayout->setSpacing(18);

    // Milestone Number Circle
    QFrame *numCircle = new QFrame();
    numCircle->setFixedSize(46, 46);
    QString circleBg =
        isCompleted ? "#20B486" : (isInProgress ? "#4169E1" : "#EEF2FF");
    QString circleFg = (isCompleted || isInProgress) ? "#FFFFFF" : "#4169E1";
    numCircle->setStyleSheet(
        QString("background-color: %1; border-radius: 23px;").arg(circleBg));

    QVBoxLayout *circleLayout = new QVBoxLayout(numCircle);
    circleLayout->setContentsMargins(0, 0, 0, 0);
    circleLayout->setAlignment(Qt::AlignCenter);

    QLabel *numLbl = new QLabel(QString("#%1").arg(i + 1), numCircle);
    numLbl->setStyleSheet(QString("color: %1; font-weight: 800; font-size: "
                                  "14px; background: transparent;")
                              .arg(circleFg));
    numLbl->setAlignment(Qt::AlignCenter);
    circleLayout->addWidget(numLbl);

    cardLayout->addWidget(numCircle);

    // Center Content Column
    QVBoxLayout *contentCol = new QVBoxLayout();
    contentCol->setSpacing(6);

    // Row 1: Phase Badge + Priority Score
    QHBoxLayout *r1 = new QHBoxLayout();
    r1->setSpacing(8);

    QLabel *phaseBadge = new QLabel(phaseStr);
    phaseBadge->setStyleSheet(
        QString("background-color: %115; color: %1; font-size: 10px; "
                "font-weight: 700; padding: 3px 8px; border-radius: 8px;")
            .arg(phaseColor));

    double score = task.getPriorityScore();
    QLabel *scoreBadge = new QLabel(
        QString("Priority Score: %1").arg(QString::number(score, 'f', 2)));
    scoreBadge->setToolTip(
        QString("Normalized: (0.5 x Gap:%1) + (0.3 x Imp:%2) + (0.2 x Urg:%3)")
            .arg(QString::number(matchGap.getNormalizedGapScore(), 'f', 2))
            .arg(QString::number(matchGap.getNormalizedImportanceScore(), 'f',
                                 2))
            .arg(
                QString::number(matchGap.getNormalizedUrgencyScore(), 'f', 2)));
    scoreBadge->setStyleSheet(
        "background-color: #F3F0FD; color: #8064E8; font-size: 10px; "
        "font-weight: 700; padding: 3px 8px; border-radius: 8px;");

    r1->addWidget(phaseBadge);
    r1->addWidget(scoreBadge);
    r1->addStretch();
    contentCol->addLayout(r1);

    // Row 2: Task Title & Skill Name
    QLabel *titleLabel =
        new QLabel(QString("%1: %2")
                       .arg(QString::fromStdString(matchGap.getSkillName()))
                       .arg(QString::fromStdString(task.getTitle())));
    titleLabel->setStyleSheet("font-size: 15px; font-weight: 800; color: "
                              "#202B45; background: transparent;");
    contentCol->addWidget(titleLabel);

    // Row 3: Proficiency Transition + Dates
    QHBoxLayout *r3 = new QHBoxLayout();
    r3->setSpacing(12);

    QString profStr = QString("Level %1 ➔ Level %2 (Gap: %3)")
                          .arg(matchGap.getCurrentLevel())
                          .arg(matchGap.getRequiredLevel())
                          .arg(matchGap.calculateGap());
    QLabel *profBadge = new QLabel(profStr);
    profBadge->setStyleSheet(
        "background-color: #EEF2FF; color: #4169E1; font-size: 11px; "
        "font-weight: 700; padding: 3px 8px; border-radius: 8px;");

    QLabel *dateLabel =
        new QLabel(QString("📅 %1 to %2")
                       .arg(QString::fromStdString(task.getStartDate()))
                       .arg(QString::fromStdString(task.getDueDate())));
    dateLabel->setStyleSheet(
        "font-size: 11px; color: #7A849B; background: transparent;");

    r3->addWidget(profBadge);
    r3->addWidget(dateLabel);
    r3->addStretch();
    contentCol->addLayout(r3);

    // Row 4: Progress Bar
    QProgressBar *pBar = new QProgressBar();
    pBar->setRange(0, 100);
    pBar->setValue(task.getProgress());
    pBar->setFixedHeight(8);
    pBar->setTextVisible(false);
    if (isCompleted) {
      pBar->setStyleSheet("QProgressBar { background-color: #E8EAF2; "
                          "border-radius: 4px; } QProgressBar::chunk { "
                          "background-color: #20B486; border-radius: 4px; }");
    } else {
      pBar->setStyleSheet("QProgressBar { background-color: #E8EAF2; "
                          "border-radius: 4px; } QProgressBar::chunk { "
                          "background-color: #4169E1; border-radius: 4px; }");
    }
    contentCol->addWidget(pBar);

    cardLayout->addLayout(contentCol, 1);

    // Right Action Column
    QVBoxLayout *actionCol = new QVBoxLayout();
    actionCol->setSpacing(8);
    actionCol->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // Status Badge
    QLabel *statusBadge = new QLabel(QString::fromStdString(task.getStatus()));
    statusBadge->setAlignment(Qt::AlignCenter);
    if (isCompleted) {
      statusBadge->setStyleSheet(
          "background-color: #E8F8F3; color: #20B486; font-size: 11px; "
          "font-weight: 700; padding: 4px 12px; border-radius: 10px;");
    } else if (isInProgress) {
      statusBadge->setStyleSheet(
          "background-color: #FEF6EC; color: #F3A640; font-size: 11px; "
          "font-weight: 700; padding: 4px 12px; border-radius: 10px;");
    } else {
      statusBadge->setStyleSheet(
          "background-color: #F1F5F9; color: #7A849B; font-size: 11px; "
          "font-weight: 700; padding: 4px 12px; border-radius: 10px;");
    }
    actionCol->addWidget(statusBadge);

    // Status change quick buttons
    QHBoxLayout *quickBtns = new QHBoxLayout();
    quickBtns->setSpacing(6);

    QPushButton *btnInProg = new QPushButton("In Progress");
    btnInProg->setStyleSheet(
        "QPushButton { background-color: #FEF6EC; color: #F3A640; font-weight: "
        "700; font-size: 11px; padding: 5px 10px; border: 1px solid #FCD34D; "
        "border-radius: 6px; }"
        "QPushButton:hover { background-color: #FDE68A; }");
    btnInProg->setCursor(Qt::PointingHandCursor);
    connect(btnInProg, &QPushButton::clicked, this,
            [this, i]() { onMarkTaskInProgress(static_cast<int>(i)); });

    QPushButton *btnComp = new QPushButton("Complete");
    btnComp->setStyleSheet(
        "QPushButton { background-color: #E8F8F3; color: #20B486; font-weight: "
        "700; font-size: 11px; padding: 5px 10px; border: 1px solid #A7F3D0; "
        "border-radius: 6px; }"
        "QPushButton:hover { background-color: #A7F3D0; }");
    btnComp->setCursor(Qt::PointingHandCursor);
    connect(btnComp, &QPushButton::clicked, this,
            [this, i]() { onMarkTaskCompleted(static_cast<int>(i)); });

    quickBtns->addWidget(btnInProg);
    quickBtns->addWidget(btnComp);
    actionCol->addLayout(quickBtns);

    // Open Material Button
    QPushButton *viewResBtn = new QPushButton("📚 Learning Material");
    viewResBtn->setStyleSheet(Theme::secondaryButtonStyle());
    viewResBtn->setCursor(Qt::PointingHandCursor);
    int skillId = task.getSkillId();
    connect(viewResBtn, &QPushButton::clicked, this, [this, skillId]() {
      RecommendationService recService;
      auto res = recService.getResourcesForSkill(skillId);
      if (!res.empty()) {
        onViewResource(QString::fromStdString(res.front()->getUrl()));
      } else {
        QMessageBox::information(
            this, "Learning Material",
            "No specific URL resource linked for this skill yet.");
      }
    });
    actionCol->addWidget(viewResBtn);

    cardLayout->addLayout(actionCol);

    m_milestonesLayout->addWidget(card);
  }
}

void RoadmapPage::renderResourceCards() {
  // Clear old resource widgets
  QLayoutItem *item;
  while ((item = m_resourcesLayout->takeAt(0)) != nullptr) {
    if (item->widget())
      delete item->widget();
    delete item;
  }

  int selectedSkillId = m_resourceSkillFilter->currentData().toInt();
  QString selectedType = m_resourceTypeFilter->currentText();

  RecommendationService recService;
  std::vector<std::unique_ptr<LearningResource>> resources;

  if (selectedSkillId > 0) {
    resources =
        recService.filterResources(selectedSkillId, selectedType.toStdString());
  } else {
    std::unordered_set<int> seenSkills;
    for (const auto &g : m_currentGaps) {
      if (g.hasGap() && seenSkills.find(g.getSkillId()) == seenSkills.end()) {
        seenSkills.insert(g.getSkillId());
        auto subset = recService.filterResources(g.getSkillId(),
                                                 selectedType.toStdString());
        for (auto &itemRef : subset) {
          resources.push_back(std::move(itemRef));
        }
      }
    }
    if (resources.empty()) {
      resources = recService.filterResources(-1, selectedType.toStdString());
    }
  }

  if (resources.empty()) {
    QLabel *emptyLbl =
        new QLabel("No resources found matching the selected filter criteria.");
    emptyLbl->setStyleSheet("color: #7A849B; font-size: 12px; padding: 12px; "
                            "background: transparent;");
    emptyLbl->setAlignment(Qt::AlignCenter);
    m_resourcesLayout->addWidget(emptyLbl);
    return;
  }

  for (const auto &res : resources) {
    QFrame *rCard = new QFrame();
    rCard->setStyleSheet("QFrame {"
                         "  background-color: #F8FAFC;"
                         "  border: 1px solid #E8EAF2;"
                         "  border-radius: 10px;"
                         "  padding: 12px 16px;"
                         "}"
                         "QFrame:hover {"
                         "  border-color: #4169E1;"
                         "}");

    QHBoxLayout *rLayout = new QHBoxLayout(rCard);
    rLayout->setSpacing(14);
    rLayout->setContentsMargins(4, 4, 4, 4);

    // Type Badge
    QString rType = QString::fromStdString(res->resourceType());
    QString typeBg = (rType == "Video")
                         ? "#EEF2FF"
                         : (rType == "Article" ? "#E8F8F3" : "#F3F0FD");
    QString typeFg = (rType == "Video")
                         ? "#4169E1"
                         : (rType == "Article" ? "#20B486" : "#8064E8");

    QLabel *tBadge = new QLabel(rType);
    tBadge->setStyleSheet(
        QString("background-color: %1; color: %2; font-size: 11px; "
                "font-weight: 700; padding: 4px 10px; border-radius: 8px;")
            .arg(typeBg, typeFg));
    tBadge->setFixedWidth(64);
    tBadge->setAlignment(Qt::AlignCenter);
    rLayout->addWidget(tBadge);

    // Content
    QVBoxLayout *tCol = new QVBoxLayout();
    tCol->setSpacing(2);

    QLabel *tTitle = new QLabel(QString::fromStdString(res->getTitle()));
    tTitle->setStyleSheet("font-size: 13px; font-weight: 700; color: #202B45; "
                          "background: transparent;");

    QLabel *tDesc =
        new QLabel(QString("%1 • %2")
                       .arg(QString::fromStdString(res->getDifficulty()))
                       .arg(QString::fromStdString(res->getDuration())));
    tDesc->setStyleSheet(
        "font-size: 11px; color: #7A849B; background: transparent;");

    tCol->addWidget(tTitle);
    tCol->addWidget(tDesc);
    rLayout->addLayout(tCol, 1);

    // Open Link Button
    QPushButton *openBtn = new QPushButton("Open Resource ↗");
    openBtn->setStyleSheet(Theme::secondaryButtonStyle());
    openBtn->setCursor(Qt::PointingHandCursor);
    QString urlStr = QString::fromStdString(res->getUrl());
    connect(openBtn, &QPushButton::clicked, this,
            [this, urlStr]() { onViewResource(urlStr); });
    rLayout->addWidget(openBtn);

    m_resourcesLayout->addWidget(rCard);
  }
}

void RoadmapPage::refreshData() {
  Student *student = AuthService::instance().getCurrentStudent();
  int studentId = student ? student->getId() : 1;

  UserRepository userRepo;
  CareerRepository careerRepo;
  int targetRoleId = userRepo.getTargetRole(studentId);
  if (targetRoleId <= 0 && student && student->getTargetRoleId() > 0) {
    targetRoleId = student->getTargetRoleId();
  }
  if (targetRoleId <= 0) {
    auto history = careerRepo.getAnalysisHistoryForStudent(studentId);
    if (!history.empty()) {
      targetRoleId = history.front().roleId;
    }
  }
  if (targetRoleId <= 0) {
    targetRoleId = 1;
  }

  QString roleTitle = "Career Role";
  auto roleOpt = careerRepo.getJobRoleById(targetRoleId);
  if (roleOpt.has_value()) {
    roleTitle = QString::fromStdString(roleOpt->getTitle());
  }

  SkillAnalysisService analysisService;
  auto result = analysisService.analyzeGaps(studentId, targetRoleId, false);
  if (m_readinessLabel) {
    m_readinessLabel->setText(
        QString("Career Readiness: %1%")
            .arg(QString::number(result.matchPercentage, 'f', 1)));
  }

  buildRoadmap(result.allGaps, roleTitle);

  LearningRepository learningRepo;
  auto activeTasks = learningRepo.getTasksForStudent(studentId);
  if (!activeTasks.empty()) {
    m_statusLabel->setText(QString("ℹ️ %1 task(s) active in persistent plan.")
                               .arg(activeTasks.size()));
    m_statusLabel->setStyleSheet("color: #4169E1; font-weight: 700;");
  } else {
    m_statusLabel->clear();
  }
}

void RoadmapPage::setRoadmapGaps(const std::vector<SkillGap> &gaps) {
  buildRoadmap(gaps, "Current Gap Analysis");
}

void RoadmapPage::onGenerateRoadmap() {
  refreshData();
  m_statusLabel->setText("✓ Roadmap regenerated via STL priority queue.");
  m_statusLabel->setStyleSheet("color: #20B486; font-weight: 700;");
}

void RoadmapPage::onRefreshRoadmap() { refreshData(); }

void RoadmapPage::buildRoadmap(const std::vector<SkillGap> &gaps,
                               const QString &roleTitle) {
  m_currentGaps = gaps;
  Student *student = AuthService::instance().getCurrentStudent();
  int studentId = student ? student->getId() : 1;

  if (m_roleTitleLabel) {
    m_roleTitleLabel->setText(
        QString("Target Track: %1")
            .arg(roleTitle.isEmpty() ? "Career Analysis" : roleTitle));
  }

  RoadmapService roadmapService;
  m_currentTasks = roadmapService.generateRoadmap(studentId, gaps);

  // Update Skill Filter ComboBox
  m_resourceSkillFilter->blockSignals(true);
  m_resourceSkillFilter->clear();
  m_resourceSkillFilter->addItem("All Skills", 0);
  for (const auto &g : gaps) {
    if (g.hasGap()) {
      m_resourceSkillFilter->addItem(QString::fromStdString(g.getSkillName()),
                                     g.getSkillId());
    }
  }
  m_resourceSkillFilter->blockSignals(false);

  // Count statistics
  int totalCount = static_cast<int>(m_currentTasks.size());
  int notStarted = 0;
  int inProgress = 0;
  int completed = 0;
  int totalWeeks = 0;

  for (const auto &t : m_currentTasks) {
    if (t.getStatus() == "Completed")
      completed++;
    else if (t.getStatus() == "In Progress")
      inProgress++;
    else
      notStarted++;
  }

  for (const auto &g : gaps) {
    if (g.hasGap()) {
      totalWeeks += std::max(1, g.calculateGap() * 2);
    }
  }

  m_cardTotal->setValue(QString::number(totalCount));
  m_cardNotStarted->setValue(QString::number(notStarted));
  m_cardInProgress->setValue(QString::number(inProgress));
  m_cardCompleted->setValue(QString::number(completed));
  m_cardDuration->setValue(QString("%1 Wks").arg(totalWeeks));
  m_cardDuration->setSubtitle(QString("%1 calendar days").arg(totalWeeks * 7));

  renderMilestoneCards();
  renderResourceCards();
}

void RoadmapPage::onFilterResources() { renderResourceCards(); }

void RoadmapPage::onViewResource(const QString &url) {
  if (url.trimmed().isEmpty()) {
    QMessageBox::warning(
        this, "Empty URL",
        "The requested resource does not have an access URL specified.");
    return;
  }

  QUrl qurl(url);
  if (!qurl.isValid() ||
      (qurl.scheme() != "http" && qurl.scheme() != "https")) {
    QMessageBox::warning(
        this, "Invalid Resource URL",
        QString("The resource URL is invalid or an identified placeholder:\n%1")
            .arg(url));
    return;
  }

  if (!QDesktopServices::openUrl(qurl)) {
    QMessageBox::warning(
        this, "Browser Error",
        QString("Could not open system browser for URL:\n%1").arg(url));
  }
}

void RoadmapPage::onMarkTaskInProgress(int taskIndex) {
  if (taskIndex >= 0 &&
      static_cast<size_t>(taskIndex) < m_currentTasks.size()) {
    m_currentTasks[taskIndex].setStatus("In Progress");
    m_currentTasks[taskIndex].updateProgress(50);
    onSaveRoadmap();
    buildRoadmap(m_currentGaps);
  }
}

void RoadmapPage::onMarkTaskCompleted(int taskIndex) {
  if (taskIndex >= 0 &&
      static_cast<size_t>(taskIndex) < m_currentTasks.size()) {
    m_currentTasks[taskIndex].setStatus("Completed");
    m_currentTasks[taskIndex].updateProgress(100);
    onSaveRoadmap();
    buildRoadmap(m_currentGaps);
  }
}

void RoadmapPage::onSaveRoadmap() {
  Student *student = AuthService::instance().getCurrentStudent();
  if (!student)
    return;

  if (m_currentTasks.empty()) {
    m_statusLabel->setText("No tasks in roadmap to save.");
    m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700;");
    return;
  }

  RoadmapService roadmapService;
  if (roadmapService.saveRoadmapTasks(student->getId(), m_currentTasks)) {
    m_statusLabel->setText(
        QString("✓ Saved %1 tasks to active plan!").arg(m_currentTasks.size()));
    m_statusLabel->setStyleSheet("color: #20B486; font-weight: 700;");
  } else {
    m_statusLabel->setText("Failed to save some tasks.");
    m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700;");
  }
}

void RoadmapPage::onClearRoadmap() {
  Student *student = AuthService::instance().getCurrentStudent();
  if (!student)
    return;

  auto reply = QMessageBox::question(
      this, "Clear Active Plan",
      "Are you sure you want to clear your active learning tasks?",
      QMessageBox::Yes | QMessageBox::No);
  if (reply != QMessageBox::Yes)
    return;

  LearningRepository repo;
  if (repo.deleteTasksForStudent(student->getId())) {
    m_statusLabel->setText("✓ Cleared active tasks for student.");
    m_statusLabel->setStyleSheet("color: #7A849B; font-weight: 700;");
    refreshData();
  } else {
    m_statusLabel->setText("Failed to clear tasks.");
    m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700;");
  }
}
