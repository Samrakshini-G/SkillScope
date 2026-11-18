#ifndef STUDENTAVATAR_H
#define STUDENTAVATAR_H

#include <QWidget>
#include <QString>

class StudentAvatar : public QWidget {
    Q_OBJECT

public:
    explicit StudentAvatar(const QString& name = "Alex Morgan", int size = 44, QWidget* parent = nullptr);

    void setName(const QString& name);
    void setAvatarSize(int size);

    QSize sizeHint() const override { return QSize(m_size, m_size); }
    QSize minimumSizeHint() const override { return QSize(m_size, m_size); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_initials;
    int m_size;
    void extractInitials(const QString& name);
};

#endif // STUDENTAVATAR_H
