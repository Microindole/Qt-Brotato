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
    
    // 音频函数
    void playShootSound();
    void playHitSound();
    void playGameOverSound();
    void startBackgroundMusic();
    void stopBackgroundMusic();

    Ui::GameWidget *ui;
    
    // 游戏组件
    QGraphicsScene *gameScene;
    Player *player;
    QList<Enemy*> enemies;
    QList<Bullet*> bullets;

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
    bool gameRunning;
    bool gamePaused;
    int frameCount;
    bool gameInitialized;
};

#endif // GAMEWIDGET_H
