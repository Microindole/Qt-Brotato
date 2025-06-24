#include "gamewidget.h"
#include "ui_gamewidget.h"
#include "enemy.h"
#include "bullet.h"
#include <QRandomGenerator>
#include <QLineF>
#include <limits>

void GameWidget::updateGame()
{
    if (!gameRunning || gamePaused || !player) return;

    movePlayer();

    for (Enemy *enemy : enemies) {
        enemy->moveTowards(player->pos());
    }
    for (Bullet *bullet : bullets) {
        bullet->move();
    }

    checkCollisions();
    cleanupObjects();
    updateWave();
    updateUI();

    gameScene->advance(); 

    frameCount++;
    if (fpsTimer.elapsed() > 1000) {
        float fps = frameCount / (fpsTimer.elapsed() / 1000.0f);
        ui->fpsLabel->setText(QString("FPS: %1").arg(qRound(fps)));
        fpsTimer.restart();
        frameCount = 0;
    }
}

void GameWidget::movePlayer()
{
    QPointF pos = player->pos();
    const float speed = player->getSpeed();

    if (pressedKeys.contains(Qt::Key_W) || pressedKeys.contains(Qt::Key_Up))    pos.setY(pos.y() - speed);
    if (pressedKeys.contains(Qt::Key_S) || pressedKeys.contains(Qt::Key_Down))  pos.setY(pos.y() + speed);
    if (pressedKeys.contains(Qt::Key_A) || pressedKeys.contains(Qt::Key_Left))  pos.setX(pos.x() - speed);
    if (pressedKeys.contains(Qt::Key_D) || pressedKeys.contains(Qt::Key_Right)) pos.setX(pos.x() + speed);

    pos.setX(qBound(15.0, pos.x(), gameScene->width() - 15.0));
    pos.setY(qBound(15.0, pos.y(), gameScene->height() - 15.0));
    player->setPos(pos);
}

void GameWidget::spawnEnemy()
{
    int enemiesToSpawn = 1 + QRandomGenerator::global()->bounded(qMax(1, wave / 3));
    enemiesToSpawn = qMin(enemiesToSpawn, 5);

    QRectF visibleRect = gameScene->sceneRect();
    float spawnMargin = 30.0f;

    for (int i = 0; i < enemiesToSpawn; ++i) {
        Enemy *enemy = new Enemy(wave);
        int side = QRandomGenerator::global()->bounded(4);
        QPointF pos;

        switch (side) {
            case 0: pos = QPointF(QRandomGenerator::global()->bounded(static_cast<int>(visibleRect.width())), visibleRect.top() - spawnMargin); break;
            case 1: pos = QPointF(visibleRect.right() + spawnMargin, QRandomGenerator::global()->bounded(static_cast<int>(visibleRect.height()))); break;
            case 2: pos = QPointF(QRandomGenerator::global()->bounded(static_cast<int>(visibleRect.width())), visibleRect.bottom() + spawnMargin); break;
            case 3: pos = QPointF(visibleRect.left() - spawnMargin, QRandomGenerator::global()->bounded(static_cast<int>(visibleRect.height()))); break;
        }

        enemy->setPos(pos);
        gameScene->addItem(enemy);
        enemies.append(enemy);
    }
}

void GameWidget::shootBullets()
{
    if (enemies.isEmpty() || !player) return;

    Enemy *nearestEnemy = nullptr;
    float minDistance = std::numeric_limits<float>::max();
    for (Enemy *enemy : enemies) {
        float distance = QLineF(player->pos(), enemy->pos()).length();
        if (distance < minDistance) {
            minDistance = distance;
            nearestEnemy = enemy;
        }
    }

    if (nearestEnemy && minDistance < 300) {
        Bullet *bullet = new Bullet(player->pos(), nearestEnemy->pos());
        gameScene->addItem(bullet);
        bullets.append(bullet);
        playShootSound();
    }
}

void GameWidget::updateWave()
{
    if (enemiesKilled >= wave * 5 + 6) {
        wave++;
        enemiesKilled = 0;
        
        int spawnInterval = qMax(600, 2000 - (wave - 1) * 140);
        enemySpawnTimer->setInterval(spawnInterval);
        
        int shootInterval = qMax(300, 600 - (wave - 1) * 10);
        shootTimer->setInterval(shootInterval);
    }
}