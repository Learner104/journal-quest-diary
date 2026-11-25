#pragma once

#include <QWidget>
#include <QPainter>

class GardenWidget : public QWidget {
    Q_OBJECT
public:
    explicit GardenWidget(QWidget* parent = nullptr);

    void setGardenLevel(int level); // 0 = Dormant Seed, 1 = Sprout, 2 = Growing Bush, 3 = Blooming Flower, 4 = Thriving Sanctuary
    int gardenLevel() const { return m_level; }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    int m_level = 0;
};
