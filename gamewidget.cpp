#include "gamewidget.h"
#include "ui_gamewidget.h"
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QBrush>   // 1. 包含 QBrush 头文件
#include <QPixmap>  // 2. 包含 QPixmap 头文件

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameWidget)
    , gameScene(nullptr)
    , player(nullptr)
    , gameTimer(nullptr)
    , enemySpawnTimer(nullptr)
    , shootTimer(nullptr)
    , backgroundMusic(nullptr)
    , backgroundAudioOutput(nullptr)
    , shootSound(nullptr)
    , shootAudioOutput(nullptr)
    , hitSound(nullptr)
    , hitAudioOutput(nullptr)
    , gameOverSound(nullptr)
    , gameOverAudioOutput(nullptr)
    , score(0)
    , wave(1)
    , enemiesKilled(0)
    , gameRunning(false)
    , gamePaused(false)
    , frameCount(0)
    , gameInitialized(false)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
}

GameWidget::~GameWidget()
{
    delete ui;
}

void GameWidget::startGame()
{
    if (!gameInitialized) {
        setupGame();
        gameInitialized = true;
    }
    restartGame();
    setFocus();
}

void GameWidget::connectActions()
{
    // This can be used if you add buttons like "pause" to the game widget UI
}

void GameWidget::setupGame()
{
    gameScene = new QGraphicsScene(this);
    ui->gameView->setScene(gameScene);

    // 3. 设置地图背景图片
    // 提示: 您需要将名为 "game_background.png" 的图片添加到您的资源文件 (.qrc) 中
    QPixmap bgPixmap(":/images/map.png");
    if (!bgPixmap.isNull()) {
        // 将图片设置为场景背景，如果图片尺寸不匹配，它会被平铺
        gameScene->setBackgroundBrush(QBrush(bgPixmap));
    } else {
        // 如果图片加载失败，使用一个深灰色作为后备
        gameScene->setBackgroundBrush(QBrush(QColor(40, 40, 40)));
    }

    // 确保视图没有边框和滚动条
    ui->gameView->setFrameShape(QFrame::NoFrame);
    ui->gameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->gameView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->gameView->setStyleSheet("background: transparent; border: 0px"); // 使视图背景透明以显示场景背景

    gameTimer = new QTimer(this);
    enemySpawnTimer = new QTimer(this);
    shootTimer = new QTimer(this);

    connect(gameTimer, &QTimer::timeout, this, &GameWidget::updateGame);
    connect(enemySpawnTimer, &QTimer::timeout, this, &GameWidget::spawnEnemy);
    connect(shootTimer, &QTimer::timeout, this, &GameWidget::shootBullets);

    setupAudio();
}

void GameWidget::restartGame()
{
    gameRunning = false;

    if (player) {
        gameScene->removeItem(player);
        delete player;
    }
    player = new Player();

    // 4. 设置固定的地图大小 (例如 1920x1080)
    gameScene->setSceneRect(0, 0, 1920, 1080);

    // 将玩家放置在地图中心
    player->setPos(gameScene->width() / 2.0, gameScene->height() / 2.0);
    gameScene->addItem(player);

    // 确保视图内容适应窗口大小，并保持宽高比
    ui->gameView->fitInView(gameScene->sceneRect(), Qt::KeepAspectRatio);

    gameTimer->start(16); // ~60 FPS
    enemySpawnTimer->start(2000);
    shootTimer->start(600);
    fpsTimer.start();
    frameCount = 0;

    gameRunning = true;
    startBackgroundMusic();
    updateUI();
}

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
        // 5. 窗口大小改变时，重新调整视图以适应固定的地图
        ui->gameView->fitInView(gameScene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void GameWidget::pauseGame()
{
    if (!gameRunning) return;

    gamePaused = !gamePaused;
    if (gamePaused) {
        gameTimer->stop();
        enemySpawnTimer->stop();
        shootTimer->stop();
        stopBackgroundMusic();
        QMessageBox::information(this, "游戏暂停", "游戏已暂停。按 OK 或 ESC 继续。");
        // After the dialog is closed, resume the game
        gamePaused = false;
        gameTimer->start(16);
        enemySpawnTimer->start();
        shootTimer->start();
        startBackgroundMusic();
        fpsTimer.start();
    }
}

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

void GameWidget::checkCollisions()
{
    if (!player) return;

    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        bool bulletHit = false;
        for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
            if ((*bulletIt)->collidesWithItem(*enemyIt)) {
                (*enemyIt)->takeDamage(10);
                playHitSound();
                
                if ((*enemyIt)->isDead()) {
                    gameScene->removeItem(*enemyIt);
                    delete *enemyIt;
                    enemyIt = enemies.erase(enemyIt);
                    score += 5;
                    enemiesKilled++;
                } else {
                    ++enemyIt;
                }
                
                bulletHit = true;
                break;
            } else { 
                ++enemyIt; 
            }
        }
        if (bulletHit) {
            gameScene->removeItem(*bulletIt);
            delete *bulletIt;
            bulletIt = bullets.erase(bulletIt);
        } else { 
            ++bulletIt; 
        }
    }

    for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
        if ((*enemyIt)->collidesWithItem(player)) {
            player->takeDamage((*enemyIt)->getDamage());
            playHitSound();
            if (player->getHealth() <= 0) {
                gameOver();
                return;
            }
            gameScene->removeItem(*enemyIt);
            delete *enemyIt;
            enemyIt = enemies.erase(enemyIt);
        } else { 
            ++enemyIt; 
        }
    }
}

void GameWidget::cleanupObjects()
{
    QRectF sceneBounds = gameScene->sceneRect().adjusted(-100, -100, 100, 100);
    for (auto it = bullets.begin(); it != bullets.end();) {
        if (!sceneBounds.contains((*it)->pos())) {
            gameScene->removeItem(*it);
            delete *it;
            it = bullets.erase(it);
        } else { ++it; }
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

void GameWidget::updateUI()
{
    if (player) {
        ui->healthBar->setMaximum(player->getMaxHealth());
        ui->healthBar->setValue(player->getHealth());
        ui->healthBar->setFormat(QString("%1 / %2").arg(player->getHealth()).arg(player->getMaxHealth()));
    }
    ui->scoreLabel->setText(QString("🏆 分数: %1").arg(score));
    ui->waveLabel->setText(QString("🌊 波次: %1").arg(wave));
    ui->enemyCountLabel->setText(QString("👾 敌人: %1").arg(enemies.size()));
}

void GameWidget::gameOver()
{
    gameRunning = false;
    gameTimer->stop();
    enemySpawnTimer->stop();
    shootTimer->stop();
    stopBackgroundMusic();
    playGameOverSound();

    QMessageBox::information(this, "游戏结束",
        QString("土豆兄弟倒下了！\n\n你的分数: %1\n到达波次: %2").arg(score).arg(wave));

    emit backToMenuRequested();
}

void GameWidget::showControls() { /* Implementation can be added if needed */ }
void GameWidget::showAbout() { /* Implementation can be added if needed */ }

void GameWidget::setupAudio()
{
    backgroundMusic = new QMediaPlayer(this);
    backgroundAudioOutput = new QAudioOutput(this);
    backgroundMusic->setAudioOutput(backgroundAudioOutput);
    backgroundMusic->setSource(QUrl("qrc:/sounds/background.wav"));
    backgroundAudioOutput->setVolume(0.3);

    shootSound = new QMediaPlayer(this);
    shootAudioOutput = new QAudioOutput(this);
    shootSound->setAudioOutput(shootAudioOutput);
    shootSound->setSource(QUrl("qrc:/sounds/shoot.wav"));
    
    hitSound = new QMediaPlayer(this);
    hitAudioOutput = new QAudioOutput(this);
    hitSound->setAudioOutput(hitAudioOutput);
    hitSound->setSource(QUrl("qrc:/sounds/hit.wav"));
    hitAudioOutput->setVolume(0.4);
    
    gameOverSound = new QMediaPlayer(this);
    gameOverAudioOutput = new QAudioOutput(this);
    gameOverSound->setAudioOutput(gameOverAudioOutput);
    gameOverSound->setSource(QUrl("qrc:/sounds/push.wav"));
}

void GameWidget::playShootSound()
{
    if (shootSound->playbackState() == QMediaPlayer::PlayingState) {
        shootSound->setPosition(0);
    } else {
        shootSound->play();
    }
}

void GameWidget::playHitSound()
{
    if (hitSound->playbackState() == QMediaPlayer::PlayingState) {
        hitSound->setPosition(0);
    } else {
        hitSound->play();
    }
}

void GameWidget::playGameOverSound()
{
    gameOverSound->play();
}

void GameWidget::startBackgroundMusic()
{
    backgroundMusic->setLoops(QMediaPlayer::Infinite);
    backgroundMusic->play();
}

void GameWidget::stopBackgroundMusic()
{
    backgroundMusic->stop();
}
