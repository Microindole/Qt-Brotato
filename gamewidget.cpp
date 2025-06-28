#include "gamewidget.h"
#include "ui_gamewidget.h"
#include "bullet.h"
#include "resourcemanager.h"
#include <QBrush>
#include <QPixmap>
#include <QTimer>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QLineF>
#include <QAudioOutput>
#include <QUrl>
#include <QDebug>
#include <QLabel>
#include "shop.h"


GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GameWidget)
    , m_isWaveTransition(false)
    , gameScene(nullptr)
    , shopWidget(nullptr)
    , player(nullptr)
    , pauseWidget(nullptr)
    , settingsWidget(nullptr)
    , upgradeWidget(nullptr)
    , gameTimer(new QTimer(this))
    , enemySpawnTimer(new QTimer(this))
    , shootTimer(new QTimer(this))
    , periodicEffectsTimer(new QTimer(this))
    , waveTimer(new QTimer(this))
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
    , m_selectedCharacter(Player::AllRounder)
    , gameInitialized(false)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    
    pauseWidget = new Pause(this);
    settingsWidget = new Settings(this);
    upgradeWidget = new UpgradeWidget(this);
    shopWidget = new Shop(this);
    
    pauseWidget->hide();
    settingsWidget->hide();
    upgradeWidget->hide();
    shopWidget->hide();

    connect(gameTimer, &QTimer::timeout, this, &GameWidget::updateGame);
    connect(enemySpawnTimer, &QTimer::timeout, this, &GameWidget::spawnEnemy);
    connect(shootTimer, &QTimer::timeout, this, &GameWidget::shootBullets);
    connect(periodicEffectsTimer, &QTimer::timeout, this, &GameWidget::onPeriodicEffects);
    connect(waveTimer, &QTimer::timeout, this, &GameWidget::handleWaveEnd);

    connect(pauseWidget, &Pause::continueGame, this, &GameWidget::handleContinueAction);
    connect(pauseWidget, &Pause::restartGame, this, &GameWidget::onRestartFromPause);
    connect(pauseWidget, &Pause::endRun, this, &GameWidget::onEndRun);
    connect(pauseWidget, &Pause::openSettings, this, &GameWidget::onOpenSettings);
    connect(pauseWidget, &Pause::backToMenu, this, &GameWidget::onBackToMenu);

    connect(settingsWidget, &Settings::backgroundMusicVolumeChanged, this, &GameWidget::onMusicVolumeChanged);
    connect(settingsWidget, &Settings::soundEffectsVolumeChanged, this, &GameWidget::onSfxVolumeChanged);
    connect(settingsWidget, &Settings::backClicked, this, &GameWidget::onSettingsClosed);
    connect(settingsWidget, &Settings::healthBarVisibilityChanged, this, &GameWidget::onHealthBarVisibilityChanged);


    connect(upgradeWidget, &UpgradeWidget::upgradeSelected, this, &GameWidget::onUpgradeSelected);

    connect(shopWidget, &Shop::continueToNextWave, this, &GameWidget::startNextWave);
    connect(shopWidget, &Shop::backToMenu, this, &GameWidget::onBackToMenu);
    connect(shopWidget, &Shop::itemPurchased, this, &GameWidget::onShopItemPurchased);
}

GameWidget::~GameWidget()
{
    delete ui;
}

// 游戏主流程
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
    ui->gameView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    QPixmap originalBgPixmap(":/images/map.png"); 
    
    if (originalBgPixmap.isNull()) {
        qWarning() << "错误：无法加载背景图片 ':/images/map.png'。请检查路径和资源文件(.qrc)。";
        gameScene->setBackgroundBrush(QColor(30, 30, 30));
        gameScene->setSceneRect(0, 0, 1280, 720);
    } else {
        QSize targetSize(1920, 1080);
        QPixmap scaledBgPixmap = originalBgPixmap.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        gameScene->setBackgroundBrush(QBrush(scaledBgPixmap));
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
    waveTimeLeft = 30;
    pendingLevelUps = 0;
    updateUpgradeIndicators();
    
    player->setPos(gameScene->width() / 2.0, gameScene->height() / 2.0);
    gameScene->addItem(player);
    ui->gameView->fitInView(gameScene->sceneRect(), Qt::KeepAspectRatio);

    gameTimer->start(16); // ~60 FPS
    enemySpawnTimer->start(2000);
    shootTimer->start(600);
    periodicEffectsTimer->start(1000);
    waveTimer->start(30000);
    fpsTimer.start();
    frameCount = 0;

    gameRunning = true;
    startBackgroundMusic(); 
    updateUI();
}

void GameWidget::setCharacter(Player::CharacterType type)
{
    m_selectedCharacter = type;
}

void GameWidget::onEnemyFireBullet(const QPointF& from, const QPointF& to_placeholder)
{
    Q_UNUSED(to_placeholder);

    if (!player) return;

    // 创建一颗属于敌人的子弹，目标是玩家的当前位置
    // 假设远程怪的基础伤害是 8
    Bullet *bullet = new Bullet(from, player->pos(), Bullet::EnemyBullet, 8);

    gameScene->addItem(bullet);
    bullets.append(bullet);

    // 播放敌人射击音效
    ResourceManager::instance().playSound("push");
}

void GameWidget::showShopScreen()
{
    // 隐藏升级界面，显示商店
    upgradeWidget->hide();
    shopWidget->resize(this->size());
    shopWidget->move(0, 0);
    shopWidget->showShop(player); // 显示商店并传入玩家信息
}

void GameWidget::onShopItemPurchased(const QString &itemId)
{
    if (!player) return;

    // 根据购买的itemId来应用效果
    if (itemId == "heal_potion") {
        player->heal(25); // 直接治疗
    } else if (itemId == "glove") {
        player->increaseAttackPower(4);
    } else if (itemId == "helmet") {
        player->increaseArmor(5);
    } else if (itemId == "wings") {
        player->multiplySpeed(1.10f);
    } else if (itemId == "scope") {
        player->increaseAttackRange(50);
    } else if (itemId == "plant") {
        player->increaseHealthRegen(0.5f);
    } else if (itemId == "potato") {
        player->increaseMaxHealth(3);
        player->increaseAttackPower(1);
        player->multiplySpeed(1.02f);
    } else if (itemId == "glasses") {
        player->increaseAttackRange(100);
        player->multiplyAttackPower(0.95f);
    } else if (itemId == "scared_sausage") {
        player->multiplySpeed(1.20f);
        player->multiplyMaxHealth(0.90f);
    } else if (itemId == "weird_ghost") {
        player->multiplyExpGain(1.15f);
    }

    // 购买后可以播放一个音效
    ResourceManager::instance().playSound("push");
}

