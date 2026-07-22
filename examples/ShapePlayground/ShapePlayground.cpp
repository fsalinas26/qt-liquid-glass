#include "ShapePlayground.h"

#include "QtLiquidGlass/QtLiquidGlass.h"

#include <QFontMetrics>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QTimer>

#include <algorithm>
#include <cmath>

namespace {

qreal mix(qreal from, qreal to, qreal progress) {
    return from + (to - from) * progress;
}

QPointF mixPoint(const QPointF& from,
                 const QPointF& to,
                 qreal progress) {
    return {mix(from.x(), to.x(), progress),
            mix(from.y(), to.y(), progress)};
}

QRectF mixRect(const QRectF& from, const QRectF& to, qreal progress) {
    return {mix(from.x(), to.x(), progress),
            mix(from.y(), to.y(), progress),
            mix(from.width(), to.width(), progress),
            mix(from.height(), to.height(), progress)};
}

void drawRoundedPixmap(QPainter& painter,
                       const QRectF& bounds,
                       const QPixmap& pixmap,
                       qreal radius) {
    if (pixmap.isNull()) return;

    QPainterPath clip;
    clip.addRoundedRect(bounds, radius, radius);
    painter.save();
    painter.setClipPath(clip, Qt::IntersectClip);
    painter.drawPixmap(bounds.toRect(),
                       pixmap.scaled(bounds.size().toSize(),
                                     Qt::KeepAspectRatioByExpanding,
                                     Qt::SmoothTransformation));
    painter.restore();
}

} // namespace

ShapePlayground::ShapePlayground(QWidget* parent)
    : QWidget(parent), m_albumArt(":/covers/beatles.jpg") {
    setWindowTitle("QtLiquidGlass Morphing Player");
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::StrongFocus);
    setFixedSize(700, 360);

    m_springTimer = new QTimer(this);
    m_springTimer->setTimerType(Qt::PreciseTimer);
    m_springTimer->setInterval(8);
    connect(m_springTimer, &QTimer::timeout,
            this, &ShapePlayground::advanceSpring);

    QtLiquidGlass::Options options;
    options.tintColor = "#352D164F";
    options.titlebarStyle = QtLiquidGlass::TitlebarStyle::Preserve;
    options.dragBehavior =
        QtLiquidGlass::WindowDragBehavior::MovableByWindowBackground;
    m_effectId = QtLiquidGlass::addGlassEffect(
        this, QtLiquidGlass::Material::Frosted, options);

    m_shapeSupported = m_effectId >= 0 &&
        QtLiquidGlass::Experimental::supportsCustomShapes(m_effectId);
    if (m_shapeSupported) {
        QtLiquidGlass::Experimental::setClipsToBounds(m_effectId, true);
        applyShape();
    }
}

ShapePlayground::~ShapePlayground() {
    if (m_effectId >= 0) QtLiquidGlass::remove(m_effectId);
}

QPainterPath ShapePlayground::mediaShape(qreal progress) const {
    progress = std::clamp(progress, -0.06, 1.06);

    const QPointF topJoin = mixPoint({190.0, 140.0}, {60.0, 20.0}, progress);
    const QPointF bottomJoin = mixPoint({190.0, 220.0}, {60.0, 340.0}, progress);
    const qreal right = mix(624.0, 676.0, progress);
    const qreal top = mix(140.0, 20.0, progress);
    const qreal bottom = mix(220.0, 340.0, progress);
    const qreal radius = mix(32.0, 36.0, progress);

    const QPointF lowerControl1 =
        mixPoint({169.0, 220.0}, {43.0, 340.0}, progress);
    const QPointF lowerControl2 =
        mixPoint({168.0, 232.0}, {24.0, 321.0}, progress);
    const QPointF lobeBottom =
        mixPoint({135.0, 232.0}, {24.0, 302.0}, progress);

    const QPointF leftLowerControl1 =
        mixPoint({106.0, 232.0}, {24.0, 236.0}, progress);
    const QPointF leftLowerControl2 =
        mixPoint({82.0, 208.0}, {24.0, 124.0}, progress);
    const QPointF leftmost =
        mixPoint({82.0, 180.0}, {24.0, 58.0}, progress);

    const QPointF leftUpperControl1 =
        mixPoint({82.0, 152.0}, {24.0, 37.0}, progress);
    const QPointF leftUpperControl2 =
        mixPoint({106.0, 128.0}, {43.0, 20.0}, progress);
    const QPointF lobeTop =
        mixPoint({135.0, 128.0}, {60.0, 20.0}, progress);

    const QPointF upperControl1 =
        mixPoint({168.0, 128.0}, {60.0, 20.0}, progress);
    const QPointF upperControl2 =
        mixPoint({169.0, 140.0}, {60.0, 20.0}, progress);

    QPainterPath path;
    path.moveTo(topJoin);
    path.lineTo(right - radius, top);
    path.quadTo(right, top, right, top + radius);
    path.lineTo(right, bottom - radius);
    path.quadTo(right, bottom, right - radius, bottom);
    path.lineTo(bottomJoin);
    path.cubicTo(lowerControl1, lowerControl2, lobeBottom);
    path.cubicTo(leftLowerControl1, leftLowerControl2, leftmost);
    path.cubicTo(leftUpperControl1, leftUpperControl2, lobeTop);
    path.cubicTo(upperControl1, upperControl2, topJoin);
    path.closeSubpath();
    return path;
}

void ShapePlayground::advanceSpring() {
    constexpr qreal pi = 3.14159265358979323846;
    constexpr qreal response = 0.46;
    constexpr qreal dampingRatio = 0.84;
    const qreal omega = 2.0 * pi / response;
    const qreal elapsed = m_springClock.restart() / 1000.0;
    const qreal delta = std::clamp(elapsed, 1.0 / 240.0, 1.0 / 30.0);
    const qreal acceleration =
        -2.0 * dampingRatio * omega * m_velocity
        -omega * omega * (m_progress - m_target);

    m_velocity += acceleration * delta;
    m_progress += m_velocity * delta;

    if (std::abs(m_progress - m_target) < 0.0005 &&
        std::abs(m_velocity) < 0.005) {
        m_progress = m_target;
        m_velocity = 0.0;
        m_springTimer->stop();
    }

    applyShape();
    update();
}

void ShapePlayground::applyShape() {
    if (!m_shapeSupported) return;
    QtLiquidGlass::Experimental::setShape(m_effectId,
                                          mediaShape(m_progress));
}

void ShapePlayground::toggleExpanded() {
    if (!m_shapeSupported) return;
    m_expanded = !m_expanded;
    m_target = m_expanded ? 1.0 : 0.0;
    if (!m_springTimer->isActive()) {
        m_springClock.restart();
        m_springTimer->start();
    }
}

void ShapePlayground::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        toggleExpanded();
        event->accept();
        return;
    }
    if (event->button() == Qt::RightButton) {
        close();
        return;
    }
    QWidget::mousePressEvent(event);
}

void ShapePlayground::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return) {
        toggleExpanded();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Escape) {
        close();
        return;
    }
    QWidget::keyPressEvent(event);
}

void ShapePlayground::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    applyShape();
}

void ShapePlayground::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    const qreal progress = std::clamp(m_progress, 0.0, 1.0);
    const QPainterPath shape = mediaShape(m_progress);
    painter.setClipPath(shape);
    painter.fillPath(shape, QColor(8, 5, 16, 24));

    const QRectF artBounds = mixRect({95.0, 139.0, 80.0, 80.0},
                                     {54.0, 76.0, 142.0, 142.0},
                                     progress);
    drawRoundedPixmap(painter, artBounds, m_albumArt,
                      mix(20.0, 26.0, progress));

    const QRectF titleBounds = mixRect({205.0, 150.0, 245.0, 25.0},
                                       {224.0, 78.0, 400.0, 38.0},
                                       progress);
    QFont titleFont = font();
    titleFont.setPixelSize(qRound(mix(16.0, 25.0, progress)));
    titleFont.setWeight(QFont::Bold);
    painter.setFont(titleFont);
    painter.setPen(QColor(255, 255, 255));
    const QString title = "Sgt. Pepper's Lonely Hearts Club Band";
    painter.drawText(titleBounds,
                     Qt::AlignLeft | Qt::AlignVCenter,
                     QFontMetrics(titleFont).elidedText(
                         title, Qt::ElideRight, qRound(titleBounds.width())));

    const QRectF artistBounds = mixRect({205.0, 176.0, 220.0, 21.0},
                                        {224.0, 119.0, 300.0, 28.0},
                                        progress);
    QFont artistFont = font();
    artistFont.setPixelSize(qRound(mix(12.0, 15.0, progress)));
    painter.setFont(artistFont);
    painter.setPen(QColor(255, 255, 255, 178));
    painter.drawText(artistBounds,
                     Qt::AlignLeft | Qt::AlignVCenter,
                     "The Beatles");

    const qreal controlsY = mix(180.0, 202.0, progress);
    const qreal previousX = mix(478.0, 330.0, progress);
    const qreal playX = mix(526.0, 380.0, progress);
    const qreal nextX = mix(574.0, 430.0, progress);
    QFont controlFont = font();
    controlFont.setPixelSize(qRound(mix(20.0, 28.0, progress)));
    controlFont.setWeight(QFont::DemiBold);
    painter.setFont(controlFont);
    painter.setPen(Qt::white);
    painter.drawText(QRectF(previousX - 22.0, controlsY - 22.0, 44.0, 44.0),
                     Qt::AlignCenter, QString::fromUtf8("⏮"));
    painter.drawText(QRectF(playX - 22.0, controlsY - 22.0, 44.0, 44.0),
                     Qt::AlignCenter, QString::fromUtf8("▶"));
    painter.drawText(QRectF(nextX - 22.0, controlsY - 22.0, 44.0, 44.0),
                     Qt::AlignCenter, QString::fromUtf8("⏭"));

    const qreal progressLeft = mix(205.0, 224.0, progress);
    const qreal progressRight = mix(598.0, 630.0, progress);
    const qreal progressY = mix(208.0, 281.0, progress);
    const qreal playedX = mix(progressLeft, progressRight, 0.34);
    painter.setPen(QPen(QColor(255, 255, 255, 48), 4.0,
                        Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(progressLeft, progressY),
                     QPointF(progressRight, progressY));
    painter.setPen(QPen(QColor(56, 224, 239), 4.0,
                        Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(progressLeft, progressY),
                     QPointF(playedX, progressY));
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(playedX, progressY), 6.0, 6.0);

    painter.setOpacity(progress);
    QFont eyebrowFont = font();
    eyebrowFont.setPixelSize(11);
    eyebrowFont.setLetterSpacing(QFont::AbsoluteSpacing, 1.8);
    eyebrowFont.setWeight(QFont::DemiBold);
    painter.setFont(eyebrowFont);
    painter.setPen(QColor(255, 255, 255, 155));
    painter.drawText(QRectF(54.0, 38.0, 180.0, 22.0),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     "NOW PLAYING");
    painter.drawText(QRectF(500.0, 38.0, 130.0, 22.0),
                     Qt::AlignRight | Qt::AlignVCenter,
                     "CLICK TO COLLAPSE");
    painter.setOpacity(1.0);

    if (!m_shapeSupported) {
        painter.setClipping(false);
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter,
                         "Custom glass paths are unavailable on this runtime.");
    }
}
