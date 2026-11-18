#include "ProfilePage.h"
#include "../services/AuthService.h"
#include "../database/UserRepository.h"
#include "../database/CareerRepository.h"
#include "Theme.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollArea>

ProfilePage::ProfilePage(QWidget *parent)
    : QWidget(parent),
      m_summaryAvatar(nullptr),
      m_summaryNameLabel(nullptr),
      m_summaryEmailLabel(nullptr),
      m_summaryMetaLabel(nullptr),
      m_summaryRoleBadge(nullptr),
      m_userTypeBadge(nullptr),
      m_nameEdit(nullptr),
      m_emailEdit(nullptr),
      m_deptEdit(nullptr),
      m_instEdit(nullptr),
      m_degreeEdit(nullptr),
      m_gradYearSpin(nullptr),
      m_statusLabel(nullptr) {
    setupUi();
}

void ProfilePage::setupUi() {
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
    // 1. Profile Summary Hero Card
    // ==========================================
    QFrame* summaryCard = new QFrame();
    summaryCard->setObjectName("profileSummaryCard");
    summaryCard->setStyleSheet(
        "#profileSummaryCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 16px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
    );
    Theme::applyCardShadow(summaryCard, 14, 3, 16);

    QHBoxLayout* summaryLayout = new QHBoxLayout(summaryCard);
    summaryLayout->setSpacing(22);
    summaryLayout->setContentsMargins(24, 20, 24, 20);

    m_summaryAvatar = new StudentAvatar("Alex Morgan", 72);
    summaryLayout->addWidget(m_summaryAvatar);

    QVBoxLayout* summaryInfo = new QVBoxLayout();
    summaryInfo->setSpacing(5);

    QHBoxLayout* nameRow = new QHBoxLayout();
    nameRow->setSpacing(10);

    m_summaryNameLabel = new QLabel("Alex Morgan");
    m_summaryNameLabel->setStyleSheet("font-size: 22px; font-weight: 800; color: #202B45; border: none;");

    m_userTypeBadge = new QLabel("STUDENT");
    m_userTypeBadge->setStyleSheet(
        "background-color: #EEF2FF; color: #4169E1; font-weight: 700; font-size: 11px; padding: 4px 10px; border-radius: 10px; border: none;"
    );

    nameRow->addWidget(m_summaryNameLabel);
    nameRow->addWidget(m_userTypeBadge);
    nameRow->addStretch();
    summaryInfo->addLayout(nameRow);

    m_summaryEmailLabel = new QLabel("student@skillscope.edu");
    m_summaryEmailLabel->setStyleSheet("font-size: 13px; color: #7A849B; font-weight: 500; border: none;");
    summaryInfo->addWidget(m_summaryEmailLabel);

    m_summaryMetaLabel = new QLabel("Department of Computer Science • State University of Technology");
    m_summaryMetaLabel->setStyleSheet("font-size: 12px; color: #7A849B; border: none;");
    summaryInfo->addWidget(m_summaryMetaLabel);

    summaryLayout->addLayout(summaryInfo);
    summaryLayout->addStretch();

    // Active Career Goal on right
    QVBoxLayout* targetRoleCol = new QVBoxLayout();
    targetRoleCol->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    targetRoleCol->setSpacing(4);

    QLabel* targetTitle = new QLabel("ACTIVE TARGET CAREER");
    targetTitle->setStyleSheet("font-size: 10px; font-weight: 700; color: #7A849B; letter-spacing: 0.6px; border: none;");
    targetTitle->setAlignment(Qt::AlignRight);

    m_summaryRoleBadge = new QLabel("None selected");
    m_summaryRoleBadge->setStyleSheet(
        "background-color: #F3F0FD; color: #8064E8; font-weight: 700; font-size: 12px; padding: 6px 14px; border-radius: 12px; border: none;"
    );

    targetRoleCol->addWidget(targetTitle);
    targetRoleCol->addWidget(m_summaryRoleBadge);
    summaryLayout->addLayout(targetRoleCol);

    mainLayout->addWidget(summaryCard);

    // ==========================================
    // 2. Profile Details Form Card
    // ==========================================
    QFrame* formCard = new QFrame();
    formCard->setObjectName("profileFormCard");
    formCard->setStyleSheet(
        "#profileFormCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 16px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
    );
    Theme::applyCardShadow(formCard, 14, 3, 16);

    QVBoxLayout* cardLayout = new QVBoxLayout(formCard);
    cardLayout->setContentsMargins(24, 22, 24, 22);
    cardLayout->setSpacing(18);

    QLabel* formTitle = new QLabel("Edit Academic & Personal Credentials");
    formTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #202B45; border: none;");
    cardLayout->addWidget(formTitle);

    QGridLayout* formGrid = new QGridLayout();
    formGrid->setHorizontalSpacing(24);
    formGrid->setVerticalSpacing(14);

    auto makeFieldLabel = [](const QString& text, bool required = false) -> QLabel* {
        QLabel* l = new QLabel(required ? text + " *" : text);
        l->setStyleSheet("font-size: 12px; font-weight: 700; color: #202B45; border: none;");
        return l;
    };

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("Enter full name");
    m_nameEdit->setMinimumHeight(38);

    m_emailEdit = new QLineEdit();
    m_emailEdit->setPlaceholderText("student@institution.edu");
    m_emailEdit->setMinimumHeight(38);

    m_instEdit = new QLineEdit();
    m_instEdit->setPlaceholderText("e.g. State University of Technology");
    m_instEdit->setMinimumHeight(38);

    m_deptEdit = new QLineEdit();
    m_deptEdit->setPlaceholderText("e.g. Computer Science and Engineering");
    m_deptEdit->setMinimumHeight(38);

    m_degreeEdit = new QLineEdit();
    m_degreeEdit->setPlaceholderText("e.g. Bachelor of Science in Computer Science");
    m_degreeEdit->setMinimumHeight(38);

    m_gradYearSpin = new QSpinBox();
    m_gradYearSpin->setRange(2020, 2040);
    m_gradYearSpin->setValue(2026);
    m_gradYearSpin->setFixedWidth(160);
    m_gradYearSpin->setMinimumHeight(38);

    // Row 0: Full Name & Email
    formGrid->addWidget(makeFieldLabel("Full Name", true), 0, 0);
    formGrid->addWidget(m_nameEdit, 1, 0);

    formGrid->addWidget(makeFieldLabel("Email Address", true), 0, 1);
    formGrid->addWidget(m_emailEdit, 1, 1);

    // Row 1: University & Department
    formGrid->addWidget(makeFieldLabel("University / Institution", true), 2, 0);
    formGrid->addWidget(m_instEdit, 3, 0);

    formGrid->addWidget(makeFieldLabel("Department / Faculty", true), 2, 1);
    formGrid->addWidget(m_deptEdit, 3, 1);

    // Row 2: Degree Program & Graduation Year
    formGrid->addWidget(makeFieldLabel("Degree Programme"), 4, 0);
    formGrid->addWidget(m_degreeEdit, 5, 0);

    formGrid->addWidget(makeFieldLabel("Expected Graduation Year"), 4, 1);
    formGrid->addWidget(m_gradYearSpin, 5, 1);

    cardLayout->addLayout(formGrid);

    // Bottom Action Row
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(14);

    QPushButton* saveBtn = new QPushButton("💾  Save Changes");
    saveBtn->setStyleSheet(Theme::primaryButtonStyle());
    saveBtn->setCursor(Qt::PointingHandCursor);
    connect(saveBtn, &QPushButton::clicked, this, &ProfilePage::onSaveProfile);

    m_statusLabel = new QLabel("");
    m_statusLabel->setStyleSheet("font-weight: 700; font-size: 13px; border: none;");

    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(m_statusLabel);
    btnLayout->addStretch();

    cardLayout->addSpacing(8);
    cardLayout->addLayout(btnLayout);

    mainLayout->addWidget(formCard);
    mainLayout->addStretch();

    scrollArea->setWidget(contentWidget);
    outerLayout->addWidget(scrollArea);
}

void ProfilePage::refreshData() {
    Student* student = AuthService::instance().getCurrentStudent();
    if (student) {
        UserRepository userRepo;
        auto freshStudent = userRepo.getStudentById(student->getId());
        if (freshStudent) {
            student->setName(freshStudent->getName());
            student->setEmail(freshStudent->getEmail());
            student->setDepartment(freshStudent->getDepartment());
            student->setInstitution(freshStudent->getInstitution());
            student->setDegree(freshStudent->getDegree());
            student->setGraduationYear(freshStudent->getGraduationYear());
            student->setTargetRoleId(freshStudent->getTargetRoleId());
        }

        QString nameStr = QString::fromStdString(student->getName());
        QString emailStr = QString::fromStdString(student->getEmail());
        QString deptStr = QString::fromStdString(student->getDepartment());
        QString instStr = QString::fromStdString(student->getInstitution());

        m_summaryAvatar->setName(nameStr);
        m_summaryNameLabel->setText(nameStr);
        m_summaryEmailLabel->setText(emailStr);
        m_summaryMetaLabel->setText(QString("%1 • %2").arg(deptStr, instStr));
        m_userTypeBadge->setText(QString::fromStdString(student->userType()).toUpper());

        m_nameEdit->setText(nameStr);
        m_emailEdit->setText(emailStr);
        m_instEdit->setText(instStr);
        m_deptEdit->setText(deptStr);
        m_degreeEdit->setText(QString::fromStdString(student->getDegree()));
        m_gradYearSpin->setValue(student->getGraduationYear() > 0 ? student->getGraduationYear() : 2026);

        if (student->getTargetRoleId() > 0) {
            CareerRepository careerRepo;
            auto role = careerRepo.getJobRoleById(student->getTargetRoleId());
            if (role.has_value()) {
                m_summaryRoleBadge->setText(QString::fromStdString(role->getTitle()));
            } else {
                m_summaryRoleBadge->setText("None selected");
            }
        } else {
            m_summaryRoleBadge->setText("None selected");
        }

        m_statusLabel->clear();
    }
}

void ProfilePage::onSaveProfile() {
    Student* student = AuthService::instance().getCurrentStudent();
    if (!student) return;

    QString name = m_nameEdit->text().trimmed();
    QString email = m_emailEdit->text().trimmed();
    QString inst = m_instEdit->text().trimmed();
    QString dept = m_deptEdit->text().trimmed();
    QString degree = m_degreeEdit->text().trimmed();
    int gradYear = m_gradYearSpin->value();

    if (name.isEmpty() || email.isEmpty() || inst.isEmpty() || dept.isEmpty()) {
        m_statusLabel->setText("Error: Full Name, Email, University, and Department are required.");
        m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700; border: none;");
        return;
    }

    static const QRegularExpression emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!emailRegex.match(email).hasMatch()) {
        m_statusLabel->setText("Error: Please enter a valid email address (e.g. name@domain.edu).");
        m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700; border: none;");
        return;
    }

    UserRepository repo;
    if (repo.emailExistsForOtherUser(email.toStdString(), student->getId())) {
        m_statusLabel->setText("Error: This email address is already registered to another account.");
        m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700; border: none;");
        return;
    }

    if (repo.updateStudentProfile(student->getId(), name.toStdString(), email.toStdString(),
                                  dept.toStdString(), inst.toStdString(), degree.toStdString(), gradYear)) {
        student->setName(name.toStdString());
        student->setEmail(email.toStdString());
        student->setDepartment(dept.toStdString());
        student->setInstitution(inst.toStdString());
        student->setDegree(degree.toStdString());
        student->setGraduationYear(gradYear);

        m_summaryAvatar->setName(name);
        m_summaryNameLabel->setText(name);
        m_summaryEmailLabel->setText(email);
        m_summaryMetaLabel->setText(QString("%1 • %2").arg(dept, inst));

        m_statusLabel->setText("✓ Profile updated successfully in database!");
        m_statusLabel->setStyleSheet("color: #20B486; font-weight: 700; border: none;");
    } else {
        m_statusLabel->setText("Failed to save profile changes to database.");
        m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700; border: none;");
    }
}
