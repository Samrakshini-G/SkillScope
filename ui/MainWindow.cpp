#include "MainWindow.h"
#include "LoginWindow.h"
#include "../services/AuthService.h"
#include "Theme.h"
#include "components/LogoBadge.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_sidebar(nullptr),
      m_headerTitle(nullptr),
      m_headerSubtitle(nullptr),
      m_userAvatar(nullptr),
      m_userNameLabel(nullptr),
      m_userDeptLabel(nullptr),
      m_logoutBtn(nullptr),
      m_stackedWidget(nullptr),
      m_loginWindow(nullptr) {
    setupUi();
    resize(1340, 840);
    setMinimumSize(1100, 720);
    setWindowTitle("SkillScope — AI-Powered Skill Gap Detection & Career Development Platform");

    // Center on screen
    if (auto screen = QApplication::primaryScreen()) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }
}

void MainWindow::setLoginWindow(LoginWindow* loginWin) {
    m_loginWindow = loginWin;
}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // ==========================================
    // 1. Left Sidebar (Pure White Background)
    // ==========================================
    m_sidebar = new QWidget();
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setFixedWidth(250);
    m_sidebar->setStyleSheet(
        "QWidget#sidebar {"
        "  background-color: #FFFFFF;"
        "  border-right: 1px solid #E8EAF2;"
        "}"
    );

    QVBoxLayout* sidebarLayout = new QVBoxLayout(m_sidebar);
    sidebarLayout->setContentsMargins(14, 20, 14, 20);
    sidebarLayout->setSpacing(4);

    // App Branding Section with Icon Badge
    QHBoxLayout* brandRow = new QHBoxLayout();
    brandRow->setSpacing(12);
    brandRow->setContentsMargins(6, 4, 6, 12);

    LogoBadge* logoBadge = new LogoBadge(42, m_sidebar);

    QVBoxLayout* brandTextCol = new QVBoxLayout();
    brandTextCol->setSpacing(0);

    QLabel* brandTitle = new QLabel("SkillScope");
    brandTitle->setStyleSheet("color: #202B45; font-size: 20px; font-weight: 800; border: none; background: transparent;");

    QLabel* brandSub = new QLabel("Career Intelligence");
    brandSub->setStyleSheet("color: #7A849B; font-size: 11px; font-weight: 600; border: none; background: transparent;");

    brandTextCol->addWidget(brandTitle);
    brandTextCol->addWidget(brandSub);

    brandRow->addWidget(logoBadge);
    brandRow->addLayout(brandTextCol);
    brandRow->addStretch();

    sidebarLayout->addLayout(brandRow);
    brandSub->setStyleSheet("color: #7A849B; font-size: 11px; font-weight: 600; background: transparent;");

    brandTextCol->addWidget(brandTitle);
    brandTextCol->addWidget(brandSub);

    brandRow->addWidget(logoBadge);
    brandRow->addLayout(brandTextCol);
    brandRow->addStretch();

    sidebarLayout->addLayout(brandRow);

    // Subtle divider
    QFrame* divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Plain);
    divider->setStyleSheet("background-color: #F0F2F8; max-height: 1px; margin-bottom: 10px;");
    sidebarLayout->addWidget(divider);

    // Nav Items Helper
    auto createNavButton = [this, sidebarLayout](const QString& icon, const QString& text, int targetIndex) -> QPushButton* {
        QPushButton* btn = new QPushButton(QString("%1  %2").arg(icon, text));
        btn->setProperty("class", "nav-btn");
        btn->setCheckable(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton {"
            "  text-align: left;"
            "  padding: 11px 16px;"
            "  border: none;"
            "  border-radius: 10px;"
            "  background-color: transparent;"
            "  color: #7A849B;"
            "  font-size: 13px;"
            "  font-weight: 600;"
            "  margin: 2px 4px;"
            "}"
            "QPushButton:hover {"
            "  background-color: #F5F6FC;"
            "  color: #4169E1;"
            "}"
            "QPushButton:checked {"
            "  background-color: #EEF2FF;"
            "  color: #4169E1;"
            "  font-weight: 700;"
            "  border-left: 3px solid #4169E1;"
            "}"
        );

        connect(btn, &QPushButton::clicked, this, [this, targetIndex]() {
            navigateToPage(targetIndex);
        });

        sidebarLayout->addWidget(btn);
        m_navButtons.push_back(btn);
        return btn;
    };

    // 7 Core Navigation Items
    createNavButton("📊", "Dashboard", 0);
    createNavButton("👤", "My Profile", 1);
    createNavButton("🎯", "My Skills", 2);
    createNavButton("🔍", "Career Analysis", 3);
    createNavButton("🗺️", "Learning Roadmap", 5);
    createNavButton("📈", "Progress Tracking", 6);
    createNavButton("⚡", "Academic Evaluation", 7);

    sidebarLayout->addStretch();

    // Footer info card in sidebar
    QFrame* techCard = new QFrame();
    techCard->setStyleSheet(
        "QFrame {"
        "  background-color: #F8FAFC;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 10px;"
        "  padding: 10px 12px;"
        "}"
    );
    QVBoxLayout* techLayout = new QVBoxLayout(techCard);
    techLayout->setContentsMargins(0, 0, 0, 0);
    techLayout->setSpacing(2);

    QLabel* techTitle = new QLabel("SkillScope v1.0 • Academic");
    techTitle->setStyleSheet("color: #202B45; font-size: 11px; font-weight: 700; background: transparent;");

    QLabel* techSub = new QLabel("C++17 • Qt6 Widgets • SQLite");
    techSub->setStyleSheet("color: #7A849B; font-size: 10px; font-weight: 500; background: transparent;");

    techLayout->addWidget(techTitle);
    techLayout->addWidget(techSub);

    sidebarLayout->addWidget(techCard);

    rootLayout->addWidget(m_sidebar);

    // ==========================================
    // 2. Main Right Area (Header + Stacked Pages)
    // ==========================================
    QWidget* rightArea = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightArea);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    // Top Header Bar
    QFrame* headerBar = new QFrame();
    headerBar->setObjectName("headerBar");
    headerBar->setStyleSheet(
        "QFrame#headerBar {"
        "  background-color: #FFFFFF;"
        "  border-bottom: 1px solid #E8EAF2;"
        "  padding: 12px 28px;"
        "}"
    );
    QHBoxLayout* headerLayout = new QHBoxLayout(headerBar);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    // Page titles (Left)
    QVBoxLayout* titlesLayout = new QVBoxLayout();
    titlesLayout->setSpacing(2);

    m_headerTitle = new QLabel("Dashboard Overview");
    m_headerTitle->setStyleSheet("font-size: 18px; font-weight: 800; color: #202B45; background: transparent;");

    m_headerSubtitle = new QLabel("Welcome to SkillScope");
    m_headerSubtitle->setStyleSheet("font-size: 12px; color: #7A849B; font-weight: 500; background: transparent;");

    titlesLayout->addWidget(m_headerTitle);
    titlesLayout->addWidget(m_headerSubtitle);
    headerLayout->addLayout(titlesLayout);
    headerLayout->addStretch();

    // User Profile Widget in Header (Right)
    QHBoxLayout* userBox = new QHBoxLayout();
    userBox->setSpacing(10);

    m_userAvatar = new StudentAvatar("Alex Morgan", 38);
    userBox->addWidget(m_userAvatar);

    QVBoxLayout* userTextCol = new QVBoxLayout();
    userTextCol->setSpacing(1);

    m_userNameLabel = new QLabel("Alex Morgan");
    m_userNameLabel->setStyleSheet("font-size: 13px; font-weight: 700; color: #202B45; background: transparent;");

    m_userDeptLabel = new QLabel("Computer Science (Student)");
    m_userDeptLabel->setStyleSheet("font-size: 11px; color: #7A849B; font-weight: 500; background: transparent;");

    userTextCol->addWidget(m_userNameLabel);
    userTextCol->addWidget(m_userDeptLabel);
    userBox->addLayout(userTextCol);

    headerLayout->addLayout(userBox);
    headerLayout->addSpacing(16);

    // Logout Button
    m_logoutBtn = new QPushButton("Logout");
    m_logoutBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #F8FAFC;"
        "  color: #7A849B;"
        "  font-weight: 600;"
        "  font-size: 12px;"
        "  padding: 7px 16px;"
        "  border-radius: 8px;"
        "  border: 1px solid #E8EAF2;"
        "}"
        "QPushButton:hover {"
        "  background-color: #FDF0F1;"
        "  color: #E56B75;"
        "  border-color: #F8C8CC;"
        "}"
    );
    m_logoutBtn->setCursor(Qt::PointingHandCursor);
    connect(m_logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogout);
    headerLayout->addWidget(m_logoutBtn);

    rightLayout->addWidget(headerBar);

    // Stacked Widget for Pages (on light #F5F6FC canvas)
    m_stackedWidget = new QStackedWidget();
    m_stackedWidget->setStyleSheet("background-color: #F5F6FC;");

    // Initialize all 8 pages
    m_dashboardPage = new DashboardPage();
    m_profilePage = new ProfilePage();
    m_skillManagementPage = new SkillManagementPage();
    m_careerSelectionPage = new CareerSelectionPage();
    m_skillAnalysisPage = new SkillAnalysisPage();
    m_roadmapPage = new RoadmapPage();
    m_progressPage = new ProgressPage();
    m_evaluationPage = new EvaluationPage();

    // Add in order
    m_stackedWidget->addWidget(m_dashboardPage);        // 0: Dashboard
    m_stackedWidget->addWidget(m_profilePage);          // 1: Profile
    m_stackedWidget->addWidget(m_skillManagementPage);  // 2: My Skills
    m_stackedWidget->addWidget(m_careerSelectionPage);  // 3: Career Selection
    m_stackedWidget->addWidget(m_skillAnalysisPage);    // 4: Skill Analysis
    m_stackedWidget->addWidget(m_roadmapPage);          // 5: Roadmap
    m_stackedWidget->addWidget(m_progressPage);         // 6: Progress
    m_stackedWidget->addWidget(m_evaluationPage);       // 7: Evaluation

    // Wire inter-page navigation signals
    connect(m_dashboardPage, &DashboardPage::navigateToPage, this, &MainWindow::navigateToPage);
    connect(m_careerSelectionPage, &CareerSelectionPage::roleSelectedForAnalysis, this, &MainWindow::onRoleSelected);
    connect(m_skillAnalysisPage, &SkillAnalysisPage::generateRoadmapRequested, this, &MainWindow::onRoadmapRequested);

    rightLayout->addWidget(m_stackedWidget);
    rootLayout->addWidget(rightArea);

    // Default to Dashboard
    navigateToPage(0);
}

void MainWindow::updateHeader(int pageIndex) {
    int activeNavIndex = -1;
    switch (pageIndex) {
        case 0: activeNavIndex = 0; break;
        case 1: activeNavIndex = 1; break;
        case 2: activeNavIndex = 2; break;
        case 3:
        case 4: activeNavIndex = 3; break;
        case 5: activeNavIndex = 4; break;
        case 6: activeNavIndex = 5; break;
        case 7: activeNavIndex = 6; break;
    }

    for (size_t i = 0; i < m_navButtons.size(); ++i) {
        m_navButtons[i]->setChecked(static_cast<int>(i) == activeNavIndex);
    }

    switch (pageIndex) {
        case 0:
            m_headerTitle->setText("Student Dashboard");
            m_headerSubtitle->setText("Overview of career readiness, tracked competencies, and active roadmap milestones.");
            break;
        case 1:
            m_headerTitle->setText("Student Academic Profile");
            m_headerSubtitle->setText("View and maintain your personal and institutional details.");
            break;
        case 2:
            m_headerTitle->setText("Technical Skill Inventory");
            m_headerSubtitle->setText("Self-assess your proficiency across programming languages, tools, and computer science foundations.");
            break;
        case 3:
            m_headerTitle->setText("Career Track Selection");
            m_headerSubtitle->setText("Explore industry roles, inspect required competency benchmarks, or match a job description.");
            break;
        case 4:
            m_headerTitle->setText("Career Skill Gap Analysis");
            m_headerSubtitle->setText("Comparative gap breakdown identifying missing, insufficient, and mastered skills.");
            break;
        case 5:
            m_headerTitle->setText("STL-Prioritized Learning Roadmap");
            m_headerSubtitle->setText("Sequenced learning tasks powered by std::priority_queue and curated polymorphic resources.");
            break;
        case 6:
            m_headerTitle->setText("Progress Tracking & Analytics");
            m_headerSubtitle->setText("Interactive task status controls, milestone tracking, and task distribution charts.");
            break;
        case 7:
            m_headerTitle->setText("Academic Evaluation Benchmark");
            m_headerSubtitle->setText("Empirical comparison of Procedural vs Object-Oriented implementations with algorithmic analysis.");
            break;
    }
}

void MainWindow::navigateToPage(int pageIndex) {
    if (pageIndex < 0 || pageIndex >= m_stackedWidget->count()) return;

    m_stackedWidget->setCurrentIndex(pageIndex);
    updateHeader(pageIndex);

    // Refresh page data on navigation
    switch (pageIndex) {
        case 0: m_dashboardPage->refreshData(); break;
        case 1: m_profilePage->refreshData(); break;
        case 2: m_skillManagementPage->refreshData(); break;
        case 3: m_careerSelectionPage->refreshData(); break;
        case 4: m_skillAnalysisPage->refreshData(); break;
        case 5: m_roadmapPage->refreshData(); break;
        case 6: m_progressPage->refreshData(); break;
        case 7: m_evaluationPage->refreshData(); break;
    }
}

void MainWindow::onRoleSelected(int roleId) {
    m_skillAnalysisPage->setTargetRole(roleId);
    navigateToPage(4); // Switch to SkillAnalysisPage
}

void MainWindow::onRoadmapRequested(const std::vector<SkillGap>& gaps) {
    m_roadmapPage->setRoadmapGaps(gaps);
    navigateToPage(5); // Switch to RoadmapPage
}

void MainWindow::updateUserData() {
    Student* student = AuthService::instance().getCurrentStudent();
    if (student) {
        QString name = QString::fromStdString(student->getName());
        m_userAvatar->setName(name);
        m_userNameLabel->setText(name);
        m_userDeptLabel->setText(QString("%1 (%2)")
            .arg(QString::fromStdString(student->getDepartment()))
            .arg(QString::fromStdString(student->userType())));
    }
    navigateToPage(0); // Refresh dashboard
}

void MainWindow::onLogout() {
    AuthService::instance().logout();
    this->hide();
    if (m_loginWindow) {
        m_loginWindow->show();
    }
}
