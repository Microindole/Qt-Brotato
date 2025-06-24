#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsEllipseItem>
#include <QPointF>
#include <QPainter>
#include <QPixmap>

class Enemy : public QGraphicsEllipseItem
{
public:
    enum EnemyType {
        Type1, // common1.png
        Type2  // common2.png
    };

    Enemy(int waveNumber = 1);
    
    QRectF boundingRect() const override;
    
    void moveTowards(const QPointF &target);
    int getDamage() const { return damage; }
    float getSpeed() const { return speed; }
    
    void takeDamage(int dmg);
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    bool isDead() const { return health <= 0; }
    
    void advance(int phase) override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    float speed;
    int damage;
    int health;
    int maxHealth;
    int waveLevel;

    EnemyType type;
    QPixmap pixmap; // 使用单个 pixmap
    double animationCounter; // 用于动画
};

#endif // ENEMY_H
