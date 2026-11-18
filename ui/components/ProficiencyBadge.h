#ifndef PROFICIENCYBADGE_H
#define PROFICIENCYBADGE_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

class ProficiencyBadge : public QWidget {
    Q_OBJECT

public:
    explicit ProficiencyBadge(int level = 0, bool showMeter = true, QWidget* parent = nullptr);

    void setLevel(int level);
    int level() const { return m_level; }

    static QString levelName(int level);
    static QString levelColor(int level);
    static QString levelBgColor(int level);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_level;
    bool m_showMeter;
    QLabel* m_label;
    void updateAppearance();
};

#endif // PROFICIENCYBADGE_H
