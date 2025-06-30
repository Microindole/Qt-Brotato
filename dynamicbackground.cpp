#include "dynamicbackground.h"
#include <QPainter>
#include <QTimer>
#include <cmath>

DynamicBackgroundWidget::DynamicBackgroundWidget(QWidget *parent)
    : QWidget(parent), m_animationCounter(0.0)
{
    // 定义背景的渐变颜色
    m_centerColor = QColor(85, 75, 55);
    m_edgeColor = QColor(28, 28, 28);

    // 加载所有图层
    m_farBackgroundPixmap.load(":/images/bg2.png");
    m_midGroundPixmap.load(":/images/bg3.png");
    m_foregroundPixmap.load(":/images/bg1.png");

    // 设置动画定时器
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DynamicBackgroundWidget::updateAnimation);
    timer->start(33); // 大约 30 FPS

    // 确保控件可以绘制半透明内容
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

void DynamicBackgroundWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QRect widgetRect = this->rect();

    // 1. 绘制最底层的径向渐变
    QRadialGradient gradient(widgetRect.center(), widgetRect.width() / 1.8);
    gradient.setColorAt(0, m_centerColor);
    gradient.setColorAt(1, m_edgeColor);
    painter.fillRect(widgetRect, gradient);

    // --- 修复逻辑：使用超扫描（Overscan）技术绘制图层 ---
    auto drawLayer = [&](const QPixmap &pixmap, double speed, double amplitude, double opacity = 1.0, bool fixedToBottom = false) {
        if (pixmap.isNull()) return;

        double offsetX = amplitude * sin(m_animationCounter * speed);
        
        // 计算一个比控件实际宽度更宽的绘图区域，确保边缘总在屏幕外
        double overscanWidth = widgetRect.width() + 2 * amplitude;
        double scaledHeight = overscanWidth * pixmap.height() / pixmap.width();
        
        double yPos = fixedToBottom ? (widgetRect.height() - scaledHeight * widgetRect.width() / overscanWidth) : (widgetRect.height() - scaledHeight) / 2.0;

        // 目标矩形比控件宽，并根据偏移量移动
        QRectF targetRect(offsetX - amplitude, yPos, overscanWidth, scaledHeight);

        painter.setOpacity(opacity);
        painter.drawPixmap(targetRect, pixmap, pixmap.rect());
        painter.setOpacity(1.0); // 绘制后重置
    };

    // 2. 按顺序绘制所有图层
    drawLayer(m_farBackgroundPixmap, 0.25, 10.0, 0.6);  // 远景 (bg2), 移动最慢
    drawLayer(m_midGroundPixmap, 0.5, 20.0);         // 中景 (bg3)
    drawLayer(m_foregroundPixmap, 1.0, 40.0, 1.0, true); // 前景 (bg1), 移动最快且贴底
}

void DynamicBackgroundWidget::updateAnimation()
{
    m_animationCounter += 0.02;
    if (m_animationCounter > 2 * 3.1415926 * 100) {
        m_animationCounter = 0;
    }
    update();
}
