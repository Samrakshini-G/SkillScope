#ifndef THEME_H
#define THEME_H

#include <QString>
#include <QColor>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QWidget>

namespace Theme {
    // -------------------------------------------------------------------------
    // Core Color Palette (as requested in Section 2)
    // -------------------------------------------------------------------------
    inline const QString ColorMainBg       = "#F5F6FC"; // Soft light background
    inline const QString ColorCardBg       = "#FFFFFF"; // Clean white cards
    inline const QString ColorSidebarBg    = "#FFFFFF"; // Pure white sidebar
    inline const QString ColorPrimary      = "#4169E1"; // Primary Royal Blue
    inline const QString ColorPrimaryHover = "#3557C7";
    inline const QString ColorPrimaryLight = "#EEF2FF"; // Soft blue active/tint
    inline const QString ColorSecondary    = "#8064E8"; // Secondary Purple
    inline const QString ColorSecondaryLight = "#F3F0FD";
    inline const QString ColorTeal         = "#25B99A"; // Teal
    inline const QString ColorSuccess      = "#20B486"; // Success Green
    inline const QString ColorSuccessLight = "#E8F8F3";
    inline const QString ColorWarning      = "#F3A640"; // Warning Orange
    inline const QString ColorWarningLight = "#FEF6EC";
    inline const QString ColorError        = "#E56B75"; // Error Red
    inline const QString ColorErrorLight   = "#FDF0F1";
    inline const QString ColorMainText     = "#202B45"; // Dark navy/slate text
    inline const QString ColorTextSecondary= "#7A849B"; // Neutral secondary text
    inline const QString ColorTextMuted    = "#A0A8BF"; // Muted placeholders
    inline const QString ColorBorder       = "#E8EAF2"; // Subtle border
    inline const QString ColorBorderHover  = "#CBD5E1";

    // -------------------------------------------------------------------------
    // QColor objects for custom QPainter drawing
    // -------------------------------------------------------------------------
    inline QColor qColorMainBg()        { return QColor(ColorMainBg); }
    inline QColor qColorCardBg()        { return QColor(ColorCardBg); }
    inline QColor qColorPrimary()       { return QColor(ColorPrimary); }
    inline QColor qColorPrimaryLight()  { return QColor(ColorPrimaryLight); }
    inline QColor qColorSecondary()     { return QColor(ColorSecondary); }
    inline QColor qColorSuccess()       { return QColor(ColorSuccess); }
    inline QColor qColorWarning()       { return QColor(ColorWarning); }
    inline QColor qColorError()         { return QColor(ColorError); }
    inline QColor qColorTextMain()      { return QColor(ColorMainText); }
    inline QColor qColorTextSecondary() { return QColor(ColorTextSecondary); }
    inline QColor qColorBorder()        { return QColor(ColorBorder); }

    // -------------------------------------------------------------------------
    // Helper to apply soft subtle shadows to card frames
    // -------------------------------------------------------------------------
    inline void applyCardShadow(QWidget* widget, int blurRadius = 14, int offset = 3, int alpha = 16) {
        if (!widget) return;
        auto* shadow = new QGraphicsDropShadowEffect(widget);
        shadow->setBlurRadius(blurRadius);
        shadow->setOffset(0, offset);
        shadow->setColor(QColor(32, 43, 69, alpha));
        widget->setGraphicsEffect(shadow);
    }

    // -------------------------------------------------------------------------
    // Stylesheet string generators targeting specific object IDs
    // -------------------------------------------------------------------------
    inline QString cardStyle(const QString& objName = "card", int radius = 14) {
        return QString(
            "#%1 {"
            "  background-color: %2;"
            "  border: 1px solid %3;"
            "  border-radius: %4px;"
            "}"
            "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
        ).arg(objName, ColorCardBg, ColorBorder).arg(radius);
    }

    inline QString primaryButtonStyle() {
        return QString(
            "QPushButton {"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 #3557C7);"
            "  color: #FFFFFF;"
            "  font-weight: 700;"
            "  font-size: 13px;"
            "  padding: 10px 20px;"
            "  border-radius: 9px;"
            "  border: none;"
            "}"
            "QPushButton:hover {"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #3557C7, stop:1 #2A46A8);"
            "}"
            "QPushButton:pressed {"
            "  background-color: #2A46A8;"
            "}"
            "QPushButton:disabled {"
            "  background-color: #E2E8F0;"
            "  color: #94A3B8;"
            "}"
        ).arg(ColorPrimary);
    }

    inline QString secondaryButtonStyle() {
        return QString(
            "QPushButton {"
            "  background-color: #FFFFFF;"
            "  color: %1;"
            "  font-weight: 600;"
            "  font-size: 13px;"
            "  padding: 8px 16px;"
            "  border-radius: 9px;"
            "  border: 1px solid %2;"
            "}"
            "QPushButton:hover {"
            "  background-color: #F8FAFC;"
            "  border-color: %3;"
            "  color: %4;"
            "}"
            "QPushButton:pressed {"
            "  background-color: #EDF2F7;"
            "}"
        ).arg(ColorMainText, ColorBorder, ColorPrimary, ColorPrimary);
    }

    inline QString successButtonStyle() {
        return QString(
            "QPushButton {"
            "  background-color: %1;"
            "  color: #FFFFFF;"
            "  font-weight: 700;"
            "  font-size: 13px;"
            "  padding: 10px 20px;"
            "  border-radius: 9px;"
            "  border: none;"
            "}"
            "QPushButton:hover {"
            "  background-color: #1AA076;"
            "}"
        ).arg(ColorSuccess);
    }

    inline QString dangerButtonStyle() {
        return QString(
            "QPushButton {"
            "  background-color: %1;"
            "  color: %2;"
            "  font-weight: 600;"
            "  font-size: 12px;"
            "  padding: 8px 16px;"
            "  border-radius: 8px;"
            "  border: 1px solid #F8C8CC;"
            "}"
            "QPushButton:hover {"
            "  background-color: %2;"
            "  color: #FFFFFF;"
            "  border-color: %2;"
            "}"
        ).arg(ColorErrorLight, ColorError);
    }

    inline QString badgeStyle(const QString& bg, const QString& text, int radius = 12) {
        return QString(
            "QLabel {"
            "  background-color: %1;"
            "  color: %2;"
            "  font-weight: 700;"
            "  font-size: 11px;"
            "  padding: 4px 10px;"
            "  border-radius: %3px;"
            "  border: none;"
            "}"
        ).arg(bg, text).arg(radius);
    }
}

#endif // THEME_H
