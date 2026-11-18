#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <vector>

#include "DashboardPage.h"
#include "ProfilePage.h"
#include "SkillManagementPage.h"
#include "CareerSelectionPage.h"
#include "SkillAnalysisPage.h"
#include "RoadmapPage.h"
#include "ProgressPage.h"
#include "EvaluationPage.h"
#include "components/StudentAvatar.h"

class LoginWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

    void updateUserData();
    void setLoginWindow(LoginWindow* loginWin);

public slots:
    void navigateToPage(int pageIndex);

private slots:
    void onLogout();
    void onRoleSelected(int roleId);
    void onRoadmapRequested(const std::vector<SkillGap>& gaps);

private:
    void setupUi();
    void updateHeader(int pageIndex);

    QWidget* m_sidebar;
    QLabel* m_headerTitle;
    QLabel* m_headerSubtitle;
    StudentAvatar* m_userAvatar;
    QLabel* m_userNameLabel;
    QLabel* m_userDeptLabel;
    QPushButton* m_logoutBtn;

    QStackedWidget* m_stackedWidget;
    std::vector<QPushButton*> m_navButtons;

    // Pages
    DashboardPage* m_dashboardPage;
    ProfilePage* m_profilePage;
    SkillManagementPage* m_skillManagementPage;
    CareerSelectionPage* m_careerSelectionPage;
    SkillAnalysisPage* m_skillAnalysisPage;
    RoadmapPage* m_roadmapPage;
    ProgressPage* m_progressPage;
    EvaluationPage* m_evaluationPage;

    LoginWindow* m_loginWindow;
};

#endif // MAINWINDOW_H
