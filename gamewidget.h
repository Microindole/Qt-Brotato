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
#include "bullet.h"
#include "pause.h"
#include "settings.h"
#include "upgradewidget.h"

namespace Ui {
class GameWidget;
}

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = nullptr);
    ~GameWidget();
    
    void startGame();

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
    void restartGame();
    void showControls();
    void showAbout();
    
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

    // 升级流程的槽函数声明
    void onPlayerLevelUp();
    void onUpgradeSelected(UpgradeWidget::UpgradeType type);

private:
    void setupGame();
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
    
    // 音频函数
    void playShootSound();
    void playHitSound();
    void playGameOverSound();
    void startBackgroundMusic();
    void stopBackgroundMusic();

    // 音量控制
    void setMusicVolume(float volume);
    void setSfxVolume(float volume);

    Ui::GameWidget *ui;
    
    // 游戏组件
    QGraphicsScene *gameScene;
    Player *player;
    QList<Enemy*> enemies;
    QList<Bullet*> bullets;
    Pause *pauseWidget;
    Settings *settingsWidget;
    UpgradeWidget *upgradeWidget;

    // 定时器
    QTimer *gameTimer;
    QTimer *enemySpawnTimer;
    QTimer *shootTimer;
    QElapsedTimer fpsTimer;

    // 音频组件
    QMediaPlayer *backgroundMusic;
    QAudioOutput *backgroundAudioOutput;
    QMediaPlayer *shootSound;
    QAudioOutput *shootAudioOutput;
    QMediaPlayer *hitSound;
    QAudioOutput *hitAudioOutput;
    QMediaPlayer *gameOverSound;
    QAudioOutput *gameOverAudioOutput;

    // 游戏状态
    QSet<int> pressedKeys;
    int score;
    int wave;
    int enemiesKilled;
    float currentMusicVolume;
    float currentSfxVolume;
    bool gameRunning;
    bool gamePaused;
    int frameCount;
    bool gameInitialized;
};

#endif // GAMEWIDGET_H
