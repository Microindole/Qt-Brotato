#include "player.h"
#include <QBrush>
#include <QPen>
#include <QRandomGenerator>
#include <algorithm>
#include <cmath>

Player::Player()
    : QObject()
    , QGraphicsEllipseItem()
    , maxHealth(60)
    , health(60)
    , speed(2.8f)
    , level(1)
    , experience(0)
    , expToNextLevel(100)
    , healthRegen(0.0f)
    , attackPower(10)
    , armor(0)
    , coins(0)
    , animationCounter(0.0)
    , facingRight(true) // <-- 初始化朝向为右
    , moving(false)
{
    // 加载图片资源
    bodyPixmap.load(":/images/quanneng.png");
    footPixmap.load(":/images/foot.png");
    
    // 设置碰撞区域 - 基于身体和脚部的组合大小
    if (!bodyPixmap.isNull()) {
        qreal bodyWidth = bodyPixmap.width();
        qreal bodyHeight = bodyPixmap.height();
        
        qreal totalHeight = bodyHeight;
        if (!footPixmap.isNull()) {
            totalHeight += footPixmap.height() * 0.6; // 脚部与身体有部分重叠
        }
        
        qreal collisionWidth = bodyWidth * 0.8;
        qreal collisionHeight = totalHeight * 0.8;
        
        setRect(-collisionWidth / 2.0, -collisionHeight / 2.0, collisionWidth, collisionHeight);
    } else {
        setRect(-15, -15, 30, 30);
    }
    
    animationCounter = QRandomGenerator::global()->generateDouble() * 2 * 3.14159;
}

void Player::setFacingDirection(bool right)
{
    facingRight = right;
}

QRectF Player::boundingRect() const
{
    // 这个函数必须返回一个足够大的、居中的矩形，
    // 以包含所有动画状态下的身体、脚和生命条。

    // 估算基础尺寸
    qreal bodyW = bodyPixmap.isNull() ? 30 : bodyPixmap.width();
    qreal bodyH = bodyPixmap.isNull() ? 30 : bodyPixmap.height();
    
    // 增加足够的安全边距来容纳动画（缩放、浮动）和附加元素（脚、生命条）
    qreal padding = 40.0; 
    qreal visualWidth = bodyW + padding;
    qreal visualHeight = bodyH + padding;

    // 返回一个以原点(0,0)为中心的、足够大的矩形
    return QRectF(-visualWidth / 2, -visualHeight / 2, visualWidth, visualHeight);
}

void Player::advance(int phase)
{
    if (phase == 0) return;

    // 无论是否移动，都让动画计数器自增，实现持续呼吸
    animationCounter += 0.12;

    // 调用 update() 来请求重绘，从而绘制新的动画帧。
    // 这是必需的，因为当玩家静止时，setPos() 不会被调用，
    // 因此不会自动触发重绘。
    update();
}

void Player::takeDamage(int damage)
{
    // 护甲减伤逻辑
    int reducedDamage = damage * (100 - armor) / 100;
    health -= reducedDamage;
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

void Player::gainExperience(int exp)
{
    experience += exp;
    
    // 检查是否可以升级
    while (experience >= expToNextLevel) {
        levelUp();
    }
}

void Player::levelUp()
{
    experience -= expToNextLevel;
    level++;
    
    // 升级时提升属性
    maxHealth += 10;        
    health = maxHealth;     
    attackPower += 2;       
    speed += 0.1f;          
    increaseAttackRange(1.1f); 

    // 计算下一级所需经验
    expToNextLevel = calculateExpForLevel(level + 1);
    emit levelUpOccurred();
}

void Player::regenerateHealth()
{
    if (healthRegen > 0 && health < maxHealth) {
        heal(static_cast<int>(healthRegen));
    }
}

int Player::calculateExpForLevel(int targetLevel)
{
    // 每级所需经验递增公式：基础经验 * 等级系数
    return 100 * targetLevel;
}

void Player::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    painter->save();

    // 根据朝向翻转画布
    if (!facingRight) {
        painter->scale(-1, 1);
    }

    // 根据移动状态计算不同的动画参数
    qreal bodyBounce, scaleX, scaleY;

    if (moving) {
        // 移动时：身体的起伏和缩放与脚部摆动频率（更快）同步
        qreal moveAnimationTime = animationCounter * 2.5;
        bodyBounce = 3.0 * sin(moveAnimationTime);
        scaleX = 1.0 + 0.04 * sin(moveAnimationTime);
        scaleY = 1.0 - 0.04 * sin(moveAnimationTime);
    } else {
        // 静止时：使用原有的较慢的“呼吸”效果
        bodyBounce = 2.0 * sin(animationCounter);
        scaleX = 1.0 + 0.05 * sin(animationCounter);
        scaleY = 1.0 - 0.05 * sin(animationCounter);
    }


    qreal bodyW = bodyPixmap.width();
    qreal bodyH = bodyPixmap.height();

    // 先绘制脚部，并应用浮动效果
    if (!footPixmap.isNull()) {
        qreal footScale = 2;
        qreal footW = footPixmap.width() * footScale;
        qreal footH = footPixmap.height() * footScale;

        qreal footSeparation = bodyW * 0.25; 
        // 脚的Y坐标应用 bodyBounce，使其随身体一起上下浮动
        qreal footY = bodyH * 0.3 + bodyBounce; 

        qreal swing = 0;
        if (moving) {
            // 脚的摆动频率使用更快的动画时间
            swing = 20 * std::sin(animationCounter * 2.5);
        }

        // 左脚
        painter->save();
        painter->translate(-footSeparation, footY);
        painter->rotate(15 + swing);
        painter->drawPixmap(QRectF(-footW / 2, 0, footW, footH), footPixmap, footPixmap.rect());
        painter->restore();

        // 右脚
        painter->save();
        painter->translate(footSeparation, footY);
        painter->rotate(-15 - swing);
        painter->drawPixmap(QRectF(-footW / 2, 0, footW, footH), footPixmap, footPixmap.rect());
        painter->restore();
    }

    // 后绘制身体，计算一个缩放后的矩形来绘制
    qreal newWidth = bodyW * scaleX;
    qreal newHeight = bodyH * scaleY;
    // 身体的位置也应用 bodyBounce
    QRectF scaledBodyRect(-newWidth / 2.0, -newHeight / 2.0 + bodyBounce, newWidth, newHeight);
    painter->drawPixmap(scaledBodyRect, bodyPixmap, bodyPixmap.rect());

    painter->restore();

    // 最后绘制生命条（在所有变换之外）
    if (showHealthBar && health < maxHealth) {
        qreal bodyH = bodyPixmap.height();
        qreal bodyW = bodyPixmap.width();
        qreal healthBarY = -bodyH / 2.0 - 12;
        qreal healthBarWidth = bodyW * 0.9;
        qreal healthBarX = -healthBarWidth / 2.0;

        painter->setBrush(QBrush(Qt::black));
        painter->setPen(QPen(Qt::white, 1));
        painter->drawRect(healthBarX, healthBarY, healthBarWidth, 5);

        if (health > 0) {
            float healthPercent = static_cast<float>(health) / maxHealth;
            QColor healthColor = (healthPercent > 0.6f) ? Qt::green : (healthPercent > 0.3f) ? Qt::yellow : Qt::red;
            painter->setBrush(QBrush(healthColor));
            painter->setPen(Qt::NoPen);
            painter->drawRect(healthBarX + 1, healthBarY + 1, (healthBarWidth - 2) * healthPercent, 3);
        }
    }
}

void Player::setMoving(bool m)
{
    moving = m;
}

void Player::increaseAttackRange(float multiplier)
{
    attackRange *= multiplier; // 根据倍数提升攻击距离
    if (attackRange < 0) {
        attackRange = 0; // 确保攻击距离不会变成负值
    }
}

void Player::increaseMaxHealth(int amount)
{
    maxHealth += amount;
    heal(amount); // 增加最大生命值时，也恢复等量的当前生命值
}

void Player::increaseAttackPower(int amount)
{
    attackPower += amount;
}

void Player::increaseSpeed(float amount)
{
    speed += amount;
}

void Player::increaseHealthRegen(float amount)
{
    healthRegen += amount;
}

int Player::getCoins() const
{
    return coins;
}

void Player::addCoins(int amount)
{
    if (amount > 0) {
        coins += amount;
    }
}

