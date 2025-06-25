#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <cmath>

// --- 前向声明 ---
namespace Ui {
class Menu;
}
class QShowEvent;


// ===================================================================
// ==          新增：专门用于播放呼吸动画的自定义 Widget           ==
// ===================================================================
class AnimatedCharacterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AnimatedCharacterWidget(QWidget *parent = nullptr)
        : QWidget(parent), animationCounter(0.0)
    {
        // 1. 加载角色图片
        pixmap.load(":/images/brotato_char.png");

        // 2. 设置固定的尺寸策略，使其保持图片大小
        if (!pixmap.isNull()) {
            setFixedSize(pixmap.size());
        }

        // 3. 设置一个定时器来驱动动画的每一帧
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this]() {
            // 类似于 player.cpp 和 enemy.cpp 中的 advance()
            animationCounter += 0.08; // 调整此值可以改变动画速度
            if (animationCounter > 2 * 3.14159 * 100) { // 防止数值过大
                animationCounter = 0;
            }
            update(); // 请求重绘
        });
        timer->start(33); // 大约 30 FPS
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);
        if (pixmap.isNull()) return;

        QPainter painter(this);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        qreal scaleX = 1.0 + 0.02 * sin(animationCounter);
        qreal scaleY = 1.0 - 0.02 * sin(animationCounter);

        // 5. 计算缩放后的新尺寸和位置
        qreal newWidth = pixmap.width() * scaleX;
        qreal newHeight = pixmap.height() * scaleY;
        qreal newX = (width() - newWidth) / 2.0;
        qreal newY = (height() - newHeight) / 2.0;

        QRectF targetRect(newX, newY, newWidth, newHeight);

        // 6. 将图片绘制到计算出的目标矩形中
        painter.drawPixmap(targetRect, pixmap, pixmap.rect());
    }

private:
    QPixmap pixmap;
    double animationCounter;
};
// ===================================================================


class Menu : public QWidget
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = nullptr);
    ~Menu();

signals:
    void startGameRequested();
    void exitGameRequested();

private:
    Ui::Menu *ui;
    // 不再需要动画组和 showEvent
};

#endif // MENU_H