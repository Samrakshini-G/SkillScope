#include "EvaluationPage.h"
#include "Theme.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>

EvaluationPage::EvaluationPage(QWidget *parent)
    : QWidget(parent),
      m_cardProcedural(nullptr),
      m_cardOop(nullptr),
      m_cardSpeedup(nullptr),
      m_comparisonTable(nullptr),
      m_reportText(nullptr) {
    setupUi();
}

void EvaluationPage::setupUi() {
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
    headerCard->setObjectName("evalHeaderCard");
    headerCard->setStyleSheet(
        "#evalHeaderCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "  padding: 20px 24px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; }"
    );
    Theme::applyCardShadow(headerCard, 14, 3, 16);

    QVBoxLayout* headerLayout = new QVBoxLayout(headerCard);
    headerLayout->setSpacing(4);

    QLabel* title = new QLabel("Academic Evaluation: Procedural vs Object-Oriented Paradigm");
    title->setStyleSheet("font-size: 20px; font-weight: 800; color: #202B45; background: transparent;");

    QLabel* subtitle = new QLabel(
        "Quantitative empirical benchmark and qualitative comparative analysis of software design paradigms for career skill gap detection and STL priority management."
    );
    subtitle->setStyleSheet("font-size: 13px; color: #7A849B; background: transparent;");

    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    mainLayout->addWidget(headerCard);

    // ==========================================
    // 2. Live Performance Benchmark Card
    // ==========================================
    QFrame* benchCard = new QFrame();
    benchCard->setObjectName("evalBenchCard");
    benchCard->setStyleSheet(
        "#evalBenchCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 16px;"
        "  padding: 22px 26px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; }"
    );
    Theme::applyCardShadow(benchCard, 14, 3, 16);

    QVBoxLayout* benchLayout = new QVBoxLayout(benchCard);
    benchLayout->setSpacing(16);

    QHBoxLayout* benchHeader = new QHBoxLayout();
    QVBoxLayout* benchTitleCol = new QVBoxLayout();
    benchTitleCol->setSpacing(2);

    QLabel* benchTitle = new QLabel("Live Performance Benchmark (5,000 Iterations)");
    benchTitle->setStyleSheet("font-size: 16px; font-weight: 800; color: #202B45; background: transparent;");

    QLabel* benchSub = new QLabel("Measures execution latency of procedural array-sorting vs STL priority_queue max-heap.");
    benchSub->setStyleSheet("font-size: 12px; color: #7A849B; background: transparent;");

    benchTitleCol->addWidget(benchTitle);
    benchTitleCol->addWidget(benchSub);
    benchHeader->addLayout(benchTitleCol);
    benchHeader->addStretch();

    QPushButton* runBtn = new QPushButton("⚡  Execute Benchmark");
    runBtn->setStyleSheet(Theme::primaryButtonStyle());
    runBtn->setCursor(Qt::PointingHandCursor);
    runBtn->setMinimumWidth(180);
    runBtn->setMinimumHeight(38);
    connect(runBtn, &QPushButton::clicked, this, &EvaluationPage::onRunBenchmark);
    benchHeader->addWidget(runBtn);

    benchLayout->addLayout(benchHeader);

    // 3 Metric Cards Row
    QHBoxLayout* metricRow = new QHBoxLayout();
    metricRow->setSpacing(18);

    m_cardProcedural = new MetricCard("⏱️", "Procedural (Bubble Sort)", "-- µs", "O(K²) raw struct swap latency", "#E56B75");
    m_cardOop = new MetricCard("⚡", "OOP / STL (Priority Queue)", "-- µs", "O(K log K) binary max-heap latency", "#20B486");
    m_cardSpeedup = new MetricCard("📊", "Empirical Efficiency Gain", "-- x", "Relative throughput factor", "#4169E1");

    metricRow->addWidget(m_cardProcedural);
    metricRow->addWidget(m_cardOop);
    metricRow->addWidget(m_cardSpeedup);
    benchLayout->addLayout(metricRow);

    // Algorithmic Analysis Notice Banner
    QFrame* algoNotice = new QFrame();
    algoNotice->setObjectName("evalAlgoNotice");
    algoNotice->setStyleSheet(
        "#evalAlgoNotice {"
        "  background: #F8FAFC;"
        "  border: 1px solid #E8EAF2;"
        "  border-left: 4px solid #4169E1;"
        "  border-radius: 8px;"
        "  padding: 10px 14px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; }"
    );
    QVBoxLayout* algoNoticeLayout = new QVBoxLayout(algoNotice);
    algoNoticeLayout->setSpacing(2);
    QLabel* noticeTitle = new QLabel("💡 Algorithmic vs Architectural Complexity Distinction");
    noticeTitle->setStyleSheet("font-size: 12px; font-weight: 700; color: #4169E1; background: transparent;");
    QLabel* noticeBody = new QLabel(
        "Algorithmic scaling differences: std::priority_queue utilizes an O(N log K) binary heap versus the O(K²) quadratic bubble sort. "
        "Architectural trade-offs: Plain structs offer cache locality, while OOP encapsulation guarantees class invariants and runtime safety."
    );
    noticeBody->setStyleSheet("font-size: 11px; color: #7A849B; background: transparent; line-height: 1.4;");
    noticeBody->setWordWrap(true);
    algoNoticeLayout->addWidget(noticeTitle);
    algoNoticeLayout->addWidget(noticeBody);
    benchLayout->addWidget(algoNotice);

    mainLayout->addWidget(benchCard);

    // ==========================================
    // 3. Architectural & Paradigm Comparison Matrix
    // ==========================================
    QFrame* tableCard = new QFrame();
    tableCard->setObjectName("evalTableCard");
    tableCard->setStyleSheet(
        "#evalTableCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "  padding: 20px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; }"
    );
    Theme::applyCardShadow(tableCard, 14, 3, 16);

    QVBoxLayout* tableLayout = new QVBoxLayout(tableCard);
    tableLayout->setSpacing(12);

    QLabel* tableTitle = new QLabel("Architectural & Paradigm Comparison Matrix");
    tableTitle->setStyleSheet("font-size: 16px; font-weight: 800; color: #202B45; background: transparent;");
    tableLayout->addWidget(tableTitle);

    m_comparisonTable = new QTableWidget();
    m_comparisonTable->setColumnCount(3);
    m_comparisonTable->setHorizontalHeaderLabels({"Dimension / Criterion", "Procedural Paradigm", "Object-Oriented + STL Paradigm"});
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_comparisonTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_comparisonTable->verticalHeader()->setVisible(false);
    m_comparisonTable->setAlternatingRowColors(true);
    m_comparisonTable->setMinimumHeight(210);

    struct CompRow {
        QString dim;
        QString proc;
        QString oop;
    };

    const QList<CompRow> rows = {
        {"Data Modeling & Invariants", "Plain C structs with public data fields. No bounds enforcement on level (0..3) or task progress (0..100%).", "Encapsulated classes with invariants strictly guaranteed via constructors and mutating methods."},
        {"Resource Polymorphism", "Switch/case statements keyed on integer/enum tags. Adding a new resource requires modifying all dispatchers.", "Pure virtual resourceType() & learningDescription(). Open-Closed Principle (OCP) compliant."},
        {"Roadmap Prioritization", "Manual O(K²) bubble sort on raw array buffers with temporary swap logic.", "std::priority_queue binary max-heap (O(K log K)) parameterized with custom functor comparator."},
        {"Code Modularity & Coupling", "Functions coupled directly to memory layouts of global/passed structs.", "Layered architecture: Presentation, Service, Model, and Database repositories are decoupled."},
        {"Extensibility", "High friction: changing data fields impacts every procedural function.", "Low friction: new behaviors are added by extending classes or introducing service interfaces."}
    };

    m_comparisonTable->setRowCount(rows.size());
    for (int i = 0; i < rows.size(); ++i) {
        QTableWidgetItem* d = new QTableWidgetItem(rows[i].dim);
        d->setFont(QFont("Segoe UI", 10, QFont::Bold));
        m_comparisonTable->setItem(i, 0, d);

        QTableWidgetItem* p = new QTableWidgetItem(rows[i].proc);
        p->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_comparisonTable->setItem(i, 1, p);

        QTableWidgetItem* o = new QTableWidgetItem(rows[i].oop);
        o->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        m_comparisonTable->setItem(i, 2, o);
    }

    tableLayout->addWidget(m_comparisonTable);
    mainLayout->addWidget(tableCard);

    // ==========================================
    // 4. Formal Academic Report Card
    // ==========================================
    QFrame* reportCard = new QFrame();
    reportCard->setObjectName("evalReportCard");
    reportCard->setStyleSheet(
        "#evalReportCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 14px;"
        "  padding: 20px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; }"
    );
    Theme::applyCardShadow(reportCard, 14, 3, 16);

    QVBoxLayout* reportLayout = new QVBoxLayout(reportCard);
    reportLayout->setSpacing(10);

    QLabel* reportTitle = new QLabel("Formal Academic Evaluation Report");
    reportTitle->setStyleSheet("font-size: 16px; font-weight: 800; color: #202B45; background: transparent;");
    reportLayout->addWidget(reportTitle);

    m_reportText = new QTextEdit();
    m_reportText->setReadOnly(true);
    m_reportText->setMinimumHeight(180);
    m_reportText->setFont(QFont("Consolas", 10));
    m_reportText->setStyleSheet(
        "background-color: #F8FAFC;"
        "border: 1px solid #E8EAF2;"
        "border-radius: 8px;"
        "padding: 14px;"
        "color: #202B45;"
    );

    EvaluationService evalService;
    m_reportText->setPlainText(QString::fromStdString(evalService.getDetailedAcademicReport()));
    reportLayout->addWidget(m_reportText);

    mainLayout->addWidget(reportCard);

    mainLayout->addStretch();

    scrollArea->setWidget(contentWidget);
    outerLayout->addWidget(scrollArea);
}

void EvaluationPage::refreshData() {
    onRunBenchmark();
}

void EvaluationPage::onRunBenchmark() {
    EvaluationService service;
    auto res = service.runComparisonBenchmark(5000);

    m_cardProcedural->setValue(QString("%1 µs").arg(res.proceduralDurationUs));
    m_cardOop->setValue(QString("%1 µs").arg(res.oopDurationUs));
    m_cardSpeedup->setValue(QString("%1x").arg(QString::number(res.speedupRatio, 'f', 2)));
}
