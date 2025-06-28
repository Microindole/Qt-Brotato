#include "gamewidget.h"
#include "ui_gamewidget.h"
#include <QKeyEvent>
#include <QResizeEvent>

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }
    if (event->key() == Qt::Key_Escape) {
        pauseGame();
    } else {
        pressedKeys.insert(event->key());
    }
    QWidget::keyPressEvent(event);
}

void GameWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }
    pressedKeys.remove(event->key());
    QWidget::keyReleaseEvent(event);
}

void GameWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (gameScene) {
        ui->gameView->fitInView(gameScene->sceneRect(), Qt::KeepAspectRatio);
    }
    
    if (pauseWidget && pauseWidget->isVisible()) {
        pauseWidget->resize(this->size());
        pauseWidget->move(0, 0);
    }

    updateUpgradeIndicators();
}
