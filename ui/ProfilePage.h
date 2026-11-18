#ifndef PROFILEPAGE_H
#define PROFILEPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>

#include "components/StudentAvatar.h"

class ProfilePage : public QWidget {
    Q_OBJECT

public:
    explicit ProfilePage(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onSaveProfile();

private:
    void setupUi();

    // Summary Header widgets
    StudentAvatar* m_summaryAvatar;
    QLabel* m_summaryNameLabel;
    QLabel* m_summaryEmailLabel;
    QLabel* m_summaryMetaLabel;
    QLabel* m_summaryRoleBadge;
    QLabel* m_userTypeBadge;

    // Form Edit widgets
    QLineEdit* m_nameEdit;
    QLineEdit* m_emailEdit;
    QLineEdit* m_deptEdit;
    QLineEdit* m_instEdit;
    QLineEdit* m_degreeEdit;
    QSpinBox* m_gradYearSpin;

    QLabel* m_statusLabel;
};

#endif // PROFILEPAGE_H
