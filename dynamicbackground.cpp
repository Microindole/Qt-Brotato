#include "dynamicbackground.h"

DynamicBackgroundWidget::DynamicBackgroundWidget(QWidget *parent)
    : QWidget(parent), m_animationCounter(0.0)
{
    // 1. 定义背景渐变的颜色
    m_centerColor = QColor(85, 75, 55); // 中心的亮色 (类似土黄色)
    m_edgeColor = QColor(28, 28, 28);   // 边缘的暗色 (深灰色)

    // 2. 从资源文件加载图层
    m_monstersPixmap.load(":/images/bg3.png");
    m_foregroundPixmap.load(":/images/bg1.png");

    // 3. 设置动画定时器
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DynamicBackgroundWidget::updateAnimation);
    timer->start(33); // 约 30 FPS

    // 4. 设置属性，允许绘制透明内容
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

void DynamicBackgroundWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QRect widgetRect = this->rect();

    // 1. 绘制底层径向渐变背景
    QRadialGradient gradient(widgetRect.center(), widgetRect.width() / 2.0);
    gradient.setColorAt(0, m_centerColor);
    gradient.setColorAt(1, m_edgeColor);
    painter.fillRect(widgetRect, gradient);

    // 2. 绘制中景怪物层 (bg3.png)
    if (!m_monstersPixmap.isNull()) {
        // 使用 sin 函数创建平滑的来回移动效果
        double monstersOffsetX = 15 * sin(m_animationCounter * 0.5); // 幅度较小，速度较慢
        QRectF monstersRect = widgetRect.adjusted(monstersOffsetX, 0, monstersOffsetX, 0);
        painter.drawPixmap(monstersRect, m_monstersPixmap, m_monstersPixmap.rect());
    }

    // 3. 绘制前景烟雾层 (bg1.png)
    if (!m_foregroundPixmap.isNull()) {
        double foregroundOffsetX = 30 * sin(m_animationCounter); // 幅度较大，速度较快

        // 计算前景的位置和尺寸
        // 我们希望它保持原始高宽比，并固定在底部
        int newHeight = widgetRect.width() * m_foregroundPixmap.height() / m_foregroundPixmap.width();
        int newY = widgetRect.height() - newHeight;

        QRectF foregroundRect(widgetRect.x() + foregroundOffsetX, newY, widgetRect.width(), newHeight);
        painter.drawPixmap(foregroundRect, m_foregroundPixmap, m_foregroundPixmap.rect());
    }
}

void DynamicBackgroundWidget::updateAnimation()
{
    m_animationCounter += 0.02;
    // 周期性重置以防数值溢出
    if (m_animationCounter > 2 * 3.1415926 * 100) {
        m_animationCounter = 0;
    }
    update(); // 请求重绘
}
