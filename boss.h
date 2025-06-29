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

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private slots:
    void performSpecialAttack();

private:
    QTimer* m_attackPatternTimer;
    int m_attackPhase;
};

#endif // BOSS_H
