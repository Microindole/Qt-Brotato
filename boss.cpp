#include "boss.h"
#include "resourcemanager.h"
#include <QTimer>
#include <QPainter>

Boss::Boss(int waveNumber, QObject *parent)
    // 调用基类的默认构造函数
    : Enemy(parent)
{
    pixmap = ResourceManager::instance().getPixmap(":/images/boss.png");
    baseSpeed = 0.6f + (waveNumber / 10.0f) * 0.1f;
    damage = 25 + waveNumber;
    maxHealth = 100 + waveNumber * 50;
    health = maxHealth;
    experienceValue = 60 + waveNumber * 10;

    qreal scale = 0.3;


    setRect(-pixmap.width() * scale / 2.0, -pixmap.height() * scale / 2.0, pixmap.width() * scale, pixmap.height() * scale);
    setZValue(2);

    m_attackPhase = 0;
    m_attackPatternTimer = new QTimer(this);
    connect(m_attackPatternTimer, &QTimer::timeout, this, &Boss::performSpecialAttack);
    m_attackPatternTimer->start(3000);
}

void Boss::moveTowards(const QPointF &target)
{
    // Boss的移动逻辑：缓慢地逼近玩家
    QPointF direction = target - pos();
    qreal distance = QLineF(pos(), target).length();
    if (distance > 200) {
        setPos(pos() + (direction / distance) * baseSpeed);
    }
    setFacingDirection(direction.x() > 0);
}

void Boss::performSpecialAttack()
{

    if (m_attackPhase < 3) {
        // 发射信号，让GameWidget创建子弹
        emit fireBullet(pos(), scene()->items().first()->pos());
        m_attackPhase++;
        // 快速连续发射
        m_attackPatternTimer->setInterval(200);
        m_attackPatternTimer->start();
    } else {
        // 三连发结束后，进入CD
        m_attackPhase = 0;
        m_attackPatternTimer->setInterval(3000); // 3秒CD
    }
}

void Boss::takeDamage(int dmg)
{
    // Boss可以有独特的受伤逻辑，比如有减伤护盾
    int finalDamage = qMax(1, dmg - 10);
    Enemy::takeDamage(finalDamage);

    // 可以在这里添加Boss受伤的视觉/音效反馈
}

void Boss::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 调用基类的paint函数来绘制血条等通用元素
    Enemy::paint(painter, option, widget);

    // 在此基础上添加Boss独有的视觉效果，光环
    // painter->save();
    // painter->setPen(QPen(QColor(255, 100, 0, 100), 5));
    // painter->setBrush(Qt::NoBrush);
    // painter->drawEllipse(boundingRect());
    // painter->restore();
}
