#include "gamewidget.h"
#include "ui_gamewidget.h"
#include "enemy.h"
#include "boss.h"
#include "bullet.h"
#include "coin.h"
#include "resourcemanager.h"
#include <QRandomGenerator>
#include <QLineF>
#include <QVector2D>

void GameWidget::onPeriodicEffects()
{
    // 如果游戏正在运行且玩家存在
    if (gameRunning && !gamePaused && player) {
        // 调用玩家的生命再生函数
        player->regenerateHealth();
    }
}

void GameWidget::updateGame()
{
    if (!gameRunning || gamePaused || !player) return;

    // 更新波次倒计时
    if (fpsTimer.elapsed() >= 1000) {
        waveTimeLeft--;
        float fps = frameCount / (fpsTimer.elapsed() / 1000.0f);
        ui->fpsLabel->setText(QString("FPS: %1").arg(qRound(fps)));
        fpsTimer.restart();
        frameCount = 0;
    }
    frameCount++;

    movePlayer();

    for (Enemy *enemy : std::as_const(enemies)) {
        enemy->moveTowards(player->pos());
    }
    for (Bullet *bullet : std::as_const(bullets)) {
        bullet->move();
    }

    checkCollisions();
    cleanupObjects();
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
    // 如果Boss已存在，则不再刷普通小怪
    if (m_boss) return;

    // 每3波一个boss
    if (wave % 2 == 0) {
        spawnBoss();
        return;
    }
    int enemiesToSpawn = 1 + QRandomGenerator::global()->bounded(qMax(1, wave / 3));
    enemiesToSpawn = qMin(enemiesToSpawn, 5);

    QRectF visibleRect = gameScene->sceneRect();
    float spawnMargin = 30.0f;

    for (int i = 0; i < enemiesToSpawn; ++i) {
        Enemy *enemy = new Enemy(wave);
        // 当新敌人诞生时，是否要显示血条
        enemy->showHealthBar = m_showHealthBars;
        if (enemy->getType() == Enemy::Ranged) {
            connect(enemy, &Enemy::fireBullet, this, &GameWidget::onEnemyFireBullet);
        }
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

void GameWidget::spawnBoss()
{
    // 如果已有Boss，则不再生成
    if (m_boss) return;

    waveTimer->stop();
    ui->waveTimerLabel->setText(QString("BOSS"));

    m_boss = new Boss(wave, nullptr);
    m_boss->showHealthBar = m_showHealthBars;

    // 连接Boss的开火信号
    connect(m_boss, &Boss::fireBullet, this, &GameWidget::onEnemyFireBullet);
    connect(m_boss, &Boss::fireRadialShot, this, &GameWidget::onBossFireRadialShot);

    // 将Boss放在屏幕中央
    m_boss->setPos(gameScene->width() / 2, gameScene->height() / 4);

    gameScene->addItem(m_boss);
    enemies.append(m_boss);
}

// 生成金币
void GameWidget::spawnCoin(const QPointF &position)
{
    Coin *coin = new Coin();
    coin->setPos(position);
    gameScene->addItem(coin);
    coins.append(coin);
}

void GameWidget::shootBullets()
{
    if (enemies.isEmpty() || !player) return;

    Enemy *nearestEnemy = nullptr;
    // 核心修正：将玩家的攻击距离作为初始的最小距离
    float minDistance = player->getAttackRange();

    for (Enemy *enemy : std::as_const(enemies)) {
        float distance = QLineF(player->pos(), enemy->pos()).length();
        // 寻找在攻击范围内且最近的敌人
        if (distance < minDistance) {
            minDistance = distance;
            nearestEnemy = enemy;
        }
    }

    if (nearestEnemy) {
        Bullet *bullet = new Bullet(player->pos(), nearestEnemy->pos(), Bullet::PlayerBullet, player->getAttackPower());
        gameScene->addItem(bullet);
        bullets.append(bullet);
        ResourceManager::instance().playSound("shoot");
    }
}

void GameWidget::onBossFireRadialShot(int bulletCount)
{
    if (!m_boss) return; // 安全检查

    QPointF startPos = m_boss->pos();
    // 360度 / 子弹数量 = 每颗子弹的角度间隔
    qreal angleStep = 360.0 / bulletCount;

    for (int i = 0; i < bulletCount; ++i) {
        qreal angle = i * angleStep;
        // 计算子弹的目标点（离Boss一段距离即可）
        QPointF targetPos(startPos.x() + 500 * cos(qDegreesToRadians(angle)),
                          startPos.y() + 500 * sin(qDegreesToRadians(angle)));

        // 创建子弹，伤害可以设为与Boss普通攻击相同或略低
        Bullet *bullet = new Bullet(startPos, targetPos, Bullet::EnemyBullet, m_boss->getDamage() / 2);
        gameScene->addItem(bullet);
        bullets.append(bullet);
    }
    ResourceManager::instance().playSound("enemy_shoot"); // 播放一个音效
}
