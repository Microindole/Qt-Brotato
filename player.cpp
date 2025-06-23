#include "player.h"
#include <QBrush>
#include <QPen>

Player::Player()
    : QGraphicsEllipseItem(-15, -15, 30, 30)
    , maxHealth(80)    // 玩家最大血量降低
    , health(80)       // 当前血量
    , speed(3.5f)      // 移动速度稍微降低
{
    // 设置玩家的样式
    setBrush(QBrush(QColor(139, 69, 19))); // 土豆色
    setPen(QPen(Qt::black, 2));
}

void Player::takeDamage(int damage)
{
    health -= damage;
    if (health < 0) health = 0;
}

void Player::heal(int amount)
{
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

void Player::resetHealth()
{
    health = maxHealth;
}

void Player::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    // 绘制土豆主体
    painter->setBrush(QBrush(QColor(139, 69, 19))); // 棕色土豆
    painter->setPen(QPen(Qt::black, 2));
    painter->drawEllipse(rect());
    
    // 绘制土豆的斑点
    painter->setBrush(QBrush(QColor(101, 67, 33)));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-8, -5, 4, 3);
    painter->drawEllipse(5, -8, 3, 4);
    painter->drawEllipse(-3, 8, 5, 3);
    
    // 绘制眼睛
    painter->setBrush(QBrush(Qt::white));
    painter->setPen(QPen(Qt::black, 1));
    painter->drawEllipse(-8, -8, 6, 6);
    painter->drawEllipse(2, -8, 6, 6);
    
    // 绘制瞳孔
    painter->setBrush(QBrush(Qt::black));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-6, -6, 2, 2);
    painter->drawEllipse(4, -6, 2, 2);
    
    // 绘制微笑
    painter->setPen(QPen(Qt::black, 2));
    painter->drawArc(-5, 2, 10, 8, 0, 180 * 16);
}
