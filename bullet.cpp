#include "bullet.h"
#include <QBrush>
#include <QPen>
#include <cmath>

Bullet::Bullet(const QPointF &startPos, const QPointF &targetPos, Owner owner, int damage, float speed)
    : QGraphicsEllipseItem(-8, -8, 16, 16)
    , m_owner(owner)
    , m_speed(speed)
    , m_damage(damage)
{
    setPos(startPos);

    QPointF direction = targetPos - startPos;
    float distance = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

    if (distance > 0) {
        velocity = (direction / distance) * speed;
    }
}

// 获取子弹所有者
Bullet::Owner Bullet::getOwner() const
{
    return m_owner;
}

// 获取伤害值
int Bullet::getDamage() const
{
    return m_damage;
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

    QColor coreColor;
    QColor glowColor;

    // 根据所有者选择不同的颜色
    if (m_owner == PlayerBullet) {
        coreColor = Qt::yellow;
        glowColor = QColor(255, 255, 0, 100);
    } else { // EnemyBullet
        coreColor = QColor(255, 80, 200); // 敌人的子弹为粉紫色
        glowColor = QColor(255, 80, 200, 100);
    }

    // 绘制发光圈
    painter->setBrush(QBrush(glowColor));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-12, -12, 24, 24);

    // 绘制子弹核心
    painter->setBrush(QBrush(coreColor));
    painter->setPen(QPen(coreColor.darker(150), 2));
    painter->drawEllipse(-8, -8, 16, 16);

    // 绘制中心亮点
    painter->setBrush(QBrush(Qt::white));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-2, -2, 4, 4);
}
