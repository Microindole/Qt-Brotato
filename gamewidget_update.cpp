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
    if (!player) return;

    QPointF moveDirection(0, 0);
    if (pressedKeys.contains(Qt::Key_W)) moveDirection.ry() -= 1;
    if (pressedKeys.contains(Qt::Key_S)) moveDirection.ry() += 1;
    if (pressedKeys.contains(Qt::Key_A)) {
        moveDirection.rx() -= 1;
        player->setFacingDirection(false);
    }
    if (pressedKeys.contains(Qt::Key_D)) {
        moveDirection.rx() += 1;
        player->setFacingDirection(true);
    }

    if (!moveDirection.isNull()) {
        QVector2D moveVec(moveDirection);
        moveVec.normalize();
        player->setPos(player->pos() + moveVec.toPointF() * player->getSpeed());
    }

    player->setPos(QPointF(qBound(15.0, player->pos().x(), gameScene->width() - 15.0),
                           qBound(15.0, player->pos().y(), gameScene->height() - 15.0)));

    player->setMoving(!moveDirection.isNull());
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
    // 核心修正：将玩家的攻击距离作为初始的最小距离
    float minDistance = player->getAttackRange(); 

    for (Enemy *enemy : enemies) {
        float distance = QLineF(player->pos(), enemy->pos()).length();
        // 寻找在攻击范围内且最近的敌人
        if (distance < minDistance) {
            minDistance = distance;
            nearestEnemy = enemy;
        }
    }

    if (nearestEnemy) {
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