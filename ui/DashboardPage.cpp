#include "DashboardPage.h"
#include "../services/AuthService.h"
#include "../services/ProgressService.h"
#include "../services/SkillAnalysisService.h"
#include "../services/RecommendationService.h"
#include "../database/SkillRepository.h"
#include "../database/CareerRepository.h"
#include "../database/UserRepository.h"
#include "../database/LearningRepository.h"
#include "Theme.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QPieSlice>
#include <QBarSet>

DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent),
      m_welcomeNameLabel(nullptr),
      m_studentMetaLabel(nullptr),
      m_targetGoalBadge(nullptr),
      m_avatar(nullptr),
      m_cardSkills(nullptr),
      m_cardTarget(nullptr),
      m_cardReadiness(nullptr),
      m_cardTasks(nullptr),
      m_circularReadiness(nullptr),
      m_readinessStatusLabel(nullptr),
      m_gapsCountLabel(nullptr),
      m_actionCard(nullptr),
      m_actionIconLabel(nullptr),
      m_actionTitleLabel(nullptr),
      m_actionDescLabel(nullptr),
      m_actionBtn(nullptr),
      m_actionTargetPage(3),
      m_proficiencyChart(nullptr),
      m_proficiencyChartView(nullptr),
      m_proficiencySeries(nullptr),
      m_proficiencyAxisX(nullptr),
      m_proficiencyAxisY(nullptr),
      m_taskChart(nullptr),
      m_taskChartView(nullptr),
      m_taskPieSeries(nullptr),
      m_emptyState(nullptr),
      m_analyticsContainer(nullptr) {
    setupUi();
}

void DashboardPage::setupUi() {
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
    // 1. Welcome Card (Compact, Elegant, Light)
    // ==========================================
    QFrame* welcomeCard = new QFrame();
    welcomeCard->setObjectName("welcomeCard");
    welcomeCard->setStyleSheet(
        "#welcomeCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
    );
    Theme::applyCardShadow(welcomeCard, 14, 3, 18);

    QHBoxLayout* welcomeLayout = new QHBoxLayout(welcomeCard);
    welcomeLayout->setSpacing(18);
    welcomeLayout->setContentsMargins(20, 16, 20, 16);

    m_avatar = new StudentAvatar("Alex Morgan", 56);
    welcomeLayout->addWidget(m_avatar);

    QVBoxLayout* welcomeTextCol = new QVBoxLayout();
    welcomeTextCol->setSpacing(4);

    m_welcomeNameLabel = new QLabel("Welcome back, Alex Morgan! 👋");
    m_welcomeNameLabel->setStyleSheet("font-size: 20px; font-weight: 800; color: #202B45; border: none;");

    m_studentMetaLabel = new QLabel("Department of Computer Science • State University of Technology");
    m_studentMetaLabel->setStyleSheet("font-size: 13px; color: #7A849B; font-weight: 500; border: none;");

    welcomeTextCol->addWidget(m_welcomeNameLabel);
    welcomeTextCol->addWidget(m_studentMetaLabel);
    welcomeLayout->addLayout(welcomeTextCol);
    welcomeLayout->addStretch();

    // Goal badge on the right
    QVBoxLayout* goalCol = new QVBoxLayout();
    goalCol->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    goalCol->setSpacing(4);

    QLabel* goalTitle = new QLabel("TARGET CAREER TRACK");
    goalTitle->setStyleSheet("font-size: 10px; font-weight: 700; color: #7A849B; letter-spacing: 0.6px; border: none;");
    goalTitle->setAlignment(Qt::AlignRight);

    m_targetGoalBadge = new QLabel("None Selected");
    m_targetGoalBadge->setStyleSheet(
        "background-color: #F3F0FD; color: #8064E8; font-weight: 700; font-size: 12px; padding: 6px 14px; border-radius: 12px; border: none;"
    );
    m_targetGoalBadge->setAlignment(Qt::AlignCenter);

    goalCol->addWidget(goalTitle);
    goalCol->addWidget(m_targetGoalBadge);
    welcomeLayout->addLayout(goalCol);

    mainLayout->addWidget(welcomeCard);

    // ==========================================
    // 2. Summary Metric Cards Row (4 Cards)
    // ==========================================
    QGridLayout* metricsGrid = new QGridLayout();
    metricsGrid->setSpacing(18);

    m_cardSkills = new MetricCard("🎯", "Tracked Skills", "0", "Evaluated in personal inventory", "#4169E1");
    m_cardTarget = new MetricCard("💼", "Target Career", "None", "Active career objective", "#8064E8");
    m_cardReadiness = new MetricCard("📈", "Readiness Match", "0.0%", "Curriculum requirement match", "#20B486");
    m_cardTasks = new MetricCard("⚡", "Active Tasks", "0", "In prioritized roadmap", "#F3A640");

    metricsGrid->addWidget(m_cardSkills, 0, 0);
    metricsGrid->addWidget(m_cardTarget, 0, 1);
    metricsGrid->addWidget(m_cardReadiness, 0, 2);
    metricsGrid->addWidget(m_cardTasks, 0, 3);

    mainLayout->addLayout(metricsGrid);

    // ==========================================
    // 3. Recommended Next Action Card (Dynamic Data)
    // ==========================================
    m_actionCard = new QFrame();
    m_actionCard->setObjectName("actionCard");
    m_actionCard->setStyleSheet(
        "#actionCard {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FFFFFF, stop:1 #F9FAFF);"
        "  border: 1px solid #D7E1FD;"
        "  border-left: 5px solid #4169E1;"
        "  border-radius: 14px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
    );
    Theme::applyCardShadow(m_actionCard, 12, 3, 16);

    QHBoxLayout* actionLayout = new QHBoxLayout(m_actionCard);
    actionLayout->setSpacing(16);
    actionLayout->setContentsMargins(18, 14, 18, 14);

    m_actionIconLabel = new QLabel("💡");
    m_actionIconLabel->setFont(QFont("Segoe UI Emoji", 20));
    m_actionIconLabel->setStyleSheet("border: none;");
    actionLayout->addWidget(m_actionIconLabel);

    QVBoxLayout* actionTextCol = new QVBoxLayout();
    actionTextCol->setSpacing(3);

    QLabel* recBadge = new QLabel("RECOMMENDED NEXT ACTION");
    recBadge->setStyleSheet("font-size: 10px; font-weight: 800; color: #4169E1; letter-spacing: 0.6px; border: none;");

    m_actionTitleLabel = new QLabel("Select a Target Career");
    m_actionTitleLabel->setStyleSheet("font-size: 15px; font-weight: 700; color: #202B45; border: none;");

    m_actionDescLabel = new QLabel("Choose a target career track to calculate your competency gap and generate an STL-prioritized roadmap.");
    m_actionDescLabel->setStyleSheet("font-size: 12px; color: #7A849B; border: none;");
    m_actionDescLabel->setWordWrap(true);

    actionTextCol->addWidget(recBadge);
    actionTextCol->addWidget(m_actionTitleLabel);
    actionTextCol->addWidget(m_actionDescLabel);
    actionLayout->addLayout(actionTextCol, 1);

    m_actionBtn = new QPushButton("Take Action →");
    m_actionBtn->setStyleSheet(Theme::primaryButtonStyle());
    m_actionBtn->setCursor(Qt::PointingHandCursor);
    connect(m_actionBtn, &QPushButton::clicked, this, [this]() {
        emit navigateToPage(m_actionTargetPage);
    });
    actionLayout->addWidget(m_actionBtn);

    mainLayout->addWidget(m_actionCard);

    // ==========================================
    // 4. Analytics Section (Circular Indicator + Charts)
    // ==========================================
    m_analyticsContainer = new QWidget();
    QVBoxLayout* analyticsRoot = new QVBoxLayout(m_analyticsContainer);
    analyticsRoot->setContentsMargins(0, 0, 0, 0);
    analyticsRoot->setSpacing(20);

    QHBoxLayout* chartsRow = new QHBoxLayout();
    chartsRow->setSpacing(20);

    // Left: Career Readiness Visual Card
    QFrame* readinessCard = new QFrame();
    readinessCard->setObjectName("readinessCard");
    readinessCard->setStyleSheet(
        "#readinessCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
    );
    Theme::applyCardShadow(readinessCard, 14, 3, 16);

    QVBoxLayout* readinessLayout = new QVBoxLayout(readinessCard);
    readinessLayout->setContentsMargins(18, 18, 18, 18);
    readinessLayout->setSpacing(10);

    QLabel* readinessCardTitle = new QLabel("Career Readiness Index");
    readinessCardTitle->setStyleSheet("font-size: 15px; font-weight: 700; color: #202B45; border: none;");
    readinessLayout->addWidget(readinessCardTitle);

    m_circularReadiness = new CircularProgress();
    m_circularReadiness->setFixedSize(150, 150);
    readinessLayout->addWidget(m_circularReadiness, 0, Qt::AlignCenter);

    m_readinessStatusLabel = new QLabel("Analysis Pending");
    m_readinessStatusLabel->setAlignment(Qt::AlignCenter);
    m_readinessStatusLabel->setStyleSheet("font-size: 13px; font-weight: 700; color: #7A849B; border: none;");
    readinessLayout->addWidget(m_readinessStatusLabel);

    m_gapsCountLabel = new QLabel("No career role analyzed yet");
    m_gapsCountLabel->setAlignment(Qt::AlignCenter);
    m_gapsCountLabel->setStyleSheet("font-size: 11px; color: #7A849B; border: none;");
    readinessLayout->addWidget(m_gapsCountLabel);

    readinessLayout->addStretch();
    chartsRow->addWidget(readinessCard, 1);

    // Middle: Skill Proficiency Overview Bar Chart
    QFrame* profCard = new QFrame();
    profCard->setObjectName("profCard");
    profCard->setStyleSheet(
        "#profCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
    );
    Theme::applyCardShadow(profCard, 14, 3, 16);

    QVBoxLayout* profLayout = new QVBoxLayout(profCard);
    profLayout->setContentsMargins(18, 18, 18, 18);
    profLayout->setSpacing(8);

    QLabel* profTitle = new QLabel("Skill Proficiency Distribution");
    profTitle->setStyleSheet("font-size: 15px; font-weight: 700; color: #202B45; border: none;");
    profLayout->addWidget(profTitle);

    m_proficiencyChart = new QChart();
    m_proficiencyChart->setAnimationOptions(QChart::SeriesAnimations);
    m_proficiencyChart->setBackgroundVisible(false);
    m_proficiencyChart->legend()->setVisible(false);
    m_proficiencyChart->setMargins(QMargins(0, 0, 0, 0));

    m_proficiencyChartView = new QChartView(m_proficiencyChart);
    m_proficiencyChartView->setRenderHint(QPainter::Antialiasing);
    m_proficiencyChartView->setMinimumHeight(210);
    profLayout->addWidget(m_proficiencyChartView);

    chartsRow->addWidget(profCard, 2);

    // Right: Learning Task Distribution Donut Chart
    QFrame* taskCard = new QFrame();
    taskCard->setObjectName("taskCard");
    taskCard->setStyleSheet(
        "#taskCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
    );
    Theme::applyCardShadow(taskCard, 14, 3, 16);

    QVBoxLayout* taskLayout = new QVBoxLayout(taskCard);
    taskLayout->setContentsMargins(18, 18, 18, 18);
    taskLayout->setSpacing(8);

    QLabel* taskTitle = new QLabel("Roadmap Task Status");
    taskTitle->setStyleSheet("font-size: 15px; font-weight: 700; color: #202B45; border: none;");
    taskLayout->addWidget(taskTitle);

    m_taskChart = new QChart();
    m_taskChart->setAnimationOptions(QChart::SeriesAnimations);
    m_taskChart->setBackgroundVisible(false);
    m_taskChart->legend()->setAlignment(Qt::AlignBottom);
    m_taskChart->legend()->setFont(QFont("Segoe UI", 9));
    m_taskChart->setMargins(QMargins(0, 0, 0, 0));

    m_taskPieSeries = new QPieSeries();
    m_taskPieSeries->setHoleSize(0.48);
    m_taskChart->addSeries(m_taskPieSeries);

    m_taskChartView = new QChartView(m_taskChart);
    m_taskChartView->setRenderHint(QPainter::Antialiasing);
    m_taskChartView->setMinimumHeight(210);
    taskLayout->addWidget(m_taskChartView);

    chartsRow->addWidget(taskCard, 2);

    analyticsRoot->addLayout(chartsRow);
    mainLayout->addWidget(m_analyticsContainer);

    // ==========================================
    // 5. Quick Workflow Shortcuts (Clean Cards)
    // ==========================================
    QLabel* shortcutsTitle = new QLabel("Quick Platform Actions");
    shortcutsTitle->setStyleSheet("font-size: 16px; font-weight: 800; color: #202B45; border: none; padding-top: 6px;");
    mainLayout->addWidget(shortcutsTitle);

    QHBoxLayout* shortcutsRow = new QHBoxLayout();
    shortcutsRow->setSpacing(16);

    auto makeShortcutCard = [this](const QString& icon, const QString& title, const QString& desc, int page) -> QWidget* {
        QFrame* card = new QFrame();
        card->setObjectName("shortcutCard");
        card->setStyleSheet(
            "#shortcutCard {"
            "  background-color: #FFFFFF;"
            "  border: 1px solid #E8EAF2;"
            "  border-radius: 12px;"
            "}"
            "#shortcutCard:hover {"
            "  border: 1px solid #4169E1;"
            "}"
            "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
        );
        Theme::applyCardShadow(card, 10, 2, 12);

        QVBoxLayout* l = new QVBoxLayout(card);
        l->setContentsMargins(14, 14, 14, 14);
        l->setSpacing(6);

        QLabel* ic = new QLabel(icon);
        ic->setFont(QFont("Segoe UI Emoji", 16));
        ic->setStyleSheet("border: none;");

        QLabel* t = new QLabel(title);
        t->setStyleSheet("font-size: 13px; font-weight: 700; color: #202B45; border: none;");

        QLabel* d = new QLabel(desc);
        d->setStyleSheet("font-size: 11px; color: #7A849B; border: none;");
        d->setWordWrap(true);

        QPushButton* btn = new QPushButton("Open →");
        btn->setStyleSheet(Theme::secondaryButtonStyle());
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [this, page]() { emit navigateToPage(page); });

        l->addWidget(ic);
        l->addWidget(t);
        l->addWidget(d);
        l->addStretch();
        l->addWidget(btn);

        return card;
    };

    shortcutsRow->addWidget(makeShortcutCard("🎯", "Manage Skills", "Self-assess competency levels across tools & topics.", 2));
    shortcutsRow->addWidget(makeShortcutCard("🔍", "Career Analysis", "Run benchmark role comparisons & gap analysis.", 3));
    shortcutsRow->addWidget(makeShortcutCard("🗺️", "Learning Roadmap", "View STL priority-queue sequenced task timeline.", 5));
    shortcutsRow->addWidget(makeShortcutCard("⚡", "Academic Evaluation", "Run live empirical benchmarks (Procedural vs OOP).", 7));

    mainLayout->addLayout(shortcutsRow);

    // Empty state widget
    m_emptyState = new EmptyStateWidget("📊", "No Career Analysis Data Available",
                                       "Select a target career or paste a job description to calculate your readiness score.",
                                       "Select Career Track", this);
    connect(m_emptyState, &EmptyStateWidget::actionClicked, this, [this]() {
        emit navigateToPage(3);
    });
    m_emptyState->setVisible(false);
    mainLayout->addWidget(m_emptyState);

    mainLayout->addStretch();

    scrollArea->setWidget(contentWidget);
    outerLayout->addWidget(scrollArea);
}

void DashboardPage::updateCharts(int l1, int l2, int l3, int completed, int inProg, int notStarted) {
    // 1. Proficiency Bar Chart
    m_proficiencyChart->removeAllSeries();
    for (auto ax : m_proficiencyChart->axes()) {
        m_proficiencyChart->removeAxis(ax);
    }

    QBarSet* setL1 = new QBarSet("Beginner (L1)");
    setL1->setColor(QColor("#4169E1"));
    *setL1 << l1;

    QBarSet* setL2 = new QBarSet("Intermediate (L2)");
    setL2->setColor(QColor("#8064E8"));
    *setL2 << l2;

    QBarSet* setL3 = new QBarSet("Advanced (L3)");
    setL3->setColor(QColor("#20B486"));
    *setL3 << l3;

    QBarSeries* series = new QBarSeries();
    series->append(setL1);
    series->append(setL2);
    series->append(setL3);
    m_proficiencyChart->addSeries(series);

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(QStringList{"Skill Levels"});
    axisX->setLabelsFont(QFont("Segoe UI", 9));
    m_proficiencyChart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    int maxVal = std::max({l1, l2, l3, 5});
    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, maxVal + 1);
    axisY->setTickCount(std::min(maxVal + 2, 6));
    axisY->setLabelsFont(QFont("Segoe UI", 9));
    m_proficiencyChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // 2. Task Donut Chart
    m_taskPieSeries->clear();
    int totalTasks = completed + inProg + notStarted;
    if (totalTasks == 0) {
        auto slice = m_taskPieSeries->append("No Tasks", 1);
        slice->setColor(QColor("#E8EAF2"));
    } else {
        if (completed > 0) {
            auto s = m_taskPieSeries->append(QString("Completed (%1)").arg(completed), completed);
            s->setColor(QColor("#20B486"));
        }
        if (inProg > 0) {
            auto s = m_taskPieSeries->append(QString("In Progress (%1)").arg(inProg), inProg);
            s->setColor(QColor("#4169E1"));
        }
        if (notStarted > 0) {
            auto s = m_taskPieSeries->append(QString("Not Started (%1)").arg(notStarted), notStarted);
            s->setColor(QColor("#F3A640"));
        }
    }
}

void DashboardPage::updateNextAction(int targetRoleId, double readiness, int activeTasks, int studentId) {
    if (targetRoleId <= 0) {
        m_actionIconLabel->setText("🎯");
        m_actionTitleLabel->setText("Step 1: Select a Target Career Track");
        m_actionDescLabel->setText("Explore industry roles to compare your technical skills and identify competency gaps.");
        m_actionBtn->setText("Explore Career Roles →");
        m_actionTargetPage = 3;
        return;
    }

    if (activeTasks > 0) {
        LearningRepository lRepo;
        auto tasks = lRepo.getTasksForStudent(studentId);
        std::string ongoingTitle = "";
        for (const auto& t : tasks) {
            if (t.getStatus() == "In Progress") {
                ongoingTitle = t.getTitle();
                break;
            }
        }
        if (ongoingTitle.empty() && !tasks.empty()) {
            ongoingTitle = tasks.front().getTitle();
        }

        m_actionIconLabel->setText("🚀");
        m_actionTitleLabel->setText(QString("Continue Task: %1").arg(QString::fromStdString(ongoingTitle)));
        m_actionDescLabel->setText("You have active tasks scheduled in your roadmap. Work through exercises and log your progress.");
        m_actionBtn->setText("Track Progress →");
        m_actionTargetPage = 6;
        return;
    }

    if (readiness < 100.0) {
        SkillAnalysisService aService;
        auto result = aService.analyzeGaps(studentId, targetRoleId, false);
        if (!result.allGaps.empty()) {
            SkillGap topGap = result.allGaps.front();
            for (const auto& g : result.allGaps) {
                if (g.calculateGap() > topGap.calculateGap()) {
                    topGap = g;
                }
            }
            if (topGap.hasGap()) {
                m_actionIconLabel->setText("⚡");
                m_actionTitleLabel->setText(QString("Top Skill Gap: Elevate %1").arg(QString::fromStdString(topGap.getSkillName())));
                m_actionDescLabel->setText(QString("Current Level is %1; required for %2 is Level %3. Generate a personalized learning roadmap to close this gap.")
                    .arg(topGap.getCurrentLevel())
                    .arg(QString::fromStdString(result.jobRoleTitle))
                    .arg(topGap.getRequiredLevel()));
                m_actionBtn->setText("Generate Roadmap →");
                m_actionTargetPage = 5;
                return;
            }
        }
    }

    m_actionIconLabel->setText("🏆");
    m_actionTitleLabel->setText("All Target Requirements Met!");
    m_actionDescLabel->setText("Outstanding achievement! Your evaluated competencies fully satisfy the benchmarks for your target career.");
    m_actionBtn->setText("View Academic Report →");
    m_actionTargetPage = 7;
}

void DashboardPage::refreshData() {
    Student* student = AuthService::instance().getCurrentStudent();
    if (!student) return;

    QString studentName = QString::fromStdString(student->getName());
    m_avatar->setName(studentName);
    m_welcomeNameLabel->setText(QString("Welcome back, %1! 👋").arg(studentName));
    m_studentMetaLabel->setText(QString("%1 • %2")
        .arg(QString::fromStdString(student->getDepartment()))
        .arg(QString::fromStdString(student->getInstitution())));

    // 1. Fetch Skills Count and breakdown
    SkillRepository skillRepo;
    auto skills = skillRepo.getStudentSkillLevels(student->getId());
    int countL1 = 0, countL2 = 0, countL3 = 0;
    for (const auto& [id, level] : skills) {
        if (level == 1) countL1++;
        else if (level == 2) countL2++;
        else if (level == 3) countL3++;
    }
    m_cardSkills->setValue(QString::number(skills.size()));
    m_cardSkills->setSubtitle(QString("%1 Beg • %2 Int • %3 Adv").arg(countL1).arg(countL2).arg(countL3));

    // 2. Fetch Target Role
    UserRepository userRepo;
    CareerRepository careerRepo;
    int targetRoleId = userRepo.getTargetRole(student->getId());
    if (targetRoleId <= 0 && student->getTargetRoleId() > 0) {
        targetRoleId = student->getTargetRoleId();
    }

    QString roleTitle = "None Selected";
    if (targetRoleId > 0) {
        auto role = careerRepo.getJobRoleById(targetRoleId);
        if (role.has_value()) {
            roleTitle = QString::fromStdString(role->getTitle());
        }
    }
    m_cardTarget->setValue(roleTitle);
    m_targetGoalBadge->setText(roleTitle);

    // 3. Career Readiness & Gaps
    double readiness = 0.0;
    int meetingCount = 0;
    int gapCount = 0;

    if (targetRoleId > 0) {
        SkillAnalysisService analysisService;
        auto res = analysisService.analyzeGaps(student->getId(), targetRoleId, false);
        readiness = res.matchPercentage;
        meetingCount = res.meetingCount;
        gapCount = res.insufficientCount + res.missingCount;

        m_cardReadiness->setValue(QString::number(readiness, 'f', 1) + "%");
        m_cardReadiness->setSubtitle(QString("%1 Met • %2 Gaps to close").arg(meetingCount).arg(gapCount));

        m_circularReadiness->setValue(readiness);
        if (readiness >= 75.0) {
            m_readinessStatusLabel->setText("Strong Readiness ✨");
            m_readinessStatusLabel->setStyleSheet("font-size: 13px; font-weight: 700; color: #20B486; border: none;");
        } else if (readiness >= 40.0) {
            m_readinessStatusLabel->setText("Moderate Readiness 📈");
            m_readinessStatusLabel->setStyleSheet("font-size: 13px; font-weight: 700; color: #F3A640; border: none;");
        } else {
            m_readinessStatusLabel->setText("Early Stage Gap 🎯");
            m_readinessStatusLabel->setStyleSheet("font-size: 13px; font-weight: 700; color: #E56B75; border: none;");
        }
        m_gapsCountLabel->setText(QString("%1 of %2 competencies satisfied")
            .arg(meetingCount).arg(res.totalRequiredSkills));
    } else {
        m_cardReadiness->setValue("0.0%");
        m_cardReadiness->setSubtitle("Select a target career track");
        m_circularReadiness->setValue(0.0);
        m_readinessStatusLabel->setText("No Role Selected");
        m_readinessStatusLabel->setStyleSheet("font-size: 13px; font-weight: 700; color: #7A849B; border: none;");
        m_gapsCountLabel->setText("Click below to select a target track");
    }

    // 4. Tasks Summary
    ProgressService progService;
    auto prog = progService.getProgressSummary(student->getId());
    m_cardTasks->setValue(QString::number(prog.totalTasks));
    m_cardTasks->setSubtitle(QString("%1 Done • %2 In Progress").arg(prog.completedTasks).arg(prog.inProgressTasks));

    // Update Next Action
    updateNextAction(targetRoleId, readiness, prog.totalTasks, student->getId());

    // Update Qt Charts
    updateCharts(countL1, countL2, countL3, prog.completedTasks, prog.inProgressTasks, prog.notStartedTasks);
}
