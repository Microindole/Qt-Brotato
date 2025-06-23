#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsEllipseItem>
#include <QPainter>

class Player : public QGraphicsEllipseItem
{
public:
    Player();
    
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    float getSpeed() const { return speed; }
    
    void takeDamage(int damage);
    void heal(int amount);
    
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    int health;
    int maxHealth;
    float speed;  // 确保这个成员变量存在
};

#endif // PLAYER_H
