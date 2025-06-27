#include "enemy.h"
#include "resourcemanager.h"
#include <QBrush>
#include <QPen>
#include <QRandomGenerator>
#include <cmath>

Enemy::Enemy(int waveNumber)
    : QGraphicsEllipseItem()
    , waveLevel(waveNumber)
{
    // 随机选择一个敌人类型
    type = (QRandomGenerator::global()->bounded(2) == 0) ? Type1 : Type2;

    // 根据类型加载不同的图片和设置不同的属性
    if (type == Type1) {
        pixmap = ResourceManager::instance().getPixmap(":/images/common1.png");
        // 类型1: 基础型，血厚攻高
        speed = 0.8f + (waveNumber - 1) * 0.04f;
        damage = 8 + (waveNumber - 1) * 1;
        maxHealth = 40 + (waveNumber - 1) * 6;
        experienceValue = 8;
    } else { // Type2
        pixmap = ResourceManager::instance().getPixmap(":/images/common2.png");
        // 类型2: 速度更快，但更脆弱
        speed = 1.1f + (waveNumber - 1) * 0.06f;
        damage = 6 + (waveNumber - 1) * 1;
        maxHealth = 30 + (waveNumber - 1) * 5;
        experienceValue = 5;
    }
    health = maxHealth;

    // 怪物在金币之上
    setZValue(0);
    
    // 限制最大属性
    speed = qMin(speed, 3.0f);
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

QRectF Enemy::boundingRect() const
{
    // 获取由 setRect() 设置的核心碰撞矩形
    QRectF coreRect = rect(); 
    // 为动画的最大缩放（5%）和血条（10像素）留出额外空间
    qreal extraW = coreRect.width() * 0.05;
    qreal extraH = coreRect.height() * 0.05;
    return coreRect.adjusted(-extraW, -extraH - 10, extraW, extraH); 
}

void Enemy::moveTowards(const QPointF &target)
{
    QPointF currentPos = pos();
    QPointF direction = target - currentPos;

    float distance = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

    if (distance > 0) {
        direction /= distance;
        setPos(currentPos + direction * speed);

        // 根据目标位置判断左右朝向
        setFacingDirection(direction.x() > 0); // 如果 x > 0，则朝右；否则朝左
    }
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

    // 更新动画计数器以产生平滑的“呼吸”效果
    animationCounter += 0.08; 
    update();
}

void Enemy::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (pixmap.isNull()) return;

    painter->save();

    // 根据朝向翻转画布
    if (!facingRight) {
        painter->scale(-1, 1);
    }

    // 动画缩放 
    qreal scaleX = 1.0 + 0.05 * sin(animationCounter);
    qreal scaleY = 1.0 - 0.05 * sin(animationCounter);

    QRectF originalRect = rect();
    qreal newWidth = originalRect.width() * scaleX;
    qreal newHeight = originalRect.height() * scaleY;
    QRectF scaledRect(-newWidth / 2.0, -newHeight / 2.0, newWidth, newHeight);

    // 在缩放后的矩形内绘制敌人图像
    painter->drawPixmap(scaledRect, pixmap, pixmap.rect());

    painter->restore();

    // 血条
    // 绘制血条，并检查 showHealthBar 标志
    if (showHealthBar) {
        QRectF originalRect = rect();
        qreal healthBarY = scaledRect.top() - 8;
        qreal healthBarWidth = originalRect.width() * 0.9;
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
