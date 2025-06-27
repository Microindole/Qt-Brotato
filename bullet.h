#ifndef BULLET_H
#define BULLET_H

#include <QGraphicsEllipseItem>
#include <QPointF>
#include <QPainter>

class Bullet : public QGraphicsEllipseItem
{
public:
    enum Owner {
        PlayerBullet,
        EnemyBullet
    };

    Bullet(const QPointF &startPos, const QPointF &targetPos, Owner owner, int damage, float speed = 10.0f);


    QRectF boundingRect() const override;

    void move();
    bool isOutOfBounds(const QRectF &bounds) const;
    Owner getOwner() const;
    int getDamage() const;
    
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    Owner m_owner;
    QPointF velocity;
    float m_speed;
    int m_damage;
};

#endif // BULLET_H
