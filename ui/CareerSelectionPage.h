#ifndef CAREERSELECTIONPAGE_H
#define CAREERSELECTIONPAGE_H

#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <vector>
#include "../models/JobRole.h"

class CareerSelectionPage : public QWidget {
    Q_OBJECT

public:
    explicit CareerSelectionPage(QWidget *parent = nullptr);
    void refreshData();

signals:
    void roleSelectedForAnalysis(int roleId);

private slots:
    void onSearchChanged(const QString& query);
    void onSetTargetRole(int roleId);

private:
    void setupUi();
    void renderRoleCards(const std::vector<JobRole>& roles);
    QWidget* createRoleCard(const JobRole& role, bool isCurrentTarget);

    QLineEdit* m_searchEdit;
    QGridLayout* m_rolesGrid;
    QLabel* m_statusLabel;
    int m_currentTargetRoleId;
    std::vector<JobRole> m_cachedRoles;
};

#endif // CAREERSELECTIONPAGE_H
