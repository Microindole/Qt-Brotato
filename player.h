#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class Player : public QGraphicsEllipseItem
{
public:
    Player();
    
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    void takeDamage(int damage);
    void heal(int amount);
    void resetHealth();
    
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    int health;
    int maxHealth;
};

#endif // PLAYER_H
