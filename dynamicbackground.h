#ifndef DYNAMICBACKGROUND_H
#define DYNAMICBACKGROUND_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include <cmath>

/**
 * @class DynamicBackgroundWidget
 * @brief 使用 bg1.png 和 bg3.png 创建一个多层动态视差背景。
 *
 * 这个控件在底层绘制一个径向渐变，然后在其上绘制两个
 * 以不同速度水平移动的图像层，从而创造出深度感。
 */
class DynamicBackgroundWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DynamicBackgroundWidget(QWidget *parent = nullptr);

protected:
    /**
     * @brief 重写 paintEvent 以自定义绘制逻辑。
     * @param event 绘图事件。
     */
    void paintEvent(QPaintEvent *event) override;

private slots:
    /**
     * @brief 定时器调用的槽，用于更新动画状态。
     */
    void updateAnimation();

private:
    QPixmap m_monstersPixmap;    // 中景层 (来自 bg3.png)
    QPixmap m_foregroundPixmap;  // 前景层 (来自 bg1.png)
    QColor m_centerColor;        // 渐变中心颜色
    QColor m_edgeColor;          // 渐变边缘颜色

    double m_animationCounter;   // 用于计算动画偏移量的计数器
};

#endif // DYNAMICBACKGROUND_H
