#ifndef LOGOBADGE_H
#define LOGOBADGE_H

#include <QWidget>

class LogoBadge : public QWidget {
    Q_OBJECT

public:
    explicit LogoBadge(int size = 42, QWidget* parent = nullptr);
    void setBadgeSize(int size);

    QSize sizeHint() const override { return QSize(m_size, m_size); }
    QSize minimumSizeHint() const override { return QSize(m_size, m_size); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_size;
};

#endif // LOGOBADGE_H
