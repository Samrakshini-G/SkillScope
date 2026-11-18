#include "SkillManagementPage.h"
#include "../services/AuthService.h"
#include "../database/SkillRepository.h"
#include "Theme.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <set>

SkillManagementPage::SkillManagementPage(QWidget *parent)
    : QWidget(parent),
      m_cardTotal(nullptr),
      m_cardBeg(nullptr),
      m_cardInt(nullptr),
      m_cardAdv(nullptr),
      m_searchEdit(nullptr),
      m_categoryFilter(nullptr),
      m_clearFiltersBtn(nullptr),
      m_mySkillsTable(nullptr),
      m_addSkillCombo(nullptr),
      m_newLevelCombo(nullptr),
      m_addSkillBtn(nullptr),
      m_statusLabel(nullptr) {
    setupUi();
}

void SkillManagementPage::setupUi() {
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
    // 1. Header Card
    // ==========================================
    QFrame* headerCard = new QFrame();
    headerCard->setObjectName("skillHeaderCard");
    headerCard->setStyleSheet(
        "#skillHeaderCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
    );
    Theme::applyCardShadow(headerCard, 14, 3, 16);

    QVBoxLayout* headerLayout = new QVBoxLayout(headerCard);
    headerLayout->setContentsMargins(24, 20, 24, 20);
    headerLayout->setSpacing(4);

    QLabel* title = new QLabel("Technical Skill Inventory & Proficiency Management");
    title->setStyleSheet("font-size: 20px; font-weight: 800; color: #202B45; border: none;");

    QLabel* subtitle = new QLabel(
        "Rate your current competency from Level 0 (Not learned) to Level 3 (Advanced). "
        "Self-assessed proficiencies directly feed the career readiness formula and STL priority roadmap engine."
    );
    subtitle->setStyleSheet("font-size: 13px; color: #7A849B; border: none;");

    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    mainLayout->addWidget(headerCard);

    // ==========================================
    // 2. Summary Metric Cards Row
    // ==========================================
    QGridLayout* summaryGrid = new QGridLayout();
    summaryGrid->setSpacing(18);

    m_cardTotal = new MetricCard("📋", "Total Registered", "0", "Skills in personal inventory", "#4169E1");
    m_cardBeg = new MetricCard("🌱", "Beginner (L1)", "0", "Basic understanding", "#4169E1");
    m_cardInt = new MetricCard("🔷", "Intermediate (L2)", "0", "Working competency", "#8064E8");
    m_cardAdv = new MetricCard("⭐", "Advanced (L3)", "0", "Mastery level", "#20B486");

    summaryGrid->addWidget(m_cardTotal, 0, 0);
    summaryGrid->addWidget(m_cardBeg, 0, 1);
    summaryGrid->addWidget(m_cardInt, 0, 2);
    summaryGrid->addWidget(m_cardAdv, 0, 3);
    mainLayout->addLayout(summaryGrid);

    // ==========================================
    // 3. Add Skill Section Card
    // ==========================================
    QFrame* addCard = new QFrame();
    addCard->setObjectName("skillAddCard");
    addCard->setStyleSheet(
        "#skillAddCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
    );
    Theme::applyCardShadow(addCard, 12, 3, 14);

    QVBoxLayout* addLayout = new QVBoxLayout(addCard);
    addLayout->setContentsMargins(20, 18, 20, 18);
    addLayout->setSpacing(12);

    QLabel* addTitle = new QLabel("➕ Add New Skill to My Personal Inventory");
    addTitle->setStyleSheet("font-size: 15px; font-weight: 700; color: #202B45; border: none;");
    addLayout->addWidget(addTitle);

    QHBoxLayout* addRow = new QHBoxLayout();
    addRow->setSpacing(14);

    QLabel* lblSkill = new QLabel("Select Skill:");
    lblSkill->setStyleSheet("font-weight: 700; font-size: 12px; color: #202B45; border: none;");

    m_addSkillCombo = new QComboBox();
    m_addSkillCombo->setMinimumWidth(260);

    QLabel* lblLevel = new QLabel("Initial Proficiency:");
    lblLevel->setStyleSheet("font-weight: 700; font-size: 12px; color: #202B45; border: none;");

    m_newLevelCombo = new QComboBox();
    m_newLevelCombo->addItem("Level 0 - Not learned", 0);
    m_newLevelCombo->addItem("Level 1 - Beginner", 1);
    m_newLevelCombo->addItem("Level 2 - Intermediate", 2);
    m_newLevelCombo->addItem("Level 3 - Advanced", 3);
    m_newLevelCombo->setCurrentIndex(1);
    m_newLevelCombo->setMinimumWidth(190);

    m_addSkillBtn = new QPushButton("Add to My Skills");
    m_addSkillBtn->setStyleSheet(Theme::successButtonStyle());
    m_addSkillBtn->setCursor(Qt::PointingHandCursor);
    connect(m_addSkillBtn, &QPushButton::clicked, this, &SkillManagementPage::onAddSkill);

    addRow->addWidget(lblSkill);
    addRow->addWidget(m_addSkillCombo, 2);
    addRow->addWidget(lblLevel);
    addRow->addWidget(m_newLevelCombo, 1);
    addRow->addWidget(m_addSkillBtn);
    addRow->addStretch();

    addLayout->addLayout(addRow);
    mainLayout->addWidget(addCard);

    // ==========================================
    // 4. Search and Filter Bar
    // ==========================================
    QFrame* filterCard = new QFrame();
    filterCard->setObjectName("skillFilterCard");
    filterCard->setStyleSheet(
        "#skillFilterCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 12px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
    );
    Theme::applyCardShadow(filterCard, 10, 2, 12);

    QHBoxLayout* filterLayout = new QHBoxLayout(filterCard);
    filterLayout->setContentsMargins(16, 12, 16, 12);
    filterLayout->setSpacing(14);

    QLabel* searchLabel = new QLabel("🔍 Search:");
    searchLabel->setStyleSheet("font-weight: 700; font-size: 12px; color: #202B45; border: none;");

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Filter by skill name or category...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &SkillManagementPage::onSearchChanged);

    QLabel* catLabel = new QLabel("Category:");
    catLabel->setStyleSheet("font-weight: 700; font-size: 12px; color: #202B45; border: none;");

    m_categoryFilter = new QComboBox();
    m_categoryFilter->addItem("All Categories");
    m_categoryFilter->setMinimumWidth(180);
    connect(m_categoryFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SkillManagementPage::onCategoryFilterChanged);

    m_clearFiltersBtn = new QPushButton("Clear Filters");
    m_clearFiltersBtn->setStyleSheet(Theme::secondaryButtonStyle());
    m_clearFiltersBtn->setCursor(Qt::PointingHandCursor);
    connect(m_clearFiltersBtn, &QPushButton::clicked, this, &SkillManagementPage::onClearFilters);

    filterLayout->addWidget(searchLabel);
    filterLayout->addWidget(m_searchEdit, 2);
    filterLayout->addWidget(catLabel);
    filterLayout->addWidget(m_categoryFilter, 1);
    filterLayout->addWidget(m_clearFiltersBtn);

    mainLayout->addWidget(filterCard);

    // ==========================================
    // 5. My Skills Table Card
    // ==========================================
    QFrame* tableCard = new QFrame();
    tableCard->setObjectName("skillTableCard");
    tableCard->setStyleSheet(
        "#skillTableCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
    );
    Theme::applyCardShadow(tableCard, 14, 3, 16);

    QVBoxLayout* tableLayout = new QVBoxLayout(tableCard);
    tableLayout->setContentsMargins(20, 18, 20, 18);
    tableLayout->setSpacing(14);

    QHBoxLayout* tblHeaderRow = new QHBoxLayout();
    QLabel* tableTitle = new QLabel("Registered Skills & Proficiency Levels");
    tableTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: #202B45; border: none;");
    tblHeaderRow->addWidget(tableTitle);
    tblHeaderRow->addStretch();

    m_statusLabel = new QLabel("");
    m_statusLabel->setStyleSheet("font-weight: 700; font-size: 12px; border: none;");
    tblHeaderRow->addWidget(m_statusLabel);
    tableLayout->addLayout(tblHeaderRow);

    m_mySkillsTable = new QTableWidget();
    m_mySkillsTable->setColumnCount(6);
    m_mySkillsTable->setHorizontalHeaderLabels({
        "Skill ID", "Skill Name", "Category", "Current Level & Indicator", "Update Proficiency", "Actions"
    });
    m_mySkillsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_mySkillsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_mySkillsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_mySkillsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_mySkillsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_mySkillsTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_mySkillsTable->verticalHeader()->setVisible(false);
    m_mySkillsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_mySkillsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_mySkillsTable->setAlternatingRowColors(true);
    m_mySkillsTable->setMinimumHeight(320);

    tableLayout->addWidget(m_mySkillsTable);
    mainLayout->addWidget(tableCard);

    mainLayout->addStretch();

    scrollArea->setWidget(contentWidget);
    outerLayout->addWidget(scrollArea);
}

void SkillManagementPage::updateSummaryMetrics() {
    int total = static_cast<int>(m_currentStudentLevels.size());
    int b = 0, i = 0, a = 0;
    for (const auto& [id, level] : m_currentStudentLevels) {
        if (level == 1) b++;
        else if (level == 2) i++;
        else if (level == 3) a++;
    }
    m_cardTotal->setValue(QString::number(total));
    m_cardBeg->setValue(QString::number(b));
    m_cardInt->setValue(QString::number(i));
    m_cardAdv->setValue(QString::number(a));
}

void SkillManagementPage::refreshData() {
    Student* student = AuthService::instance().getCurrentStudent();
    if (!student) return;

    SkillRepository repo;
    m_allCatalogSkills = repo.getAllSkills();
    m_currentStudentLevels = repo.getStudentSkillLevels(student->getId());

    updateSummaryMetrics();

    m_categoryFilter->blockSignals(true);
    QString currentCat = m_categoryFilter->currentText();
    m_categoryFilter->clear();
    m_categoryFilter->addItem("All Categories");

    std::set<std::string> cats;
    for (const auto& s : m_allCatalogSkills) {
        cats.insert(s.getCategory());
    }
    for (const auto& c : cats) {
        m_categoryFilter->addItem(QString::fromStdString(c));
    }
    int idx = m_categoryFilter->findText(currentCat);
    if (idx >= 0) m_categoryFilter->setCurrentIndex(idx);
    m_categoryFilter->blockSignals(false);

    populateAddSkillDropdown();
    populateMySkillsTable();
    m_statusLabel->clear();
}

void SkillManagementPage::populateAddSkillDropdown() {
    m_addSkillCombo->clear();

    for (const auto& skill : m_allCatalogSkills) {
        if (m_currentStudentLevels.find(skill.getId()) == m_currentStudentLevels.end()) {
            m_addSkillCombo->addItem(
                QString("%1 (%2)").arg(QString::fromStdString(skill.getName())).arg(QString::fromStdString(skill.getCategory())),
                skill.getId()
            );
        }
    }

    m_addSkillBtn->setEnabled(m_addSkillCombo->count() > 0);
    if (m_addSkillCombo->count() == 0) {
        m_addSkillCombo->addItem("All catalog skills already added to your profile");
    }
}

void SkillManagementPage::populateMySkillsTable() {
    m_mySkillsTable->setRowCount(0);

    QString search = m_searchEdit->text().trimmed().toLower();
    QString catFilter = m_categoryFilter->currentText();

    std::map<int, Skill> skillMap;
    for (const auto& s : m_allCatalogSkills) {
        skillMap[s.getId()] = s;
    }

    int row = 0;
    for (const auto& [skillId, level] : m_currentStudentLevels) {
        if (skillMap.find(skillId) == skillMap.end()) continue;

        const Skill& skill = skillMap[skillId];
        QString nameStr = QString::fromStdString(skill.getName());
        QString catStr = QString::fromStdString(skill.getCategory());

        if (!search.isEmpty() && !nameStr.toLower().contains(search) && !catStr.toLower().contains(search)) {
            continue;
        }

        if (catFilter != "All Categories" && catStr != catFilter) {
            continue;
        }

        m_mySkillsTable->insertRow(row);

        // Col 0: ID
        QTableWidgetItem* idItem = new QTableWidgetItem(QString::number(skill.getId()));
        idItem->setTextAlignment(Qt::AlignCenter);
        m_mySkillsTable->setItem(row, 0, idItem);

        // Col 1: Name
        QTableWidgetItem* nameItem = new QTableWidgetItem(nameStr);
        nameItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
        m_mySkillsTable->setItem(row, 1, nameItem);

        // Col 2: Category
        QTableWidgetItem* catItem = new QTableWidgetItem(catStr);
        catItem->setTextAlignment(Qt::AlignCenter);
        m_mySkillsTable->setItem(row, 2, catItem);

        // Col 3: Current Level & Indicator
        ProficiencyBadge* badge = new ProficiencyBadge(level, true);
        m_mySkillsTable->setCellWidget(row, 3, badge);

        // Col 4: New Level Selector
        QComboBox* levelCombo = new QComboBox();
        levelCombo->addItem("0 - Not learned", 0);
        levelCombo->addItem("1 - Beginner", 1);
        levelCombo->addItem("2 - Intermediate", 2);
        levelCombo->addItem("3 - Advanced", 3);
        levelCombo->setCurrentIndex(level);
        m_mySkillsTable->setCellWidget(row, 4, levelCombo);

        // Col 5: Actions
        QWidget* actWidget = new QWidget();
        QHBoxLayout* actLayout = new QHBoxLayout(actWidget);
        actLayout->setContentsMargins(4, 2, 4, 2);
        actLayout->setSpacing(6);

        QPushButton* updateBtn = new QPushButton("Save");
        updateBtn->setStyleSheet(
            "QPushButton {"
            "  background-color: #4169E1; color: #FFFFFF; font-weight: 700; padding: 4px 12px; border-radius: 6px; border: none; font-size: 11px;"
            "}"
            "QPushButton:hover { background-color: #3557C7; }"
        );
        updateBtn->setCursor(Qt::PointingHandCursor);
        connect(updateBtn, &QPushButton::clicked, this, [this, skillId, levelCombo]() {
            onUpdateSkillLevel(skillId, levelCombo->currentData().toInt());
        });

        QPushButton* removeBtn = new QPushButton("Remove");
        removeBtn->setStyleSheet(
            "QPushButton {"
            "  background-color: #FDF0F1; color: #E56B75; font-weight: 600; padding: 4px 10px; border-radius: 6px; border: 1px solid #F8C8CC; font-size: 11px;"
            "}"
            "QPushButton:hover { background-color: #E56B75; color: #FFFFFF; }"
        );
        removeBtn->setCursor(Qt::PointingHandCursor);
        connect(removeBtn, &QPushButton::clicked, this, [this, skillId]() {
            onRemoveSkill(skillId);
        });

        actLayout->addWidget(updateBtn);
        actLayout->addWidget(removeBtn);
        m_mySkillsTable->setCellWidget(row, 5, actWidget);

        row++;
    }
}

void SkillManagementPage::onSearchChanged(const QString&) {
    populateMySkillsTable();
}

void SkillManagementPage::onCategoryFilterChanged(int) {
    populateMySkillsTable();
}

void SkillManagementPage::onClearFilters() {
    m_searchEdit->clear();
    m_categoryFilter->setCurrentIndex(0);
    populateMySkillsTable();
}

void SkillManagementPage::onAddSkill() {
    Student* student = AuthService::instance().getCurrentStudent();
    if (!student) return;

    if (m_addSkillCombo->count() == 0 || m_addSkillCombo->currentData().isNull()) {
        m_statusLabel->setText("No valid skill selected to add.");
        m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700; border: none;");
        return;
    }

    int skillId = m_addSkillCombo->currentData().toInt();
    int level = m_newLevelCombo->currentData().toInt();

    SkillRepository repo;
    if (repo.addStudentSkill(student->getId(), skillId, level)) {
        m_statusLabel->setText("✓ Skill added to your profile!");
        m_statusLabel->setStyleSheet("color: #20B486; font-weight: 700; border: none;");
        refreshData();
    } else {
        m_statusLabel->setText("Skill already exists or database error occurred.");
        m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700; border: none;");
    }
}

void SkillManagementPage::onUpdateSkillLevel(int skillId, int newLevel) {
    Student* student = AuthService::instance().getCurrentStudent();
    if (!student) return;

    SkillRepository repo;
    if (repo.setStudentSkillLevel(student->getId(), skillId, newLevel)) {
        m_statusLabel->setText(QString("✓ Proficiency updated to Level %1!").arg(newLevel));
        m_statusLabel->setStyleSheet("color: #20B486; font-weight: 700; border: none;");
        m_currentStudentLevels[skillId] = newLevel;
        updateSummaryMetrics();
        populateMySkillsTable();
    } else {
        m_statusLabel->setText("Failed to update proficiency level.");
        m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700; border: none;");
    }
}

void SkillManagementPage::onRemoveSkill(int skillId) {
    Student* student = AuthService::instance().getCurrentStudent();
    if (!student) return;

    SkillRepository repo;
    if (repo.removeStudentSkill(student->getId(), skillId)) {
        m_statusLabel->setText("✓ Skill removed from your personal list.");
        m_statusLabel->setStyleSheet("color: #20B486; font-weight: 700; border: none;");
        refreshData();
    } else {
        m_statusLabel->setText("Failed to remove skill.");
        m_statusLabel->setStyleSheet("color: #E56B75; font-weight: 700; border: none;");
    }
}
