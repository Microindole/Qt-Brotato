#include "enemy.h"
#include "resourcemanager.h"
#include <QBrush>
#include <QPen>
#include <QRandomGenerator>
#include <QTimer>
#include <cmath>
#include <QGraphicsScene>

Enemy::Enemy(int waveNumber)
    : QObject()
    , QGraphicsEllipseItem()
    , waveLevel(waveNumber)
    , chargeTimer(nullptr)
    , rangedAttackTimer(nullptr)
{
    // 现在有4种敌人类型
    type = static_cast<EnemyType>(QRandomGenerator::global()->bounded(4));

    // 根据类型加载不同的图片和设置不同的属性
    // 根据类型初始化属性
    switch(type) {
        case Common_Type1:
            pixmap = ResourceManager::instance().getPixmap(":/images/common1.png");
            baseSpeed = 0.8f + (waveNumber - 1) * 0.04f;
            damage = 8 + (waveNumber - 1) * 1;
            maxHealth = 40 + (waveNumber - 1) * 6;
            experienceValue = 8;
            break;

        case Common_Type2:
            pixmap = ResourceManager::instance().getPixmap(":/images/common2.png");
            baseSpeed = 1.1f + (waveNumber - 1) * 0.06f;
            damage = 6 + (waveNumber - 1) * 1;
            maxHealth = 30 + (waveNumber - 1) * 5;
            experienceValue = 5;
            break;

        case Charger:
            pixmap = ResourceManager::instance().getPixmap(":/images/charge.png");
            baseSpeed = 0.9f + (waveNumber - 1) * 0.05f;
            damage = 15 + (waveNumber - 1) * 2; // 冲撞伤害更高
            maxHealth = 70 + (waveNumber - 1) * 8;
            experienceValue = 12;
            chargeSpeed = baseSpeed * 12.0; // 冲刺速度是基础速度的4倍

            // 设置冲刺逻辑的定时器
            chargeTimer = new QTimer(this);
            chargeTimer->setSingleShot(true); // 定时器只触发一次
            connect(chargeTimer, &QTimer::timeout, this, [this](){
                if (chargeState == Preparing) {
                    chargeState = Charging;
                    if(scene()) chargeTarget = scene()->items().first()->pos();
                    chargeTimer->start(2000); // 冲刺时间
                } else if (chargeState == Charging) {
                    chargeState = Cooldown;
                    chargeTimer->start(500); // 冲刺冷却
                } else if (chargeState == Cooldown) {
                    chargeState = Idle;
                }
            });
            chargeTimer->start(QRandomGenerator::global()->bounded(1000, 2000)); // 随机延迟后开始第一次准备
            break;

        case Ranged:
            pixmap = ResourceManager::instance().getPixmap(":/images/remote.png");
            baseSpeed = 0.7f + (waveNumber - 1) * 0.03f;
            damage = 5 + (waveNumber - 1) * 1; // 近战伤害较低
            maxHealth = 50 + (waveNumber - 1) * 5;
            experienceValue = 10;

            // 设置远程攻击定时器
            rangedAttackTimer = new QTimer(this);
            connect(rangedAttackTimer, &QTimer::timeout, this, [this](){
                // 当定时器触发，就发射子弹 (目标暂时设为0,0，应在GameWidget中设为玩家位置)
                // 信号将在 advance() 中根据玩家位置发射
            });
            rangedAttackTimer->start(QRandomGenerator::global()->bounded(100, 500)); // 随机的攻击间隔
            break;
    }
    health = maxHealth;
    // 怪物在金币之上
    setZValue(0);
    
    // 限制最大属性
    baseSpeed = qMin(baseSpeed, 3.0f);
    damage = qMin(damage, 30);
    maxHealth = qMin(maxHealth, 350);
    health = maxHealth;

    // 使用图片尺寸来定义碰撞区域，并将原点设置在中心
    if (!pixmap.isNull()) {
        setRect(-pixmap.width() / 2.0, -pixmap.height() / 2.0, pixmap.width(), pixmap.height());
    }

    // 为动画设置一个随机的起始点，防止所有敌人动作同步
    animationCounter = QRandomGenerator::global()->generateDouble() * 2 * 3.14159;
}

Enemy::~Enemy()
{
    if(chargeTimer) { chargeTimer->stop();}
    if(rangedAttackTimer) { rangedAttackTimer->stop();}
}

QRectF Enemy::boundingRect() const
{
    // 获取由 setRect() 设置的核心碰撞矩形
    QRectF coreRect = rect(); 
    // 为动画的最大缩放（5%）和血条（10像素）留出额外空间
    qreal extraW = coreRect.width() * 0.15;
    qreal extraH = coreRect.height() * 0.15;
    return coreRect.adjusted(-extraW, -extraH - 10, extraW, extraH); 
}

void Enemy::moveTowards(const QPointF &target)
{
    // 远程怪有特殊的移动逻辑
    if (type == Ranged) {
        QPointF direction = target - pos();
        qreal distance = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
        if (distance > 0) {
            direction /= distance;
            // 如果离玩家太远，就靠近；如果太近，就后退
            if (distance > preferredDistance + 50) {
                setPos(pos() + direction * baseSpeed);
            } else if (distance < preferredDistance - 50) {
                setPos(pos() - direction * baseSpeed * 0.8); // 后退速度稍慢
            }
        }
        setFacingDirection(direction.x() > 0);
        return;
    }

    // 其他怪物使用普通移动逻辑
    if (chargeState == Charging) {
        // 冲刺时，向预定目标冲刺
        QPointF direction = chargeTarget - pos();
        qreal distance = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
        if (distance > 5) { // 避免抖动
            setPos(pos() + (direction / distance) * chargeSpeed);
        }
    } else if (chargeState == Idle) {
        // 正常状态下追击玩家
        QPointF direction = target - pos();
        qreal distance = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
        if (distance > 0) {
            direction /= distance;
            setPos(pos() + direction * baseSpeed);
        }
        setFacingDirection(direction.x() > 0);
    }
    // Preparing和Cooldown状态下不移动
}

void Enemy::takeDamage(int dmg)
{
    health -= dmg;
    if (health < 0) health = 0;
    update(); // 受伤时立即更新视觉表现（如血条）
}

void Enemy::advance(int phase)
{
    if (phase == 0) return;

    // 冲刺怪的状态检查
    if (type == Charger && chargeState == Idle && !chargeTimer->isActive()) {
        // 当冷却结束且计时器不活动时，进入准备状态
        chargeState = Preparing;
        chargeTimer->start(500); // 准备0.5秒
    }
    // 远程怪的开火逻辑
    if (type == Ranged && !rangedAttackTimer->isActive()) {
        QPointF targetPos = scene()->items().isEmpty() ? QPointF(0,0) : scene()->items().first()->pos();
        emit fireBullet(pos(), targetPos); // 发射信号
        rangedAttackTimer->start(QRandomGenerator::global()->bounded(2500, 4000)); // 重置计时器
    }
    animationCounter += 0.08;
    update();
}

void Enemy::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if (pixmap.isNull()) return;

    painter->save();

    if (!facingRight) {
        painter->scale(-1, 1);
    }

    // 为冲刺怪添加视觉提示
    if (type == Charger && chargeState == Preparing) {
        // 准备时闪烁红色
        painter->setOpacity(0.5 + 0.5 * abs(sin(animationCounter * 5)));
        QPixmap redPixmap = pixmap;
        QPainter p(&redPixmap);
        p.setCompositionMode(QPainter::CompositionMode_SourceIn);
        p.fillRect(redPixmap.rect(), QColor(255, 50, 50, 200));
        p.end();
        painter->drawPixmap(rect(), redPixmap, pixmap.rect());
    } else if (type == Charger && chargeState == Charging) {
        // 冲刺时留下残影
        painter->setOpacity(0.3);
        painter->drawPixmap(rect().translated(10, 0), pixmap, pixmap.rect());
        painter->drawPixmap(rect().translated(-10, 0), pixmap, pixmap.rect());
        painter->setOpacity(1.0);
        painter->drawPixmap(rect(), pixmap, pixmap.rect());
    }
    else {
        // 正常绘制
        qreal scaleX = 1.0 + 0.05 * sin(animationCounter);
        qreal scaleY = 1.0 - 0.05 * sin(animationCounter);
        QRectF scaledRect(-rect().width()*scaleX/2.0, -rect().height()*scaleY/2.0, rect().width()*scaleX, rect().height()*scaleY);
        painter->drawPixmap(scaledRect, pixmap, pixmap.rect());
    }

    painter->restore();

    if (showHealthBar) {
        QRectF r = rect();
        qreal healthBarY = r.top() - 12;
        qreal healthBarWidth = r.width() * 0.9;
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

void Enemy::setFacingDirection(bool right)
{
    facingRight = right;
}

int Enemy::getDamage() const
{
    // 冲刺怪在冲刺状态下造成双倍伤害
    if (type == Charger && chargeState == Charging) {
        return damage * 2;
    }
    return damage;
}
