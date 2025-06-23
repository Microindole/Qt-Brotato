#include "bullet.h"
#include <QBrush>
#include <QPen>
#include <cmath>

Bullet::Bullet(const QPointF &startPos, const QPointF &targetPos)
    : QGraphicsEllipseItem(-3, -3, 6, 6)
    , speed(10.0f)  // 子弹速度稍微降低
    , damage(12)    // 子弹伤害降低
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
    // 返回一个能完全包裹住子弹核心和发光效果的矩形
    // 发光效果半径为5，我们取6作为安全半径
    return QRectF(-6, -6, 12, 12);
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
    
    // 绘制子弹外层发光
    painter->setBrush(QBrush(QColor(255, 255, 0, 100)));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-5, -5, 10, 10);
    
    // 绘制子弹核心
    painter->setBrush(QBrush(Qt::yellow));
    painter->setPen(QPen(Qt::darkYellow, 1));
    painter->drawEllipse(rect());
    
    // 绘制中心亮点
    painter->setBrush(QBrush(Qt::white));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-1, -1, 2, 2);
}
