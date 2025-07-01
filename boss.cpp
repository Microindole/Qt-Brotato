#include "boss.h"
#include "resourcemanager.h"
#include <QTimer>
#include <QPainter>

Boss::Boss(int waveNumber, QObject *parent)
    // 调用基类的默认构造函数
    : Enemy(parent),
    m_currentState(State::Chasing),
    m_isEnraged(false)
{
    pixmap = ResourceManager::instance().getPixmap(":/images/boss.png");
    baseSpeed = 0.6f + (waveNumber / 10.0f) * 0.1f;
    damage = 25 + waveNumber * 1.8;
    maxHealth = 200 + waveNumber * 50;
    health = maxHealth;
    experienceValue = 80 + waveNumber * 15;

    qreal scale = 0.3;
    setRect(-pixmap.width() * scale / 2.0, -pixmap.height() * scale / 2.0, pixmap.width() * scale, pixmap.height() * scale);
    setZValue(2);

    m_attackCycleTimer = new QTimer(this);
    connect(m_attackCycleTimer, &QTimer::timeout, this, &Boss::performAttackCycle);
    m_attackCycleTimer->start(3000); // 初始攻击间隔
}

void Boss::moveTowards(const QPointF &target)
{
    m_playerPosition = target;
    // 根据当前状态决定如何移动
    switch (m_currentState) {
    case State::Chasing:
    {
        QPointF direction = target - pos();
        qreal distance = QLineF(pos(), target).length();
        if (distance > 150) { // 保持一定距离
            setPos(pos() + (direction / distance) * baseSpeed);
        }
        setFacingDirection(direction.x() > 0);
        break;
    }
    case State::Dashing:
    {
        QPointF direction = m_dashTarget - pos();
        qreal distance = QLineF(pos(), m_dashTarget).length();
        qreal dashSpeed = baseSpeed * 5.0;
        if (distance > 1.0) {
            setPos(pos() + (direction / distance) * dashSpeed);
        }
        break;
    }
    case State::Attacking:
        // 攻击时不移动
        break;
    }
}

void Boss::performAttackCycle()
{
    if (m_currentState != State::Chasing) return;
    if (!m_isEnraged) {
        // 阶段一攻击：三连发
        m_currentState = State::Attacking;
        // 始终以缓存的玩家位置为目标
        QTimer::singleShot(0, [this]() { emit fireBullet(pos(), m_playerPosition); });
        QTimer::singleShot(250, [this]() { emit fireBullet(pos(), m_playerPosition); });
        QTimer::singleShot(500, [this]() {
            emit fireBullet(pos(), m_playerPosition);
            m_currentState = State::Chasing;
            m_attackCycleTimer->start(3500);
        });
    } else {
        // 阶段二攻击：向玩家最后的位置冲锋
        startDashAttack(m_playerPosition);
    }
}

void Boss::startDashAttack(const QPointF &target)
{
    m_currentState = State::Dashing;
    m_dashTarget = target;
    // 冲锋计时，0.8秒后强制结束
    QTimer::singleShot(800, this, &Boss::finishDash);
}

void Boss::finishDash()
{
    if (m_currentState != State::Dashing) return; // 防止重复触发

    m_currentState = State::Attacking;
    // 发射弹幕新星信号，例如发射12颗子弹
    emit fireRadialShot(18);

    // 弹幕结束后恢复
    QTimer::singleShot(500, [this]() {
        m_currentState = State::Chasing;
        m_attackCycleTimer->start(3000);
    });
}

void Boss::takeDamage(int dmg)
{
    Enemy::takeDamage(dmg); // 调用基类受伤函数

    // 检查是否进入狂暴阶段
    if (!m_isEnraged && (float(health) / maxHealth) <= 0.5f) {
        enterEnragedState();
    }
}

void Boss::enterEnragedState()
{
    m_isEnraged = true;
    baseSpeed *= 1.5; // 移速增加50%
    // 立即触发一次攻击循环
    performAttackCycle();
}

void Boss::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 调用基类的paint函数来绘制血条等通用元素
    Enemy::paint(painter, option, widget);

    // 如果是狂暴状态，可以绘制一个红色闪烁效果
    if (m_isEnraged) {
        // 使用animationCounter制造闪烁效果
        int alpha = 100 + 80 * sin(animationCounter * 4);
        QColor redWithAlpha(255, 0, 0, alpha);
        painter->setPen(QPen(redWithAlpha, 5));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(boundingRect().center(), boundingRect().width() / 2 * 0.8f, boundingRect().height() / 2 * 0.8f);
    }
}
