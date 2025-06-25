#include "bullet.h"
#include <QBrush>
#include <QPen>
#include <cmath>

Bullet::Bullet(const QPointF &startPos, const QPointF &targetPos)
    : QGraphicsEllipseItem(-8, -8, 16, 16) // 变大
    , speed(10.0f)
    , damage(12)
{
    setPos(startPos);

    QPointF direction = targetPos - startPos;
    float distance = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

    if (distance > 0) {
        velocity = (direction / distance) * speed;
    }
}

QRectF Bullet::boundingRect() const
{
    // 变大，发光圈半径12
    return QRectF(-12, -12, 24, 24);
}

void Bullet::move()
{
    setPos(pos() + velocity);
}

bool Bullet::isOutOfBounds(const QRectF &bounds) const
{
    QPointF currentPos = pos();
    return currentPos.x() < bounds.left() - 20 || 
           currentPos.x() > bounds.right() + 20 ||
           currentPos.y() < bounds.top() - 20 || 
           currentPos.y() > bounds.bottom() + 20;
}

void Bullet::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    // 发光圈变大
    painter->setBrush(QBrush(QColor(255, 255, 0, 100)));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-12, -12, 24, 24);

    // 子弹核心变大
    painter->setBrush(QBrush(Qt::yellow));
    painter->setPen(QPen(Qt::darkYellow, 2));
    painter->drawEllipse(-8, -8, 16, 16);

    // 中心亮点也适当变大
    painter->setBrush(QBrush(Qt::white));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-2, -2, 4, 4);
}
