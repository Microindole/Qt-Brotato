#ifndef COIN_H
#define COIN_H

#include <QGraphicsPixmapItem>
#include <QObject>

class Coin : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Coin(QGraphicsItem *parent = nullptr);

    // 获取金币的价值
    int getValue() const;

protected:
    // advance函数用于实现动画
    void advance(int phase) override;

private:
    int value;
    qreal initialY; // 记录初始Y坐标用于浮动动画
    double animationCounter; // 动画计数器
    bool isInitialPositionSet; // 金币标志位
};

#endif // COIN_H
