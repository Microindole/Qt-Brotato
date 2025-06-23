#include "player.h"
#include <QBrush>
#include <QPen>
#include <QRandomGenerator>

Player::Player()
    : QGraphicsEllipseItem(-15, -15, 30, 30)
    , maxHealth(60)    // 玩家最大血量从80降低到60
    , health(60)       // 当前血量
    , speed(2.8f)      // 移动速度从3.5降低到2.8（如果你使用了Player类的speed属性）
{
    // 设置玩家的样式
    setBrush(QBrush(QColor(139, 69, 19))); // 土豆色
    setPen(QPen(Qt::black, 2));
}

void Player::takeDamage(int damage)
{
    health -= damage;
    if (health < 0) {
        health = 0;
    }
}

void Player::heal(int amount)
{
    health += amount;
    if (health > maxHealth) {
        health = maxHealth;
    }
}

void Player::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    // 绘制玩家主体（土豆形状）
    painter->setBrush(QBrush(QColor(139, 69, 19))); // 土豆色
    painter->setPen(QPen(Qt::black, 2));
    painter->drawEllipse(-15, -15, 30, 30);
    
    // 绘制眼睛
    painter->setBrush(QBrush(Qt::white));
    painter->setPen(QPen(Qt::black, 1));
    painter->drawEllipse(-8, -8, 5, 5);
    painter->drawEllipse(3, -8, 5, 5);
    
    // 绘制瞳孔
    painter->setBrush(QBrush(Qt::black));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-6, -6, 2, 2);
    painter->drawEllipse(5, -6, 2, 2);
    
    // 绘制嘴巴
    painter->setPen(QPen(Qt::black, 2));
    painter->drawArc(-6, 2, 12, 8, 0, 180 * 16);
    
    // 绘制土豆上的小坑点
    painter->setBrush(QBrush(QColor(120, 60, 15)));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-10, 5, 3, 2);
    painter->drawEllipse(8, -2, 2, 3);
    painter->drawEllipse(-5, 10, 2, 2);
}
