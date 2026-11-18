#ifndef ROADMAPPAGE_H
#define ROADMAPPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <vector>

#include "../models/LearningTask.h"
#include "../models/SkillGap.h"
#include "components/MetricCard.h"

class RoadmapPage : public QWidget {
    Q_OBJECT

public:
    explicit RoadmapPage(QWidget *parent = nullptr);
    void refreshData();
    void setRoadmapGaps(const std::vector<SkillGap>& gaps);

private slots:
    void onGenerateRoadmap();
    void onRefreshRoadmap();
    void onSaveRoadmap();
    void onClearRoadmap();
    void onFilterResources();
    void onViewResource(const QString& url);
    void onMarkTaskInProgress(int taskIndex);
    void onMarkTaskCompleted(int taskIndex);

private:
    void setupUi();
    void buildRoadmap(const std::vector<SkillGap>& gaps, const QString& roleTitle = "");
    void renderMilestoneCards();
    void renderResourceCards();

    QLabel* m_roleTitleLabel;
    QLabel* m_readinessLabel;

    // Summary Metric Badges
    MetricCard* m_cardTotal;
    MetricCard* m_cardNotStarted;
    MetricCard* m_cardInProgress;
    MetricCard* m_cardCompleted;
    MetricCard* m_cardDuration;

    // Milestone container
    QVBoxLayout* m_milestonesLayout;
    QWidget* m_milestonesContainer;

    // Resource filters & container
    QComboBox* m_resourceSkillFilter;
    QComboBox* m_resourceTypeFilter;
    QVBoxLayout* m_resourcesLayout;
    QWidget* m_resourcesContainer;

    QLabel* m_priorityQueueExplanation;
    QLabel* m_statusLabel;

    QPushButton* m_generateBtn;
    QPushButton* m_refreshBtn;
    QPushButton* m_saveBtn;
    QPushButton* m_clearBtn;

    std::vector<LearningTask> m_currentTasks;
    std::vector<SkillGap> m_currentGaps;
};

#endif // ROADMAPPAGE_H
