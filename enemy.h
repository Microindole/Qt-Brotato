#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsEllipseItem>
#include <QPointF>
#include <QPainter>

class Enemy : public QGraphicsEllipseItem
{
public:
    Enemy(int waveNumber = 1);
    
    QRectF boundingRect() const override;
    
    void moveTowards(const QPointF &target);
    int getDamage() const { return damage; }
    float getSpeed() const { return speed; }
    
    void takeDamage(int dmg);
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    bool isDead() const { return health <= 0; }
    
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    float speed;
    int damage;
    int health;
    int maxHealth;
    int waveLevel;
};

#endif // ENEMY_H
