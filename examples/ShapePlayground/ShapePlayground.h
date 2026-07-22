#ifndef SHAPEPLAYGROUND_H
#define SHAPEPLAYGROUND_H

#include <QElapsedTimer>
#include <QPainterPath>
#include <QPixmap>
#include <QWidget>

class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QTimer;

class ShapePlayground final : public QWidget {
public:
    explicit ShapePlayground(QWidget* parent = nullptr);
    ~ShapePlayground() override;

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QPainterPath mediaShape(qreal progress) const;
    void advanceSpring();
    void applyShape();
    void toggleExpanded();

    QPixmap m_albumArt;
    QTimer* m_springTimer = nullptr;
    QElapsedTimer m_springClock;
    int m_effectId = -1;
    qreal m_progress = 0.0;
    qreal m_velocity = 0.0;
    qreal m_target = 0.0;
    bool m_expanded = false;
    bool m_shapeSupported = false;
};

#endif
