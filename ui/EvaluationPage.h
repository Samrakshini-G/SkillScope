#ifndef EVALUATIONPAGE_H
#define EVALUATIONPAGE_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QTextEdit>

#include "components/MetricCard.h"
#include "../services/EvaluationService.h"

class EvaluationPage : public QWidget {
    Q_OBJECT

public:
    explicit EvaluationPage(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onRunBenchmark();

private:
    void setupUi();

    MetricCard* m_cardProcedural;
    MetricCard* m_cardOop;
    MetricCard* m_cardSpeedup;

    QTableWidget* m_comparisonTable;
    QTextEdit* m_reportText;
};

#endif // EVALUATIONPAGE_H
