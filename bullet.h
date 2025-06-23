#ifndef BULLET_H
#define BULLET_H

#include <QGraphicsEllipseItem>
#include <QPointF>
#include <QPainter>

class Bullet : public QGraphicsEllipseItem
{
public:
    Bullet(const QPointF &startPos, const QPointF &targetPos);

    QRectF boundingRect() const override;

    void move();
    bool isOutOfBounds(const QRectF &bounds) const;
    
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QPointF velocity;
    float speed;
    int damage;
};

#endif // BULLET_H
