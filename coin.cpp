#include "coin.h"
#include <QRandomGenerator>
#include <cmath>

Coin::Coin(QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent)
{
    // 设置金币价值
    value = QRandomGenerator::global()->bounded(1, 6); // 随机掉落1-5个金币

    // 加载金币图片，你需要提供一个图片资源，路径为 :/images/coin.png
    setPixmap(QPixmap(":/images/coin.png"));

    // 金币在最底层渲染
    setZValue(-1);

    // 随机设置初始动画相位，让金币上下浮动效果错开
    animationCounter = QRandomGenerator::global()->generateDouble() * 2 * 3.14159;
    // initialY = 0; // 该物品的原点在中心
}

int Coin::getValue() const
{
    return value;
}

void Coin::advance(int phase)
{
    // advance(0) 用于准备工作，advance(1) 用于执行。我们只在执行阶段做动画。
    if (phase == 0) return;

    // 在金币第一次执行动画时，捕获它被放置好的Y坐标
    if (!isInitialPositionSet) {
        initialY = y(); // 获取当前的Y坐标作为浮动的中心
        isInitialPositionSet = true;
    }

    // 更新动画计数器
    animationCounter += 0.1;

    // 计算Y方向的微小浮动
    qreal bobbingOffset = 3 * sin(animationCounter);

    // 设置新的Y坐标。注意我们使用 setY 而不是 setPos，因为我们只改变局部Y坐标
    setY(initialY + bobbingOffset);
}
