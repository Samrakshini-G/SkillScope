#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>

#include "components/MetricCard.h"
#include "components/CircularProgress.h"
#include "components/StudentAvatar.h"
#include "components/EmptyStateWidget.h"

class DashboardPage : public QWidget {
    Q_OBJECT

public:
    explicit DashboardPage(QWidget *parent = nullptr);
    void refreshData();

signals:
    void navigateToPage(int pageIndex);

private:
    void setupUi();
    void updateCharts(int l1, int l2, int l3, int completed, int inProg, int notStarted);
    void updateNextAction(int targetRoleId, double readiness, int activeTasks, int studentId);

    // Welcome Section
    QLabel* m_welcomeNameLabel;
    QLabel* m_studentMetaLabel;
    QLabel* m_targetGoalBadge;
    StudentAvatar* m_avatar;

    // 4 Summary Metric Cards
    MetricCard* m_cardSkills;
    MetricCard* m_cardTarget;
    MetricCard* m_cardReadiness;
    MetricCard* m_cardTasks;

    // Visual Circular Indicator
    CircularProgress* m_circularReadiness;
    QLabel* m_readinessStatusLabel;
    QLabel* m_gapsCountLabel;

    // Recommended Next Action Card
    QFrame* m_actionCard;
    QLabel* m_actionIconLabel;
    QLabel* m_actionTitleLabel;
    QLabel* m_actionDescLabel;
    QPushButton* m_actionBtn;
    int m_actionTargetPage;

    // Qt Charts
    QChart* m_proficiencyChart;
    QChartView* m_proficiencyChartView;
    QBarSeries* m_proficiencySeries;
    QBarCategoryAxis* m_proficiencyAxisX;
    QValueAxis* m_proficiencyAxisY;

    QChart* m_taskChart;
    QChartView* m_taskChartView;
    QPieSeries* m_taskPieSeries;

    // Empty State Widget
    EmptyStateWidget* m_emptyState;
    QWidget* m_analyticsContainer;
};

#endif // DASHBOARDPAGE_H
