#ifndef BOSS_H
#define BOSS_H

#include "enemy.h"

class Boss : public Enemy
{
    Q_OBJECT
public:
    explicit Boss(int waveNumber, QObject *parent = nullptr);

    // 重写基类的虚函数来实现Boss独特的行为
    void moveTowards(const QPointF &target) override;
    void takeDamage(int dmg) override;

signals:
    // 新增信号，用于触发弹幕新星攻击
    void fireRadialShot(int bulletCount);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private slots:
    void performAttackCycle();
    void finishDash();

private:
    // Boss状态管理
    enum class State { Chasing, Dashing, Attacking };
    State m_currentState;

    void enterEnragedState();
    void startDashAttack(const QPointF &target);

    QTimer* m_attackCycleTimer;
    bool m_isEnraged; // 狂暴
    QPointF m_dashTarget;
    QPointF m_playerPosition;
};

#endif // BOSS_H
