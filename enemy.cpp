#include "enemy.h"
#include <QBrush>
#include <QPen>
#include <QRandomGenerator>
#include <cmath>

Enemy::Enemy(int waveNumber)
    : QGraphicsEllipseItem(-12, -12, 24, 24)
    , waveLevel(waveNumber)
{
    // 根据波数动态调整敌人属性 - 数值调小
    speed = 0.6f + QRandomGenerator::global()->bounded(100) / 300.0f + (waveNumber - 1) * 0.05f; // 基础速度降低，成长减缓
    damage = 8 + (waveNumber - 1) * 2; // 基础伤害降低，每波增长减少
    maxHealth = 50 + (waveNumber - 1) * 10; // 基础血量降低，每波增长减少
    health = maxHealth;
    
    // 限制最大属性，防止后期过于变态
    speed = qMin(speed, 2.0f);      // 最大速度降低
    damage = qMin(damage, 30);      // 最大伤害降低
    maxHealth = qMin(maxHealth, 150); // 最大血量降低
    health = maxHealth;
}

QRectF Enemy::boundingRect() const
{
    // 扩大边界以包含血条
    return QRectF(-17, -20, 34, 40);
}

void Enemy::moveTowards(const QPointF &target)
{
    QPointF currentPos = pos();
    QPointF direction = target - currentPos;
    
    float distance = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
    
    if (distance > 0) {
        direction /= distance;
        setPos(currentPos + direction * speed);
    }
}

void Enemy::takeDamage(int dmg)
{
    health -= dmg;
    if (health < 0) health = 0;
}

void Enemy::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    // 绘制敌人主体
    painter->setBrush(QBrush(QColor(220, 20, 20)));
    painter->setPen(QPen(Qt::darkRed, 2));
    painter->drawEllipse(-12, -12, 24, 24);
    
    // 绘制触手
    painter->setPen(QPen(Qt::darkRed, 2));
    for (int i = 0; i < 8; i++) {
        float angle = i * 45.0f * M_PI / 180.0f;
        float x1 = 12 * cos(angle);
        float y1 = 12 * sin(angle);
        float x2 = 16 * cos(angle);
        float y2 = 16 * sin(angle);
        painter->drawLine(x1, y1, x2, y2);
    }
    
    // 绘制愤怒的眼睛
    painter->setBrush(QBrush(Qt::white));
    painter->setPen(QPen(Qt::black, 1));
    painter->drawEllipse(-6, -6, 4, 4);
    painter->drawEllipse(2, -6, 4, 4);
    
    painter->setBrush(QBrush(Qt::red));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(-5, -5, 2, 2);
    painter->drawEllipse(3, -5, 2, 2);
    
    // 绘制愤怒的嘴巴
    painter->setPen(QPen(Qt::black, 2));
    painter->drawLine(-4, 2, 4, 2);
    painter->drawLine(-2, 2, -2, 6);
    painter->drawLine(0, 2, 0, 6);
    painter->drawLine(2, 2, 2, 6);
    
    // 绘制血条背景
    painter->setBrush(QBrush(Qt::black));
    painter->setPen(QPen(Qt::white, 1));
    painter->drawRect(-15, -18, 30, 4);
    
    // 绘制血条
    if (health > 0) {
        float healthPercent = static_cast<float>(health) / maxHealth;
        QColor healthColor;
        if (healthPercent > 0.6f) {
            healthColor = Qt::green;
        } else if (healthPercent > 0.3f) {
            healthColor = Qt::yellow;
        } else {
            healthColor = Qt::red;
        }
        
        painter->setBrush(QBrush(healthColor));
        painter->setPen(Qt::NoPen);
        painter->drawRect(-14, -17, static_cast<int>(28 * healthPercent), 2);
    }
}
