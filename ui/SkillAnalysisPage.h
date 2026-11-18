#ifndef SKILLANALYSISPAGE_H
#define SKILLANALYSISPAGE_H

#include <QWidget>
#include <QComboBox>
#include <QTextEdit>
#include <QTableWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <vector>

#include "../models/SkillGap.h"
#include "../services/SkillAnalysisService.h"
#include "components/CircularProgress.h"
#include "components/MetricCard.h"

class SkillAnalysisPage : public QWidget {
    Q_OBJECT

public:
    explicit SkillAnalysisPage(QWidget *parent = nullptr);
    void refreshData();
    void setTargetRole(int roleId);

signals:
    void generateRoadmapRequested(const std::vector<SkillGap>& gaps);

private slots:
    void onAnalyzeRole();
    void onAnalyzeJobDescription();
    void onLoadSampleJobDescription();
    void onGenerateRoadmapClicked();

private:
    void setupUi();
    void displayAnalysis(const AnalysisResult& result, bool isFromJobDescription);

    // Track A: Role Selection
    QComboBox* m_roleCombo;
    QPushButton* m_analyzeRoleBtn;

    // Track B: Job Description Parser
    QTextEdit* m_jobDescText;
    QPushButton* m_analyzeJdBtn;
    QPushButton* m_sampleJdBtn;
    QLabel* m_jdNoticeLabel;

    // Career Readiness Header & Visual Indicator
    QLabel* m_targetCareerTitleLabel;
    QLabel* m_readinessPercentLabel;
    CircularProgress* m_readinessCircular;
    QProgressBar* m_readinessProgressBar;

    // 4 Summary Metrics
    MetricCard* m_cardTotalReq;
    MetricCard* m_cardMeets;
    MetricCard* m_cardInsuff;
    MetricCard* m_cardMissing;

    // Detailed Tables
    QTableWidget* m_comparisonTable;
    QTableWidget* m_missingTable;
    QTableWidget* m_insufficientTable;
    QTableWidget* m_masteredTable;

    QLabel* m_statusLabel;
    AnalysisResult m_currentResult;
};

#endif // SKILLANALYSISPAGE_H
