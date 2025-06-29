#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsEllipseItem>
#include <QPointF>
#include <QPainter>
#include <QPixmap>
#include <QObject>
#include <QGraphicsScene>

class QTimer;

class Enemy : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

public:
    enum EnemyType {
        Common_Type1,
        Common_Type2,
        Charger,
        Ranged,
        // 我们会用一个独立的Boss类，所以这里不需要Boss类型
    };
    Q_ENUM(EnemyType)

    // 允许派生类调用
    explicit Enemy(QObject* parent = nullptr);
    // 为普通小怪保留的构造函数
    Enemy(int waveNumber, QObject* parent = nullptr);
    virtual ~Enemy(); // 基类的析构函数应该是虚函数

    QRectF boundingRect() const override;

    // --- 将这些函数设为虚函数，以便Boss可以重写它们 ---
    virtual void moveTowards(const QPointF &target);
    virtual int getDamage() const;
    virtual void takeDamage(int dmg);

    int getExperienceValue() const { return experienceValue; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    bool isDead() const { return health <= 0; }
    bool showHealthBar = true;
    EnemyType getType() const { return type; }

    void advance(int phase) override;

signals:
    void fireBullet(const QPointF& from, const QPointF& to);

protected:
    // paint也应该是虚函数，但QGraphicsItem的paint已经是虚函数了，所以不用显式声明
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // 将成员变量设为 protected，以便派生类(Boss)可以访问
protected:
    void setFacingDirection(bool right);

    enum ChargeState {
        Idle, Preparing, Charging, Cooldown
    };

    EnemyType type;
    float baseSpeed;
    int damage;
    int health;
    int maxHealth;
    int waveLevel;
    int experienceValue;

    QPixmap pixmap;
    double animationCounter;
    bool facingRight = true;

    ChargeState chargeState = Idle;
    QTimer* chargeTimer = nullptr;
    QPointF chargeTarget;
    qreal chargeSpeed = 0;
    QTimer* rangedAttackTimer = nullptr;
    float preferredDistance = 250.0f;
};

#endif // ENEMY_H
