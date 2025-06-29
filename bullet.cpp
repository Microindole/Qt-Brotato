#include "bullet.h"
#include "resourcemanager.h"
#include <QBrush>
#include <QPen>
#include <cmath>

Bullet::Bullet(const QPointF &startPos, const QPointF &targetPos, Owner owner, int damage, float speed)
    : QGraphicsEllipseItem()
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

    // 根据所有者决定是加载图片还是使用默认绘制
    if (m_owner == EnemyBullet) {
        m_pixmap = ResourceManager::instance().getPixmap(":/images/m_bullet.png");
        if (!m_pixmap.isNull()) {
            qreal scale = 0.1;
            setRect(-m_pixmap.width() * scale / 2.0, -m_pixmap.height() * scale / 2.0, m_pixmap.width() * scale, m_pixmap.height() * scale);
        }
    } else {
        setRect(-8, -8, 16, 16);
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
    return rect().adjusted(-2, -2, 2, 2);
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
    if (!m_pixmap.isNull()) {
        painter->drawPixmap(rect(), m_pixmap, m_pixmap.rect());
    } else {
        // 玩家的子弹使用原来的绘制逻辑
        QColor coreColor = Qt::yellow;
        QColor glowColor = QColor(255, 255, 0, 100);

        // 绘制发光圈
        painter->setBrush(QBrush(glowColor));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(rect().adjusted(-4,-4,4,4));

        // 绘制子弹核心
        painter->setBrush(QBrush(coreColor));
        painter->setPen(QPen(coreColor.darker(150), 2));
        painter->drawEllipse(rect());

        // 绘制中心亮点
        painter->setBrush(QBrush(Qt::white));
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(rect().center(), 2, 2);
    }
}
