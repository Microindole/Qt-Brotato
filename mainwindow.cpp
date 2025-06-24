#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QRandomGenerator>
#include <QApplication>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , gameScene(nullptr)
    , player(nullptr)
    , score(0)
    , wave(1)
    , enemiesKilled(0)
    , gameRunning(false)
    , gamePaused(false)
    , frameCount(0)
    , gameInitialized(false) // 初始化新变量
{
    ui->setupUi(this);
    setupAudio();  // 添加这行
    showStartScreen(); // 显示开始界面
    connectActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectActions()
{
    connect(ui->actionNewGame, &QAction::triggered, this, &MainWindow::restartGame);
    connect(ui->actionPause, &QAction::triggered, this, &MainWindow::pauseGame);
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionControls, &QAction::triggered, this, &MainWindow::showControls);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAbout);
    
    // 连接开始界面的按钮
    connect(ui->startGameButton, &QPushButton::clicked, this, &MainWindow::startGame);
    connect(ui->helpButton, &QPushButton::clicked, this, &MainWindow::showHelp);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWindow::exitGame);
    connect(ui->backToMenuButton, &QPushButton::clicked, this, &MainWindow::backToMenu);
}

void MainWindow::showStartScreen()
{
    ui->stackedWidget->setCurrentIndex(0); // 显示开始界面
    ui->statusbar->showMessage("欢迎来到土豆兄弟！");
}

void MainWindow::showGameScreen()
{
    ui->stackedWidget->setCurrentIndex(1); // 显示游戏界面
    setFocusPolicy(Qt::StrongFocus);
    setFocus(); // 确保主窗口获得焦点以接收键盘事件
}

void MainWindow::startGame()
{
    if (!gameInitialized) {
        setupGame();
        gameInitialized = true;
    }
    showGameScreen();
    restartGame();
}

void MainWindow::backToMenu()
{
    // 停止游戏
    if (gameRunning) {
        gameRunning = false;
        gameTimer->stop();
        enemySpawnTimer->stop();
        shootTimer->stop();
        stopBackgroundMusic(); // 添加：停止背景音乐
    }
    
    // 清理游戏对象
    if (player) {
        gameScene->removeItem(player);
        delete player;
        player = nullptr;
    }
    qDeleteAll(enemies);
    enemies.clear();
    qDeleteAll(bullets);
    bullets.clear();
    
    showStartScreen();
}

void MainWindow::showHelp()
{
    QMessageBox::information(this, "游戏帮助",
        "<h2>🎮 土豆兄弟 - 游戏说明</h2>"
        "<h3>🎯 游戏目标</h3>"
        "<p>控制土豆兄弟击败一波又一波的敌人，尽可能获得高分！</p>"
        "<h3>🕹️ 控制方式</h3>"
        "<ul>"
        "<li><b>移动:</b> WASD 键或方向键</li>"
        "<li><b>射击:</b> 自动瞄准最近的敌人</li>"
        "<li><b>暂停:</b> ESC 键</li>"
        "<li><b>新游戏:</b> F2 键</li>"
        "</ul>"
        "<h3>⚡ 游戏机制</h3>"
        "<ul>"
        "<li>每波敌人会越来越多越来越强</li>"
        "<li>击败敌人获得分数</li>"
        "<li>小心敌人的攻击，保护你的生命值</li>"
        "<li>敌人会显示血条，需要多次攻击才能击败</li>"
        "</ul>"
        "<p><b>祝你游戏愉快！🥔</b></p>");
}

void MainWindow::exitGame()
{
    QApplication::quit();
}

void MainWindow::setupGame()
{
    // 设置场景和视图
    gameScene = new QGraphicsScene(this);
    ui->gameView->setScene(gameScene);
    gameScene->setSceneRect(0, 0, ui->gameView->width() - 2, ui->gameView->height() - 2);

    // 初始化定时器
    gameTimer = new QTimer(this);
    enemySpawnTimer = new QTimer(this);
    shootTimer = new QTimer(this);

    connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateGame);
    connect(enemySpawnTimer, &QTimer::timeout, this, &MainWindow::spawnEnemy);
    connect(shootTimer, &QTimer::timeout, this, &MainWindow::shootBullets);
}

void MainWindow::restartGame()
{
    // 确保游戏已初始化
    if (!gameInitialized) {
        setupGame();
        gameInitialized = true;
    }

    // 确保当前在游戏界面
    showGameScreen();

    // 暂停游戏以安全地清理
    gameRunning = false;

    // 清理现有游戏对象
    if (player) {
        gameScene->removeItem(player);
        delete player;
        player = nullptr;
    }
    qDeleteAll(enemies);
    enemies.clear();
    qDeleteAll(bullets);
    bullets.clear();

    // 重置游戏状态
    score = 0;
    wave = 1;
    enemiesKilled = 0;
    gamePaused = false;
    pressedKeys.clear();

    // 创建玩家
    player = new Player();
    player->setPos(gameScene->width() / 2, gameScene->height() / 2);
    gameScene->addItem(player);

    // 启动定时器 - 调整初始速度
    gameTimer->start(16); // 约60FPS
    enemySpawnTimer->start(2000); // 敌人生成速度稍快一些，从2500降到2000
    shootTimer->start(600);       // 射击频率降低，从500增加到600
    fpsTimer.start();
    frameCount = 0;

    gameRunning = true;
    startBackgroundMusic(); // 添加：开始播放背景音乐
    ui->statusbar->showMessage("游戏开始！", 3000);
    updateUI();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }
    if (event->key() == Qt::Key_Escape) {
        pauseGame();
    } else {
        pressedKeys.insert(event->key());
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }
    pressedKeys.remove(event->key());
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::pauseGame()
{
    if (!gameRunning) return;

    gamePaused = !gamePaused;
    if (gamePaused) {
        gameTimer->stop();
        enemySpawnTimer->stop();
        shootTimer->stop();
        ui->statusbar->showMessage("游戏已暂停。按 ESC 继续。");
    } else {
        gameTimer->start(16);
        enemySpawnTimer->start();
        shootTimer->start();
        fpsTimer.start(); // 重置FPS计时器
        ui->statusbar->showMessage("游戏继续！", 2000);
    }
}

void MainWindow::updateGame()
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

    // 计算FPS
    frameCount++;
    if (fpsTimer.elapsed() > 1000) {
        float fps = frameCount / (fpsTimer.elapsed() / 1000.0f);
        ui->fpsLabel->setText(QString("FPS: %1").arg(qRound(fps)));
        fpsTimer.restart();
        frameCount = 0;
    }
}

void MainWindow::movePlayer()
{
    QPointF pos = player->pos();
    const float speed = 2.8f; // 移动速度从3.5降低到2.8

    if (pressedKeys.contains(Qt::Key_W) || pressedKeys.contains(Qt::Key_Up))    pos.setY(pos.y() - speed);
    if (pressedKeys.contains(Qt::Key_S) || pressedKeys.contains(Qt::Key_Down))  pos.setY(pos.y() + speed);
    if (pressedKeys.contains(Qt::Key_A) || pressedKeys.contains(Qt::Key_Left))  pos.setX(pos.x() - speed);
    if (pressedKeys.contains(Qt::Key_D) || pressedKeys.contains(Qt::Key_Right)) pos.setX(pos.x() + speed);

    pos.setX(qBound(15.0, pos.x(), gameScene->width() - 15.0));
    pos.setY(qBound(15.0, pos.y(), gameScene->height() - 15.0));
    player->setPos(pos);
}

void MainWindow::spawnEnemy()
{
    // 根据当前波数决定一次生成几个敌人 - 数量控制更温和
    int enemiesToSpawn = 1 + QRandomGenerator::global()->bounded(qMax(1, wave / 3)); // 成长更缓慢
    // 限制单次生成的最大数量，防止场面瞬间失控
    enemiesToSpawn = qMin(enemiesToSpawn, 5); // 最大生成数量降低

    // 获取视图当前可见的场景区域，这是更精确的边界
    QRectF visibleRect = ui->gameView->mapToScene(ui->gameView->viewport()->rect()).boundingRect();
    float spawnMargin = 30.0f; // 在可见区域外多少距离生成敌人

    for (int i = 0; i < enemiesToSpawn; ++i) {
        Enemy *enemy = new Enemy(wave); // 传入当前波数
        int side = QRandomGenerator::global()->bounded(4);
        QPointF pos;

        // 根据可见区域来计算生成位置，确保敌人从屏幕外进入
        switch (side) {
            case 0: // 上方
                pos = QPointF(QRandomGenerator::global()->bounded(static_cast<int>(visibleRect.left()), 
                                                                static_cast<int>(visibleRect.right())),
                              visibleRect.top() - spawnMargin);
                break;
            case 1: // 右方
                pos = QPointF(visibleRect.right() + spawnMargin,
                              QRandomGenerator::global()->bounded(static_cast<int>(visibleRect.top()), 
                                                                static_cast<int>(visibleRect.bottom())));
                break;
            case 2: // 下方
                pos = QPointF(QRandomGenerator::global()->bounded(static_cast<int>(visibleRect.left()), 
                                                                static_cast<int>(visibleRect.right())),
                              visibleRect.bottom() + spawnMargin);
                break;
            case 3: // 左方
                pos = QPointF(visibleRect.left() - spawnMargin,
                              QRandomGenerator::global()->bounded(static_cast<int>(visibleRect.top()), 
                                                                static_cast<int>(visibleRect.bottom())));
                break;
        }

        enemy->setPos(pos);
        gameScene->addItem(enemy);
        enemies.append(enemy);
    }
}

void MainWindow::shootBullets()
{
    if (enemies.isEmpty()) return;

    Enemy *nearestEnemy = nullptr;
    float minDistance = std::numeric_limits<float>::max();
    for (Enemy *enemy : enemies) {
        QPointF diff = player->pos() - enemy->pos();
        float distance = std::sqrt(diff.x() * diff.x() + diff.y() * diff.y());
        if (distance < minDistance) {
            minDistance = distance;
            nearestEnemy = enemy;
        }
    }

    if (nearestEnemy && minDistance < 300) { // 射程从400降低到300
        Bullet *bullet = new Bullet(player->pos(), nearestEnemy->pos());
        gameScene->addItem(bullet);
        bullets.append(bullet);
        playShootSound(); // 添加：播放射击音效
    }
}

void MainWindow::checkCollisions()
{
    // 子弹与敌人
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        bool bulletHit = false;
        for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
            if ((*bulletIt)->collidesWithItem(*enemyIt)) {
                (*enemyIt)->takeDamage(10); // 子弹伤害从12降低到10
                playHitSound(); // 添加：播放击中音效
                
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

    // 敌人与玩家的碰撞保持不变
    for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
        if ((*enemyIt)->collidesWithItem(player)) {
            player->takeDamage((*enemyIt)->getDamage());
            playHitSound(); // 添加：播放击中音效
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

void MainWindow::cleanupObjects()
{
    QRectF sceneBounds = gameScene->sceneRect();
    for (auto it = bullets.begin(); it != bullets.end();) {
        if ((*it)->isOutOfBounds(sceneBounds)) {
            gameScene->removeItem(*it);
            delete *it;
            it = bullets.erase(it);
        } else { ++it; }
    }
}

void MainWindow::updateWave()
{
    // 更温和的波次进程，但对玩家更有挑战性
    if (enemiesKilled >= wave * 5 + 6) { // 每波需要击败的敌人数量减少，让波次进展更快
        wave++;
        enemiesKilled = 0;
        
        // 敌人生成速度提升稍快一些
        int spawnInterval = qMax(600, 2000 - (wave - 1) * 140); // 最快生成间隔降低
        enemySpawnTimer->setInterval(spawnInterval);
        
        // 玩家射速提升更缓慢
        int shootInterval = qMax(300, 600 - (wave - 1) * 10); // 射速提升更慢
        shootTimer->setInterval(shootInterval);
        
        ui->statusbar->showMessage(QString("第 %1 波敌人来袭！").arg(wave), 3000);
    }
}

void MainWindow::updateUI()
{
    if (player) {
        ui->healthBar->setValue(player->getHealth());
        ui->healthBar->setFormat(QString("%1 / %2").arg(player->getHealth()).arg(player->getMaxHealth()));
    }
    ui->scoreLabel->setText(QString("🏆 分数: %1").arg(score));
    ui->waveLabel->setText(QString("🌊 波次: %1").arg(wave));
    ui->enemyCountLabel->setText(QString("👾 敌人: %1").arg(enemies.size()));
}

void MainWindow::gameOver()
{
    gameRunning = false;
    gameTimer->stop();
    enemySpawnTimer->stop();
    shootTimer->stop();
    stopBackgroundMusic(); // 添加：停止背景音乐
    playGameOverSound();   // 添加：播放游戏结束音效

    QMessageBox::StandardButton reply = QMessageBox::question(this, "游戏结束",
        QString("土豆兄弟倒下了！\n\n你的分数: %1\n到达波次: %2\n\n是否重新开始?").arg(score).arg(wave),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        restartGame();
    } else {
        QApplication::quit();
    }
}

void MainWindow::showControls()
{
    bool wasPaused = gamePaused;
    if (gameRunning && !wasPaused) {
        pauseGame();
    }

    QMessageBox::information(this, "控制说明",
        "<h2>🎮 游戏控制</h2>"
        "<ul>"
        "<li><b>移动:</b> WASD 或方向键</li>"
        "<li><b>射击:</b> 自动向最近的敌人射击</li>"
        "<li><b>暂停/继续:</b> ESC</li>"
        "<li><b>新游戏:</b> F2</li>"
        "</ul>");

    if (gameRunning && !wasPaused) {
        pauseGame(); // 恢复游戏
    }
}

void MainWindow::showAbout()
{
    bool wasPaused = gamePaused;
    if (gameRunning && !wasPaused) {
        pauseGame();
    }

    QMessageBox::about(this, "关于",
        "<h2>🥔 土豆兄弟 (Brotato)</h2>"
        "<p>一个由 GitHub Copilot 协助创建的Qt小游戏。</p>"
        "<p>版本: 1.1 (更高难度)</p>"
        "<p>灵感来源: Brotato</p>");

    if (gameRunning && !wasPaused) {
        pauseGame(); // 恢复游戏
    }
}

void MainWindow::setupAudio()
{
    // 设置背景音乐
    backgroundMusic = new QMediaPlayer(this);
    backgroundAudioOutput = new QAudioOutput(this);
    backgroundMusic->setAudioOutput(backgroundAudioOutput);
    backgroundMusic->setSource(QUrl("qrc:/sounds/background.wav"));
    backgroundAudioOutput->setVolume(0.3); // 设置音量

    // 设置射击音效
    shootSound = new QMediaPlayer(this);
    shootAudioOutput = new QAudioOutput(this);
    shootSound->setAudioOutput(shootAudioOutput);
    shootSound->setSource(QUrl("qrc:/sounds/shoot.wav"));
    
    // 设置击中音效
    hitSound = new QMediaPlayer(this);
    hitAudioOutput = new QAudioOutput(this);
    hitSound->setAudioOutput(hitAudioOutput);
    hitSound->setSource(QUrl("qrc:/sounds/hit.wav"));
    hitAudioOutput->setVolume(0.4); // 新增：降低攻击音效的音量
    
    // 设置游戏结束音效
    gameOverSound = new QMediaPlayer(this);
    gameOverAudioOutput = new QAudioOutput(this);
    gameOverSound->setAudioOutput(gameOverAudioOutput);
    gameOverSound->setSource(QUrl("qrc:/sounds/push.wav"));
}

void MainWindow::playShootSound()
{
    shootSound->setPosition(0);
    shootSound->play();
}

void MainWindow::playHitSound()
{
    hitSound->setPosition(0);
    hitSound->play();
}

void MainWindow::playGameOverSound()
{
    gameOverSound->setPosition(0);
    gameOverSound->play();
}

void MainWindow::startBackgroundMusic()
{
    backgroundMusic->setLoops(QMediaPlayer::Infinite);
    backgroundMusic->play();
}

void MainWindow::stopBackgroundMusic()
{
    backgroundMusic->stop();
}
