#include "gamewidget.h"
#include "ui_gamewidget.h"
#include "bullet.h"
#include "enemy.h"
#include <QBrush>
#include <QPixmap>
#include <QTimer>
#include <QMessageBox>
#include <QRandomGenerator>
#include <algorithm>
#include <QLineF>
#include <limits>
#include <QAudioOutput>
#include <QUrl>
#include <QDebug>


GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameWidget)
    , gameScene(nullptr)
    , player(nullptr)
    , pauseWidget(nullptr)
    , settingsWidget(nullptr)
    , upgradeWidget(nullptr)
    , gameTimer(new QTimer(this))
    , enemySpawnTimer(new QTimer(this))
    , shootTimer(new QTimer(this))
    , periodicEffectsTimer(new QTimer(this))
    , backgroundMusic(nullptr)
    , backgroundAudioOutput(nullptr)
    , score(0)
    , wave(1)
    , enemiesKilled(0)
    , currentMusicVolume(0.7f)
    , currentSfxVolume(0.2f)
    , m_showHealthBars(true)
    , gameRunning(false)
    , gamePaused(false)
    , frameCount(0)
    , gameInitialized(false)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    
    pauseWidget = new Pause(this);
    settingsWidget = new Settings(this);
    upgradeWidget = new UpgradeWidget(this);
    
    pauseWidget->hide();
    settingsWidget->hide();
    upgradeWidget->hide();

    connect(gameTimer, &QTimer::timeout, this, &GameWidget::updateGame);
    connect(enemySpawnTimer, &QTimer::timeout, this, &GameWidget::spawnEnemy);
    connect(shootTimer, &QTimer::timeout, this, &GameWidget::shootBullets);
    connect(periodicEffectsTimer, &QTimer::timeout, this, &GameWidget::onPeriodicEffects);

    connect(pauseWidget, &Pause::continueGame, this, &GameWidget::onContinueGame);
    connect(pauseWidget, &Pause::restartGame, this, &GameWidget::onRestartFromPause);
    connect(pauseWidget, &Pause::endRun, this, &GameWidget::onEndRun);
    connect(pauseWidget, &Pause::openSettings, this, &GameWidget::onOpenSettings);
    connect(pauseWidget, &Pause::backToMenu, this, &GameWidget::onBackToMenu);

    connect(settingsWidget, &Settings::backgroundMusicVolumeChanged, this, &GameWidget::onMusicVolumeChanged);
    connect(settingsWidget, &Settings::soundEffectsVolumeChanged, this, &GameWidget::onSfxVolumeChanged);
    connect(settingsWidget, &Settings::backClicked, this, &GameWidget::onSettingsClosed);
    connect(settingsWidget, &Settings::healthBarVisibilityChanged, this, &GameWidget::onHealthBarVisibilityChanged);


    connect(upgradeWidget, &UpgradeWidget::upgradeSelected, this, &GameWidget::onUpgradeSelected);
}

GameWidget::~GameWidget()
{
    delete ui;
}

// --- 游戏主流程 ---
void GameWidget::startGame()
{
    if (!gameInitialized) {
        setupGame();
        setupAudio();
        gameInitialized = true;
    }
    restartGame();
}

void GameWidget::setupGame()
{
    gameScene = new QGraphicsScene(this);
    ui->gameView->setScene(gameScene);
    ui->gameView->setRenderHint(QPainter::Antialiasing);

    // 强制视图进行完整刷新，可以消除移动物体时的残影。
    ui->gameView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    //  加载原始的大尺寸背景图片
    QPixmap originalBgPixmap(":/images/map.png"); 
    
    if (originalBgPixmap.isNull()) {
        qWarning() << "错误：无法加载背景图片 ':/images/map.png'。请检查路径和资源文件(.qrc)。";
        gameScene->setBackgroundBrush(QColor(30, 30, 30));
        // 设置一个默认的游戏区域大小
        gameScene->setSceneRect(0, 0, 1280, 720);
    } else {

        //  定义一个更大的目标尺寸，这会让角色看起来小一些。
        QSize targetSize(1920, 1080); 

        //  将原始图片缩放到目标尺寸，保持宽高比，使用平滑变换以保证图片质量
        QPixmap scaledBgPixmap = originalBgPixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        //  使用缩放后的小图片作为背景
        gameScene->setBackgroundBrush(QBrush(scaledBgPixmap));
        
        //  将游戏场景的大小设置为与缩放后的图片完全相同
        gameScene->setSceneRect(scaledBgPixmap.rect());
    }
}

void GameWidget::restartGame()
{
    gameRunning = false;
    gamePaused = false;
    upgradeWidget->hide();

    if (player) {
        gameScene->removeItem(player);
        delete player;
        player = nullptr;
    }
    player = new Player(m_selectedCharacter); // 使用特定角色类型
    player->showHealthBar = m_showHealthBars;
    connect(player, &Player::levelUpOccurred, this, &GameWidget::onPlayerLevelUp);

    qDeleteAll(enemies);
    enemies.clear();
    qDeleteAll(bullets);
    bullets.clear();
    qDeleteAll(coins);
    coins.clear();

    score = 0;
    wave = 1;
    enemiesKilled = 0;
    
    player->setPos(gameScene->width() / 2.0, gameScene->height() / 2.0);
    gameScene->addItem(player);

    // 因为场景大小已经合适了，我们直接用 fitInView 就能得到很好的效果
    ui->gameView->fitInView(gameScene->sceneRect(), Qt::KeepAspectRatio);

    gameTimer->start(16); // ~60 FPS
    enemySpawnTimer->start(2000);
    shootTimer->start(600);
    periodicEffectsTimer->start(1000);
    fpsTimer.start();
    frameCount = 0;

    gameRunning = true;
    startBackgroundMusic(); 
    updateUI();
}
