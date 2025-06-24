#include "player.h"
#include <QBrush>
#include <QPen>
#include <QRandomGenerator>
#include <algorithm>

Player::Player()
    : QGraphicsEllipseItem(-15, -15, 30, 30)
    , maxHealth(60)         // 玩家最大血量
    , health(60)            // 当前血量
    , speed(2.8f)           // 移动速度
    , level(1)              // 初始等级为1
    , experience(0)         // 初始经验为0
    , expToNextLevel(100)   // 升到2级需要100经验
    , healthRegen(0.0f)     // 初始生命再生为0
    , attackPower(10)       // 初始攻击力
    , armor(0)              // 初始护甲为0
{
    // 设置玩家的样式
    setBrush(QBrush(QColor(139, 69, 19))); // 土豆色
    setPen(QPen(Qt::black, 2));
}

void Player::takeDamage(int damage)
{
    // 护甲减伤逻辑（之后可以在这里实现）
    // int reducedDamage = damage * (100 - armor) / 100;
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
