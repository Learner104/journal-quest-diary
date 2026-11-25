#include "GardenWidget.h"
#include "ThemeManager.h"
#include <QPainterPath>
#include <QLinearGradient>

GardenWidget::GardenWidget(QWidget* parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(160);
}

void GardenWidget::setGardenLevel(int level) {
    if (m_level != level) {
        m_level = level;
        update();
    }
}

void GardenWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();
    const auto& t = ThemeManager::instance().currentTheme();

    // Garden Card Frame
    QPainterPath bgPath;
    bgPath.addRoundedRect(0, 0, w, h, 14, 14);
    QLinearGradient bgGrad(0, 0, 0, h);
    if (t.isDark) {
        bgGrad.setColorAt(0.0, QColor("#22201E"));
        bgGrad.setColorAt(1.0, QColor("#1A1918"));
    } else {
        bgGrad.setColorAt(0.0, QColor("#F4EFE6"));
        bgGrad.setColorAt(1.0, QColor("#EAE0D0"));
    }
    p.fillPath(bgPath, bgGrad);
    p.setPen(QPen(QColor(t.border), 1.5));
    p.drawPath(bgPath);

    // Ground / Soil Mound
    p.setBrush(t.isDark ? QColor("#3E2D1E") : QColor("#8A6849"));
    p.setPen(Qt::NoPen);
    p.drawChord(QRect(w / 2 - 120, h - 45, 240, 70), 0, 180 * 16);

    int centerX = w / 2;
    int groundY = h - 25;

    if (m_level == 0) {
        // Dormant Seed Stage
        p.setBrush(QColor("#B2906C"));
        p.setPen(QPen(t.isDark ? QColor("#8A6F50") : QColor("#5C4033"), 1.5));
        p.drawEllipse(QPoint(centerX, groundY - 2), 6, 4);

        p.setPen(QColor(t.textSecondary));
        QFont f = p.font();
        f.setPointSize(10);
        f.setItalic(true);
        p.setFont(f);
        p.drawText(QRect(0, 20, w, 24), Qt::AlignCenter, "A tiny seed rests in warm earth... Write your first entry!");
    } else if (m_level == 1) {
        // Sprouting Seed Stage
        p.setPen(QPen(QColor("#4A7C59"), 3, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(centerX, groundY, centerX, groundY - 30);

        // Tiny leaves
        p.setBrush(QColor("#68A357"));
        p.setPen(QPen(QColor("#4A7C59"), 1.5));
        p.drawEllipse(centerX - 10, groundY - 35, 10, 7);
        p.drawEllipse(centerX, groundY - 35, 10, 7);

        p.setPen(QColor(t.textPrimary));
        QFont f = p.font();
        f.setPointSize(10);
        f.setBold(true);
        p.setFont(f);
        p.drawText(QRect(0, 18, w, 24), Qt::AlignCenter, "Sprout of Thought (Stage 1)");
    } else if (m_level == 2) {
        // Growing Bush / Stem
        p.setPen(QPen(QColor("#3E6B48"), 4, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(centerX, groundY, centerX, groundY - 55);
        p.drawLine(centerX, groundY - 25, centerX - 20, groundY - 45);
        p.drawLine(centerX, groundY - 30, centerX + 20, groundY - 50);

        // Leaves
        p.setBrush(QColor("#5D9A5B"));
        p.setPen(QPen(QColor("#355E3B"), 1.5));
        p.drawEllipse(centerX - 28, groundY - 52, 16, 10);
        p.drawEllipse(centerX + 14, groundY - 58, 16, 10);
        p.drawEllipse(centerX - 8, groundY - 65, 16, 12);

        p.setPen(QColor(t.textPrimary));
        QFont f = p.font();
        f.setPointSize(10);
        f.setBold(true);
        p.setFont(f);
        p.drawText(QRect(0, 18, w, 24), Qt::AlignCenter, "Nurtured Sapling (Stage 2)");
    } else if (m_level == 3) {
        // Blooming Flower Stage
        p.setPen(QPen(QColor("#355E3B"), 4, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(centerX, groundY, centerX, groundY - 65);

        // Foliage
        p.setBrush(QColor("#5D9A5B"));
        p.setPen(QPen(QColor("#294929"), 1.5));
        p.drawEllipse(centerX - 24, groundY - 35, 18, 10);
        p.drawEllipse(centerX + 6, groundY - 45, 18, 10);

        // Flower Petals
        int flowerY = groundY - 72;
        p.setBrush(QColor("#E27D60"));
        p.setPen(QPen(QColor("#C35C3E"), 1.5));
        p.drawEllipse(centerX - 16, flowerY - 12, 14, 14);
        p.drawEllipse(centerX + 2, flowerY - 12, 14, 14);
        p.drawEllipse(centerX - 7, flowerY - 22, 14, 14);
        p.drawEllipse(centerX - 7, flowerY - 2, 14, 14);

        // Flower Core
        p.setBrush(QColor("#F4D06F"));
        p.setPen(QPen(QColor("#D4AC0D"), 1.5));
        p.drawEllipse(centerX - 6, flowerY - 11, 12, 12);

        p.setPen(QColor(t.textPrimary));
        QFont f = p.font();
        f.setPointSize(10);
        f.setBold(true);
        p.setFont(f);
        p.drawText(QRect(0, 18, w, 24), Qt::AlignCenter, "Blooming Soul Garden (Stage 3)");
    } else {
        // Flourishing Sanctuary (Stage 4)
        p.setPen(QPen(QColor("#3E6B48"), 3, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(centerX - 40, groundY, centerX - 40, groundY - 45);
        p.drawLine(centerX + 40, groundY, centerX + 40, groundY - 45);

        p.setBrush(QColor("#85B880"));
        p.setPen(Qt::NoPen);
        p.drawEllipse(centerX - 50, groundY - 35, 12, 8);
        p.drawEllipse(centerX + 38, groundY - 35, 12, 8);

        p.setBrush(QColor("#9E768F"));
        p.drawEllipse(centerX - 46, groundY - 55, 12, 12);
        p.setBrush(QColor("#52B2CF"));
        p.drawEllipse(centerX + 34, groundY - 55, 12, 12);

        p.setPen(QPen(QColor("#294929"), 5, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(centerX, groundY, centerX, groundY - 75);

        p.setBrush(QColor("#538D4E"));
        p.setPen(QPen(QColor("#244222"), 1.5));
        p.drawEllipse(centerX - 35, groundY - 85, 30, 24);
        p.drawEllipse(centerX + 5, groundY - 85, 30, 24);
        p.drawEllipse(centerX - 18, groundY - 95, 36, 28);

        p.setBrush(QColor("#F4D06F"));
        p.setPen(Qt::NoPen);
        p.drawEllipse(centerX - 50, groundY - 80, 5, 5);
        p.drawEllipse(centerX + 55, groundY - 75, 6, 6);
        p.drawEllipse(centerX - 10, groundY - 110, 5, 5);

        p.setPen(QColor(t.textPrimary));
        QFont f = p.font();
        f.setPointSize(10);
        f.setBold(true);
        p.setFont(f);
        p.drawText(QRect(0, 18, w, 24), Qt::AlignCenter, "Flourishing Sanctuary (Master Stage)");
    }
}
