#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsEllipseItem>
#include <QPointF>
#include <QPainter>
#include <QPixmap>
#include <QObject>
#include <QGraphicsScene>

class Enemy : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

public:
    enum EnemyType {
        Common_Type1, // 普通近战1
        Common_Type2, // 普通近战2
        Charger,      // 冲刺怪
        Ranged        // 远程怪
    };

    Enemy(int waveNumber = 1);
    ~Enemy();
    
    QRectF boundingRect() const override;
    
    void moveTowards(const QPointF &target);
    int getDamage() const;
    int getExperienceValue() const { return experienceValue; }
    
    void takeDamage(int dmg);
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    bool isDead() const { return health <= 0; }
    bool showHealthBar = true;

    EnemyType getType() const { return type; }
    
    void advance(int phase) override;

signals:
    // 远程怪请求发射子弹时发出此信号
    void fireBullet(const QPointF& from, const QPointF& to);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    // 状态机，用于冲刺怪
    enum ChargeState {
        Idle,       // 正常移动
        Preparing,  // 准备冲刺（例如：停顿、变色）
        Charging,   // 正在冲刺
        Cooldown    // 冲刺后冷却
    };

    // 基础属性
    EnemyType type;
    float baseSpeed;
    int damage;
    int health;
    int maxHealth;
    int waveLevel;
    int experienceValue;

    // 视觉和动画
    QPixmap pixmap;
    double animationCounter;
    bool facingRight = true;

    // 冲刺怪相关
    ChargeState chargeState = Idle;
    QTimer* chargeTimer;
    QPointF chargeTarget;
    qreal chargeSpeed;

    // 远程怪相关
    QTimer* rangedAttackTimer;
    float preferredDistance = 250.0f;

public:
    void setFacingDirection(bool right); // 新增：设置朝向的方法
};

#endif // ENEMY_H
