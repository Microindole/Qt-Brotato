#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QTimer>
#include <QSet>
#include <QElapsedTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QKeyEvent>
#include <QResizeEvent>
#include "player.h"
#include "enemy.h"
#include "boss.h"
#include "bullet.h"
#include "pause.h"
#include "settings.h"
#include "upgradewidget.h"
#include "coin.h"
#include "player.h"
#include "shop.h"

class QMediaPlayer;
class QAudioOutput;
class Enemy;
class Bullet;
class Coin;
class Settings;
class QLabel;

namespace Ui {
class GameWidget;
}

class Boss;

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = nullptr);
    ~GameWidget();
    
    void startGame();
    void setCharacter(Player::CharacterType type);

signals:
    void backToMenuRequested();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateGame();
    void spawnEnemy();
    void shootBullets();
    void pauseGame();

    void handleWaveEnd();
    void startNextWave();
    void handleContinueAction();

    // 周期性效果的槽函数
    void onPeriodicEffects();
    
    // 暂停界面槽函数
    void onContinueGame();
    void onRestartFromPause();
    void onEndRun();
    void onOpenSettings();
    void onBackToMenu();

    // 设置界面槽函数
    void onMusicVolumeChanged(float volume);
    void onSfxVolumeChanged(float volume);
    void onSettingsClosed();
    void onHealthBarVisibilityChanged(bool visible);
    void onEnemyFireBullet(const QPointF& from, const QPointF& to);

    // 升级流程的槽函数声明
    void onPlayerLevelUp();
    void onUpgradeSelected(UpgradeWidget::UpgradeType type);

    // 响应商店信号
    void showShopScreen();
    void onShopItemPurchased(const QString& itemId);

    // Boss
    void onBossFireRadialShot(int bulletCount);

private:
    Player::CharacterType m_selectedCharacter; // 测试角色
    void setupGame();
    void restartGame();
    void spawnBoss();
    void updateUpgradeIndicators();
    void setupAudio();
    void connectActions();
    void movePlayer();
    void checkCollisions();
    void cleanupObjects();
    void updateWave();
    void updateUI();
    void gameOver();
    void showPauseMenu();
    void hidePauseMenu();
    void showUpgradeMenu();
    void showWaveCompleteScreen();
    
    // 音频函数
    void startBackgroundMusic();
    void stopBackgroundMusic();
    void pauseBackgroundMusic();
    void resumeBackgroundMusic();

    // 音量控制
    void setMusicVolume(float volume);
    void setSfxVolume(float volume);

    // 金币商店
    void spawnCoin(const QPointF &position);
    Shop *shopWidget;

    Ui::GameWidget *ui;
    
    // 游戏组件
    QGraphicsScene *gameScene;
    Player *player;
    QList<Enemy*> enemies;
    QList<Bullet*> bullets;
    QList<Coin*> coins;
    QList<QLabel*> m_upgradeIndicators;
    Pause *pauseWidget;
    Settings *settingsWidget;
    UpgradeWidget *upgradeWidget;

    // 定时器
    QTimer *gameTimer;
    QTimer *enemySpawnTimer;
    QTimer *shootTimer;
    QTimer *periodicEffectsTimer;
    QTimer *waveTimer;
    QElapsedTimer fpsTimer;

    // 音频组件
    QMediaPlayer *backgroundMusic;
    QAudioOutput *backgroundAudioOutput;

    // 游戏状态
    QSet<int> pressedKeys;
    int score;
    int wave;
    int waveTimeLeft;
    int pendingLevelUps;
    int enemiesKilled;
    float currentMusicVolume;
    float currentSfxVolume;
    bool m_showHealthBars;
    bool gameRunning;
    bool gamePaused;
    int frameCount;
    bool gameInitialized;
    bool m_isWaveTransition;
    Boss* m_boss;
};

#endif // GAMEWIDGET_H
