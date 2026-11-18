#include "ProgressPage.h"
#include "../database/LearningRepository.h"
#include "../database/SkillRepository.h"
#include "../services/AuthService.h"
#include "../services/ProgressService.h"
#include "Theme.h"

#include <QDate>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPieSlice>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

ProgressPage::ProgressPage(QWidget *parent)
    : QWidget(parent), m_cardTotal(nullptr), m_cardCompleted(nullptr),
      m_cardInProgress(nullptr), m_cardPending(nullptr),
      m_cardAvgProgress(nullptr), m_chartView(nullptr), m_chart(nullptr),
      m_pieSeries(nullptr), m_statusFilterCombo(nullptr), m_tasksTable(nullptr),
      m_skillProgressTable(nullptr), m_statusLabel(nullptr) {
  setupUi();
}

void ProgressPage::setupUi() {
  QVBoxLayout *outerLayout = new QVBoxLayout(this);
  outerLayout->setContentsMargins(0, 0, 0, 0);

  QScrollArea *scrollArea = new QScrollArea();
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);

  QWidget *contentWidget = new QWidget();
  QVBoxLayout *mainLayout = new QVBoxLayout(contentWidget);
  mainLayout->setContentsMargins(32, 24, 32, 28);
  mainLayout->setSpacing(22);

  // ==========================================
  // 1. Header Card
  // ==========================================
  QFrame *headerCard = new QFrame();
  headerCard->setStyleSheet("QFrame {"
                            "  background-color: #FFFFFF;"
                            "  border: 1px solid #E8EAF2;"
                            "  border-radius: 14px;"
                            "  padding: 20px 24px;"
                            "}");
  Theme::applyCardShadow(headerCard, 14, 3, 16);

  QVBoxLayout *headerLayout = new QVBoxLayout(headerCard);
  headerLayout->setSpacing(4);

  QLabel *title = new QLabel("Progress Tracking & Milestone Analytics");
  title->setStyleSheet("font-size: 20px; font-weight: 800; color: #202B45; "
                       "background: transparent;");

  QLabel *subtitle = new QLabel(
      "Monitor your active learning task completion rates, milestone history, "
      "and individual skill competency trajectories.");
  subtitle->setStyleSheet(
      "font-size: 13px; color: #7A849B; background: transparent;");

  headerLayout->addWidget(title);
  headerLayout->addWidget(subtitle);
  mainLayout->addWidget(headerCard);

  // ==========================================
  // 2. Summary Metric Cards Row (5 Cards)
  // ==========================================
  QGridLayout *metricsGrid = new QGridLayout();
  metricsGrid->setSpacing(16);

  m_cardTotal =
      new MetricCard("📋", "Total Tasks", "0", "Scheduled tasks", "#4169E1");
  m_cardCompleted =
      new MetricCard("✅", "Completed", "0", "Finished milestones", "#20B486");
  m_cardInProgress =
      new MetricCard("⚙️", "In Progress", "0", "Active learning", "#F3A640");
  m_cardPending =
      new MetricCard("⏳", "Pending", "0", "Not started yet", "#7A849B");
  m_cardAvgProgress = new MetricCard("📈", "Average Progress", "0.0%",
                                     "Across all tasks", "#8064E8");

  metricsGrid->addWidget(m_cardTotal, 0, 0);
  metricsGrid->addWidget(m_cardCompleted, 0, 1);
  metricsGrid->addWidget(m_cardInProgress, 0, 2);
  metricsGrid->addWidget(m_cardPending, 0, 3);
  metricsGrid->addWidget(m_cardAvgProgress, 0, 4);

  mainLayout->addLayout(metricsGrid);

  // ==========================================
  // 3. Middle Row: Chart & Task Distribution
  // ==========================================
  QFrame *chartCard = new QFrame();
  chartCard->setStyleSheet("QFrame {"
                           "  background-color: #FFFFFF;"
                           "  border: 1px solid #E8EAF2;"
                           "  border-radius: 14px;"
                           "  padding: 18px 24px;"
                           "}");
  Theme::applyCardShadow(chartCard, 14, 3, 16);

  QHBoxLayout *chartLayout = new QHBoxLayout(chartCard);
  chartLayout->setSpacing(24);

  QVBoxLayout *chartInfoCol = new QVBoxLayout();
  chartInfoCol->setSpacing(8);

  QLabel *chartTitle = new QLabel("Task Completion Distribution");
  chartTitle->setStyleSheet("font-size: 16px; font-weight: 800; color: "
                            "#202B45; background: transparent;");

  QLabel *chartDesc =
      new QLabel("Visual breakdown of learning tasks by execution status. "
                 "Complete tasks unlock proficiency advancements upon explicit "
                 "confirmation.");
  chartDesc->setStyleSheet("font-size: 12px; color: #7A849B; line-height: 1.4; "
                           "background: transparent;");
  chartDesc->setWordWrap(true);

  chartInfoCol->addWidget(chartTitle);
  chartInfoCol->addWidget(chartDesc);
  chartInfoCol->addStretch();
  chartLayout->addLayout(chartInfoCol, 1);

  m_chart = new QChart();
  m_chart->setAnimationOptions(QChart::SeriesAnimations);
  m_chart->setBackgroundVisible(false);
  m_chart->legend()->setAlignment(Qt::AlignRight);
  m_chart->legend()->setFont(QFont("Segoe UI", 10));

  m_pieSeries = new QPieSeries();
  m_pieSeries->setHoleSize(0.5); // Modern Donut look
  m_chart->addSeries(m_pieSeries);

  m_chartView = new QChartView(m_chart);
  m_chartView->setRenderHint(QPainter::Antialiasing);
  m_chartView->setMinimumHeight(200);
  m_chartView->setMinimumWidth(320);
  chartLayout->addWidget(m_chartView, 1);

  mainLayout->addWidget(chartCard);

  // ==========================================
  // 4. Task Management Table Card with Filter
  // ==========================================
  QFrame *tableCard = new QFrame();
  tableCard->setStyleSheet("QFrame {"
                           "  background-color: #FFFFFF;"
                           "  border: 1px solid #E8EAF2;"
                           "  border-radius: 14px;"
                           "  padding: 20px;"
                           "}");
  Theme::applyCardShadow(tableCard, 14, 3, 16);

  QVBoxLayout *tableLayout = new QVBoxLayout(tableCard);
  tableLayout->setSpacing(14);

  QHBoxLayout *tblHeader = new QHBoxLayout();
  QLabel *tableTitle = new QLabel("Assigned Learning Tasks & Milestones");
  tableTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: "
                            "#202B45; background: transparent;");
  tblHeader->addWidget(tableTitle);
  tblHeader->addStretch();

  QLabel *fltLbl = new QLabel("Filter Status:");
  fltLbl->setStyleSheet("font-weight: 700; font-size: 12px; color: #202B45; "
                        "background: transparent;");
  m_statusFilterCombo = new QComboBox();
  m_statusFilterCombo->addItems(
      {"All Tasks", "Not Started", "In Progress", "Completed"});
  m_statusFilterCombo->setMinimumWidth(150);
  connect(m_statusFilterCombo,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &ProgressPage::onFilterChanged);

  tblHeader->addWidget(fltLbl);
  tblHeader->addWidget(m_statusFilterCombo);
  tableLayout->addLayout(tblHeader);

  m_tasksTable = new QTableWidget();
  m_tasksTable->setColumnCount(7);
  m_tasksTable->setHorizontalHeaderLabels(
      {"ID", "Task Title", "Planned Schedule", "Completion Date", "Status",
       "Progress (%)", "Actions"});
  m_tasksTable->horizontalHeader()->setSectionResizeMode(
      0, QHeaderView::ResizeToContents);
  m_tasksTable->horizontalHeader()->setSectionResizeMode(1,
                                                         QHeaderView::Stretch);
  m_tasksTable->horizontalHeader()->setSectionResizeMode(
      2, QHeaderView::ResizeToContents);
  m_tasksTable->horizontalHeader()->setSectionResizeMode(
      3, QHeaderView::ResizeToContents);
  m_tasksTable->horizontalHeader()->setSectionResizeMode(
      4, QHeaderView::ResizeToContents);
  m_tasksTable->horizontalHeader()->setSectionResizeMode(
      5, QHeaderView::ResizeToContents);
  m_tasksTable->horizontalHeader()->setSectionResizeMode(
      6, QHeaderView::ResizeToContents);
  m_tasksTable->verticalHeader()->setVisible(false);
  m_tasksTable->setAlternatingRowColors(true);
  m_tasksTable->setMinimumHeight(240);
  tableLayout->addWidget(m_tasksTable);

  m_statusLabel = new QLabel("");
  m_statusLabel->setStyleSheet("font-weight: 700; font-size: 13px; color: "
                               "#20B486; background: transparent;");
  tableLayout->addWidget(m_statusLabel);

  mainLayout->addWidget(tableCard);

  // ==========================================
  // 5. Individual Skill Competency Progress Card
  // ==========================================
  QFrame *skillCard = new QFrame();
  skillCard->setStyleSheet("QFrame {"
                           "  background-color: #FFFFFF;"
                           "  border: 1px solid #E8EAF2;"
                           "  border-radius: 14px;"
                           "  padding: 20px;"
                           "}");
  Theme::applyCardShadow(skillCard, 14, 3, 16);

  QVBoxLayout *skillLayout = new QVBoxLayout(skillCard);
  skillLayout->setSpacing(14);

  QLabel *skillTitle =
      new QLabel("Individual Skill Progress & Competency Trajectory");
  skillTitle->setStyleSheet("font-size: 16px; font-weight: 700; color: "
                            "#202B45; background: transparent;");
  skillLayout->addWidget(skillTitle);

  m_skillProgressTable = new QTableWidget();
  m_skillProgressTable->setColumnCount(5);
  m_skillProgressTable->setHorizontalHeaderLabels(
      {"Skill Name", "Current Level", "Completed Tasks", "Total Tasks",
       "Skill Progress"});
  m_skillProgressTable->horizontalHeader()->setSectionResizeMode(
      0, QHeaderView::Stretch);
  m_skillProgressTable->horizontalHeader()->setSectionResizeMode(
      1, QHeaderView::ResizeToContents);
  m_skillProgressTable->horizontalHeader()->setSectionResizeMode(
      2, QHeaderView::ResizeToContents);
  m_skillProgressTable->horizontalHeader()->setSectionResizeMode(
      3, QHeaderView::ResizeToContents);
  m_skillProgressTable->horizontalHeader()->setSectionResizeMode(
      4, QHeaderView::Stretch);
  m_skillProgressTable->verticalHeader()->setVisible(false);
  m_skillProgressTable->setAlternatingRowColors(true);
  m_skillProgressTable->setMinimumHeight(160);
  skillLayout->addWidget(m_skillProgressTable);

  mainLayout->addWidget(skillCard);

  mainLayout->addStretch();

  scrollArea->setWidget(contentWidget);
  outerLayout->addWidget(scrollArea);
}

void ProgressPage::updateChart(int completed, int inProgress, int notStarted) {
  m_pieSeries->clear();

  if (completed == 0 && inProgress == 0 && notStarted == 0) {
    auto slice = m_pieSeries->append("No Tasks Yet", 1);
    slice->setColor(QColor("#E8EAF2"));
    return;
  }

  if (completed > 0) {
    auto s = m_pieSeries->append(QString("Completed (%1)").arg(completed),
                                 completed);
    s->setColor(QColor("#20B486")); // Success Green
  }
  if (inProgress > 0) {
    auto s = m_pieSeries->append(QString("In Progress (%1)").arg(inProgress),
                                 inProgress);
    s->setColor(QColor("#4169E1")); // Primary Blue
  }
  if (notStarted > 0) {
    auto s = m_pieSeries->append(QString("Not Started (%1)").arg(notStarted),
                                 notStarted);
    s->setColor(QColor("#F3A640")); // Warning Orange
  }
}

void ProgressPage::onFilterChanged(int) { refreshData(); }

void ProgressPage::refreshData() {
  Student *student = AuthService::instance().getCurrentStudent();
  if (!student)
    return;

  ProgressService service;
  auto summary = service.getProgressSummary(student->getId());

  m_cardTotal->setValue(QString::number(summary.totalTasks));
  m_cardCompleted->setValue(QString::number(summary.completedTasks));
  m_cardInProgress->setValue(QString::number(summary.inProgressTasks));
  m_cardPending->setValue(QString::number(summary.notStartedTasks));
  m_cardAvgProgress->setValue(QString::number(summary.averageProgress, 'f', 1) +
                              "%");

  updateChart(summary.completedTasks, summary.inProgressTasks,
              summary.notStartedTasks);

  auto allTasks = service.getStudentTasks(student->getId());
  QString filterChoice =
      m_statusFilterCombo ? m_statusFilterCombo->currentText() : "All Tasks";

  std::vector<LearningTask> filteredTasks;
  for (const auto &t : allTasks) {
    if (filterChoice == "All Tasks") {
      filteredTasks.push_back(t);
    } else if (filterChoice == "Not Started" &&
               t.getStatus() == "Not Started") {
      filteredTasks.push_back(t);
    } else if (filterChoice == "In Progress" &&
               t.getStatus() == "In Progress") {
      filteredTasks.push_back(t);
    } else if (filterChoice == "Completed" && t.getStatus() == "Completed") {
      filteredTasks.push_back(t);
    }
  }

  m_tasksTable->setRowCount(static_cast<int>(filteredTasks.size()));

  for (size_t i = 0; i < filteredTasks.size(); ++i) {
    const auto &t = filteredTasks[i];

    QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(t.getId()));
    idItem->setTextAlignment(Qt::AlignCenter);
    m_tasksTable->setItem(static_cast<int>(i), 0, idItem);

    QTableWidgetItem *titleItem =
        new QTableWidgetItem(QString::fromStdString(t.getTitle()));
    titleItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
    m_tasksTable->setItem(static_cast<int>(i), 1, titleItem);

    QTableWidgetItem *dateItem =
        new QTableWidgetItem(QString("%1 to %2")
                                 .arg(QString::fromStdString(t.getStartDate()))
                                 .arg(QString::fromStdString(t.getDueDate())));
    dateItem->setTextAlignment(Qt::AlignCenter);
    m_tasksTable->setItem(static_cast<int>(i), 2, dateItem);

    // Completion Date
    QString compDateStr = QString::fromStdString(t.getCompletionDate());
    if (compDateStr.isEmpty() && t.getStatus() == "Completed") {
      compDateStr = QString::fromStdString(t.getDueDate());
    } else if (compDateStr.isEmpty()) {
      compDateStr = "--";
    }
    QTableWidgetItem *compDateItem = new QTableWidgetItem(compDateStr);
    compDateItem->setTextAlignment(Qt::AlignCenter);
    m_tasksTable->setItem(static_cast<int>(i), 3, compDateItem);

    // Status Badge
    QLabel *statusBadge = new QLabel(QString::fromStdString(t.getStatus()));
    statusBadge->setAlignment(Qt::AlignCenter);
    if (t.getStatus() == "Completed") {
      statusBadge->setStyleSheet(
          "background-color: #E8F8F3; color: #20B486; font-weight: 700; "
          "font-size: 11px; padding: 4px 10px; border-radius: 10px;");
    } else if (t.getStatus() == "In Progress") {
      statusBadge->setStyleSheet(
          "background-color: #FEF6EC; color: #F3A640; font-weight: 700; "
          "font-size: 11px; padding: 4px 10px; border-radius: 10px;");
    } else {
      statusBadge->setStyleSheet(
          "background-color: #F1F5F9; color: #7A849B; font-weight: 700; "
          "font-size: 11px; padding: 4px 10px; border-radius: 10px;");
    }
    m_tasksTable->setCellWidget(static_cast<int>(i), 4, statusBadge);

    // Progress SpinBox
    QSpinBox *spin = new QSpinBox();
    spin->setRange(0, 100);
    spin->setSuffix("%");
    spin->setValue(t.getProgress());
    m_tasksTable->setCellWidget(static_cast<int>(i), 5, spin);

    // Action Buttons (Save + Delete)
    QWidget *actWidget = new QWidget();
    QHBoxLayout *actLayout = new QHBoxLayout(actWidget);
    actLayout->setContentsMargins(4, 2, 4, 2);
    actLayout->setSpacing(6);

    QPushButton *updateBtn = new QPushButton("Save");
    updateBtn->setStyleSheet(
        "QPushButton { background-color: #4169E1; color: #FFFFFF; font-weight: "
        "700; padding: 4px 12px; border-radius: 6px; border: none; font-size: "
        "11px; }"
        "QPushButton:hover { background-color: #3557C7; }");
    int taskId = t.getId();
    int skillId = t.getSkillId();
    QString taskTitle = QString::fromStdString(t.getTitle());
    connect(updateBtn, &QPushButton::clicked, this,
            [this, taskId, spin, skillId, taskTitle]() {
              onUpdateTaskProgress(taskId, spin->value(), skillId, taskTitle);
            });

    QPushButton *delBtn = new QPushButton("🗑️");
    delBtn->setToolTip("Delete this task");
    delBtn->setStyleSheet(
        "QPushButton { background-color: #FDF0F1; color: #E56B75; font-weight: "
        "600; padding: 4px 8px; border-radius: 6px; border: 1px solid #F8C8CC; "
        "font-size: 11px; }"
        "QPushButton:hover { background-color: #E56B75; color: #FFFFFF; }");
    connect(delBtn, &QPushButton::clicked, this,
            [this, taskId]() { onDeleteTask(taskId); });

    actLayout->addWidget(updateBtn);
    actLayout->addWidget(delBtn);
    m_tasksTable->setCellWidget(static_cast<int>(i), 6, actWidget);
  }

  // Populate Individual Skill Progress Table
  auto skillProgressList = service.getSkillWiseProgress(student->getId());
  m_skillProgressTable->setRowCount(static_cast<int>(skillProgressList.size()));

  for (size_t i = 0; i < skillProgressList.size(); ++i) {
    const auto &sp = skillProgressList[i];

    QTableWidgetItem *nameItem =
        new QTableWidgetItem(QString::fromStdString(sp.skillName));
    nameItem->setFont(QFont("Segoe UI", 10, QFont::Bold));
    m_skillProgressTable->setItem(static_cast<int>(i), 0, nameItem);

    QTableWidgetItem *lvlItem =
        new QTableWidgetItem(QString("Level %1 / 3").arg(sp.currentLevel));
    lvlItem->setTextAlignment(Qt::AlignCenter);
    m_skillProgressTable->setItem(static_cast<int>(i), 1, lvlItem);

    QTableWidgetItem *compItem =
        new QTableWidgetItem(QString::number(sp.completedTasks));
    compItem->setTextAlignment(Qt::AlignCenter);
    m_skillProgressTable->setItem(static_cast<int>(i), 2, compItem);

    QTableWidgetItem *totItem =
        new QTableWidgetItem(QString::number(sp.totalTasks));
    totItem->setTextAlignment(Qt::AlignCenter);
    m_skillProgressTable->setItem(static_cast<int>(i), 3, totItem);

    QProgressBar *pBar = new QProgressBar();
    pBar->setRange(0, 100);
    pBar->setValue(static_cast<int>(sp.progressPercent));
    pBar->setTextVisible(true);
    pBar->setFixedHeight(12);
    pBar->setStyleSheet("QProgressBar { background-color: #E8EAF2; "
                        "border-radius: 6px; } QProgressBar::chunk { "
                        "background-color: #20B486; border-radius: 6px; }");
    m_skillProgressTable->setCellWidget(static_cast<int>(i), 4, pBar);
  }
}

void ProgressPage::onUpdateTaskProgress(int taskId, int progress, int skillId,
                                        const QString &title) {
  ProgressService service;
  Student *student = AuthService::instance().getCurrentStudent();
  if (!student)
    return;

  std::string status = (progress >= 100)
                           ? "Completed"
                           : (progress > 0 ? "In Progress" : "Not Started");
  std::string completionDate =
      (progress >= 100)
          ? QDate::currentDate().toString("yyyy-MM-dd").toStdString()
          : "";

  if (service.updateTask(taskId, progress, status, completionDate)) {
    if (progress >= 100) {
      m_statusLabel->setText(
          QString("🎉 Milestone achieved! Task #%1 marked Completed!")
              .arg(taskId));
      m_statusLabel->setStyleSheet("color: #20B486; font-weight: 700;");

      // Explicit student confirmation before increasing proficiency
      SkillRepository sRepo;
      auto levels = sRepo.getStudentSkillLevels(student->getId());
      int curLevel = levels.count(skillId) ? levels[skillId] : 0;

      if (curLevel < 3) {
        int proposedLevel = curLevel + 1;
        auto reply = QMessageBox::question(
            this, "Task Completed — Confirm Skill Advancement",
            QString("Congratulations on completing '%1'!\n\n"
                    "Would you like to advance your registered proficiency "
                    "level for this skill in your academic profile?\n\n"
                    "Current Level: %2\n"
                    "Proposed Level: %3\n\n"
                    "(Proficiency will only be increased if you explicitly "
                    "confirm).")
                .arg(title)
                .arg(curLevel)
                .arg(proposedLevel),
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
          service.updateStudentSkillProficiency(student->getId(), skillId,
                                                proposedLevel);
          m_statusLabel->setText(
              QString("🎉 Task completed and proficiency advanced to Level %1!")
                  .arg(proposedLevel));
        }
      }
    } else {
      m_statusLabel->setText(QString("✓ Task #%1 updated to %2% (%3)!")
                                 .arg(taskId)
                                 .arg(progress)
                                 .arg(QString::fromStdString(status)));
      m_statusLabel->setStyleSheet("color: #4169E1; font-weight: 700;");
    }
    refreshData();
  }
}

void ProgressPage::onDeleteTask(int taskId) {
  LearningRepository repo;
  if (repo.deleteTask(taskId)) {
    m_statusLabel->setText(QString("Task #%1 deleted.").arg(taskId));
    m_statusLabel->setStyleSheet("color: #7A849B; font-weight: 700;");
    refreshData();
  }
}
