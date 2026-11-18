#ifndef SKILLMANAGEMENTPAGE_H
#define SKILLMANAGEMENTPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <vector>
#include <map>

#include "../models/Skill.h"
#include "components/MetricCard.h"
#include "components/ProficiencyBadge.h"

class SkillManagementPage : public QWidget {
    Q_OBJECT

public:
    explicit SkillManagementPage(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onSearchChanged(const QString& text);
    void onCategoryFilterChanged(int index);
    void onClearFilters();
    void onAddSkill();
    void onUpdateSkillLevel(int skillId, int newLevel);
    void onRemoveSkill(int skillId);

private:
    void setupUi();
    void populateAddSkillDropdown();
    void populateMySkillsTable();
    void updateSummaryMetrics();

    // Summary Metric Cards
    MetricCard* m_cardTotal;
    MetricCard* m_cardBeg;
    MetricCard* m_cardInt;
    MetricCard* m_cardAdv;

    // Filter controls
    QLineEdit* m_searchEdit;
    QComboBox* m_categoryFilter;
    QPushButton* m_clearFiltersBtn;

    // Table
    QTableWidget* m_mySkillsTable;

    // Add Skill controls
    QComboBox* m_addSkillCombo;
    QComboBox* m_newLevelCombo;
    QPushButton* m_addSkillBtn;

    QLabel* m_statusLabel;

    std::vector<Skill> m_allCatalogSkills;
    std::map<int, int> m_currentStudentLevels;
};

#endif // SKILLMANAGEMENTPAGE_H
