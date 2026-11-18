#ifndef EMPTYSTATEWIDGET_H
#define EMPTYSTATEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class EmptyStateWidget : public QWidget {
    Q_OBJECT

public:
    explicit EmptyStateWidget(const QString& icon,
                              const QString& title,
                              const QString& description,
                              const QString& actionText = "",
                              QWidget* parent = nullptr);

    void setActionText(const QString& actionText);

signals:
    void actionClicked();

private:
    QLabel* m_iconLabel;
    QLabel* m_titleLabel;
    QLabel* m_descLabel;
    QPushButton* m_actionBtn;
};

#endif // EMPTYSTATEWIDGET_H
