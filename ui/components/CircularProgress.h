#ifndef CIRCULARPROGRESS_H
#define CIRCULARPROGRESS_H

#include <QWidget>
#include <QColor>

class CircularProgress : public QWidget {
    Q_OBJECT

public:
    explicit CircularProgress(QWidget* parent = nullptr);

    void setValue(double percentage);
    double value() const { return m_value; }

    void setSubText(const QString& text);
    void setThickness(int thickness) { m_thickness = thickness; update(); }
    void setCustomColor(const QColor& color) { m_customColor = color; m_useCustomColor = true; update(); }
    void setAutoColor(bool autoColor) { m_useCustomColor = !autoColor; update(); }

    QSize sizeHint() const override { return QSize(160, 160); }
    QSize minimumSizeHint() const override { return QSize(120, 120); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double m_value; // 0.0 to 100.0
    int m_thickness;
    QString m_subText;
    bool m_useCustomColor;
    QColor m_customColor;
};

#endif // CIRCULARPROGRESS_H
