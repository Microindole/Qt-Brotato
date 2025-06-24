#include "gamewidget.h"
#include "ui_gamewidget.h"
#include <QBrush>
#include <QPixmap>

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameWidget)
    , gameScene(nullptr)
    , player(nullptr)
    , pauseWidget(nullptr)
    , settingsWidget(nullptr) // 初始化
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
    , currentMusicVolume(0.7f) // 初始化音乐音量
    , currentSfxVolume(0.02f)   // 初始化音效音量 (100% 对应 0.02)
    , gameRunning(false)
    , gamePaused(false)
    , frameCount(0)
    , gameInitialized(false)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    
    pauseWidget = new Pause(this);
    pauseWidget->hide();
    
    settingsWidget = new Settings(this); // 创建设置窗口
    settingsWidget->hide();              // 默认隐藏

    connect(pauseWidget, &Pause::continueGame, this, &GameWidget::onContinueGame);
    connect(pauseWidget, &Pause::restartGame, this, &GameWidget::onRestartFromPause);
    connect(pauseWidget, &Pause::endRun, this, &GameWidget::onEndRun);
    connect(pauseWidget, &Pause::openSettings, this, &GameWidget::onOpenSettings);
    connect(pauseWidget, &Pause::backToMenu, this, &GameWidget::onBackToMenu);

    // 连接设置窗口的信号
    connect(settingsWidget, &Settings::backgroundMusicVolumeChanged, this, &GameWidget::onMusicVolumeChanged);
    connect(settingsWidget, &Settings::soundEffectsVolumeChanged, this, &GameWidget::onSfxVolumeChanged);
    connect(settingsWidget, &Settings::backClicked, this, &GameWidget::onSettingsClosed);
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

void GameWidget::setupGame()
{
    gameScene = new QGraphicsScene(this);
    ui->gameView->setScene(gameScene);

    QPixmap bgPixmap(":/images/map.png");
    if (!bgPixmap.isNull()) {
        gameScene->setBackgroundBrush(QBrush(bgPixmap));
    } else {
        gameScene->setBackgroundBrush(QBrush(QColor(40, 40, 40)));
    }

    ui->gameView->setFrameShape(QFrame::NoFrame);
    ui->gameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->gameView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->gameView->setStyleSheet("background: transparent; border: 0px");

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
    gamePaused = false;

    if (player) {
        gameScene->removeItem(player);
        delete player;
    }
    player = new Player();

    qDeleteAll(enemies);
    enemies.clear();
    qDeleteAll(bullets);
    bullets.clear();

    score = 0;
    wave = 1;
    enemiesKilled = 0;

    gameScene->setSceneRect(0, 0, 1920, 1080);
    player->setPos(gameScene->width() / 2.0, gameScene->height() / 2.0);
    gameScene->addItem(player);

    ui->gameView->fitInView(gameScene->sceneRect(), Qt::KeepAspectRatio);

    gameTimer->start(16);
    enemySpawnTimer->start(2000);
    shootTimer->start(600);
    fpsTimer.start();
    frameCount = 0;

    gameRunning = true;
    startBackgroundMusic();
    updateUI();
}

// gamewidget_events.cpp (处理键盘和窗口事件)
// gamewidget_update.cpp (核心游戏循环和逻辑更新)
// gamewidget_collisions.cpp (碰撞检测和对象清理)
// gamewidget_state.cpp (游戏状态管理：暂停、结束等)
// gamewidget_ui.cpp (UI更新和菜单显示)
// gamewidget_audio.cpp (所有音频相关的功能)
// gamewidget.cpp (主文件)