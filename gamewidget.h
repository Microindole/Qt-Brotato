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

namespace Ui {
class GameWidget;
}

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = nullptr);
    ~GameWidget();
    
    void startGame(); // 开始游戏的公共接口

signals:
    void backToMenuRequested(); // 返回菜单信号

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
    
    // 暂停界面相关槽函数
    void onContinueGame();
    void onRestartFromPause();
    void onEndRun();
    void onOpenSettings();
    void onBackToMenu();

    // 新增设置相关的槽函数
    void onMusicVolumeChanged(float volume);
    void onSfxVolumeChanged(float volume);
    void onSettingsClosed();

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
    void showPauseMenu();    // 显示暂停菜单
    void hidePauseMenu();    // 隐藏暂停菜单
    
    // 音频函数
    void playShootSound();
    void playHitSound();
    void playGameOverSound();
    void startBackgroundMusic();
    void stopBackgroundMusic();

    // 新增音量控制方法
    void setMusicVolume(float volume);
    void setSfxVolume(float volume);

    Ui::GameWidget *ui;
    
    // 游戏组件
    QGraphicsScene *gameScene;
    Player *player;
    QList<Enemy*> enemies;
    QList<Bullet*> bullets;
    Pause *pauseWidget;
    Settings *settingsWidget; // 添加

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
    float currentMusicVolume; // 添加
    float currentSfxVolume;   // 添加
    bool gameRunning;
    bool gamePaused;
    int frameCount;
    bool gameInitialized;
};

#endif // GAMEWIDGET_H
