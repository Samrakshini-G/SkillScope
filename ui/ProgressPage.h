#ifndef PROGRESSPAGE_H
#define PROGRESSPAGE_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QChart>
#include <QChartView>
#include <QPieSeries>

#include "components/MetricCard.h"

class ProgressPage : public QWidget {
    Q_OBJECT

public:
    explicit ProgressPage(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onFilterChanged(int index);
    void onUpdateTaskProgress(int taskId, int progress, int skillId, const QString& title);
    void onDeleteTask(int taskId);

private:
    void setupUi();
    void updateChart(int completed, int inProgress, int notStarted);

    // 5 Summary Metric Cards
    MetricCard* m_cardTotal;
    MetricCard* m_cardCompleted;
    MetricCard* m_cardInProgress;
    MetricCard* m_cardPending;
    MetricCard* m_cardAvgProgress;

    // Qt Chart
    QChartView* m_chartView;
    QChart* m_chart;
    QPieSeries* m_pieSeries;

    // Task Table & Filter
    QComboBox* m_statusFilterCombo;
    QTableWidget* m_tasksTable;

    // Skill Progress Table
    QTableWidget* m_skillProgressTable;

    QLabel* m_statusLabel;
};

#endif // PROGRESSPAGE_H
