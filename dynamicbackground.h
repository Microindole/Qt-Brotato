#ifndef DYNAMICBACKGROUND_H
#define DYNAMICBACKGROUND_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include <cmath>


class DynamicBackgroundWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DynamicBackgroundWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    /**
     * @brief 定时器调用的槽，用于更新动画状态。
     */
    void updateAnimation();

private:
    QPixmap m_farBackgroundPixmap; // 远景层 (来自 bg2.png)
    QPixmap m_midGroundPixmap;     // 中景层 (来自 bg3.png)
    QPixmap m_foregroundPixmap;    // 前景层 (来自 bg1.png)
    QColor m_centerColor;        // 渐变中心颜色
    QColor m_edgeColor;          // 渐变边缘颜色

    double m_animationCounter;   // 用于计算动画偏移量的计数器
};

#endif // DYNAMICBACKGROUND_H
