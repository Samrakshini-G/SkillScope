#ifndef METRICCARD_H
#define METRICCARD_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class MetricCard : public QFrame {
    Q_OBJECT

public:
    explicit MetricCard(const QString& icon,
                        const QString& title,
                        const QString& value,
                        const QString& subtitle,
                        const QString& accentColor = "#4169E1",
                        QWidget* parent = nullptr);

    void setValue(const QString& value);
    void setSubtitle(const QString& subtitle);
    void setAccentColor(const QString& color);

private:
    void setupUi(const QString& icon, const QString& title, const QString& value, const QString& subtitle, const QString& accentColor);

    QLabel* m_iconLabel;
    QLabel* m_titleLabel;
    QLabel* m_valueLabel;
    QLabel* m_subLabel;
    QFrame* m_iconContainer;
    QString m_accentColor;
};

#endif // METRICCARD_H
