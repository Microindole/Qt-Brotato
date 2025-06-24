#include "player.h"
#include <QBrush>
#include <QPen>
#include <QRandomGenerator>
#include <algorithm>
#include <cmath>

Player::Player()
    : QGraphicsEllipseItem()
    , maxHealth(60)
    , health(60)
    , speed(2.8f)
    , level(1)
    , experience(0)
    , expToNextLevel(100)
    , healthRegen(0.0f)
    , attackPower(10)
    , armor(0)
    , animationCounter(0.0)
    , facingRight(true) // <-- 初始化朝向为右
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

    // 更新动画计数器，产生平滑的动画效果
    animationCounter += 0.12; // 比敌人稍快的动画速度，让玩家角色更有活力
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
    maxHealth += 10;        // 每级增加10点最大生命值
    health = maxHealth;     // 升级时回满血
    attackPower += 2;       // 每级增加2点攻击力
    speed += 0.1f;          // 每级增加0.1移动速度
    
    // 计算下一级所需经验
    expToNextLevel = calculateExpForLevel(level + 1);
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
    
    painter->save(); // 保存状态，以便后续恢复

    // 核心改动：根据朝向翻转整个画布
    if (!facingRight) {
        painter->scale(-1, 1);
    }

    // --- 接下来的所有绘制都会在可能被翻转的坐标系中进行 ---

    if (bodyPixmap.isNull()) {
        painter->setBrush(QBrush(QColor(139, 69, 19)));
        painter->setPen(QPen(Qt::black, 2));
        painter->drawEllipse(-15, -15, 30, 30);
        painter->restore(); // 别忘了在返回前恢复状态
        return;
    }
    
    // --- 动画计算 ---
    qreal bodyBounce = 2.0 * sin(animationCounter);
    qreal bodyScaleX = 1.0 + 0.02 * sin(animationCounter * 0.8);
    qreal bodyScaleY = 1.0 + 0.015 * cos(animationCounter * 0.8);
    qreal footSwing = 3.0 * sin(animationCounter * 1.5);
    qreal footBounce = 1.5 * cos(animationCounter * 3.0);
    
    // --- 定位与绘制 ---
    qreal bodyW = bodyPixmap.width() * bodyScaleX;
    qreal bodyH = bodyPixmap.height() * bodyScaleY;

    // --- 绘制脚部 ---
    if (!footPixmap.isNull()) {
        qreal footScale = 3; // 使用你设置的大尺寸
        qreal footW = footPixmap.width() * footScale;
        qreal footH = footPixmap.height() * footScale;
        
        qreal footY = (bodyH / 2) - (footH * 0.7) + footBounce;
        
        // 核心改动：调整双脚间距，让它们分开站立
        qreal footSeparation = bodyW * 0.25; // 脚的中心点到身体中轴的距离

        // 绘制“左”脚 (在角色看来是左脚)
        QRectF leftFootRect(-footSeparation - footW / 2 + footSwing, footY, footW, footH);
        painter->drawPixmap(leftFootRect, footPixmap, footPixmap.rect());
        
        // 绘制“右”脚 (镜像)
        painter->save();
        painter->translate(footSeparation - footSwing, footY + footH / 2);
        painter->scale(-1, 1);
        painter->drawPixmap(QRectF(-footW / 2, -footH / 2, footW, footH), footPixmap, footPixmap.rect());
        painter->restore();
    }
    
    // --- 绘制身体 ---
    QRectF bodyRect(-bodyW / 2.0, -bodyH / 2.0 + bodyBounce, bodyW, bodyH);
    painter->drawPixmap(bodyRect, bodyPixmap, bodyPixmap.rect());
    
    painter->restore(); // 恢复画布状态，取消翻转，以便后续绘制UI元素

    // --- 在正常坐标系中绘制生命条 (这样文字不会被翻转) ---
    if (health < maxHealth) {
        qreal healthBarY = bodyRect.top() - 12;
        qreal healthBarWidth = bodyRect.width() * 0.9;
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
