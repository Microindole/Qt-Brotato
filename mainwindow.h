// #ifndef MAINWINDOW_H
// #define MAINWINDOW_H

// #include <QMainWindow>
// #include <QGraphicsView>
// #include <QGraphicsScene>
// #include <QTimer>
// #include <QKeyEvent>
// #include <QLabel>
// #include <QProgressBar>
// #include <QSet>
// #include <QList>
// #include <QElapsedTimer>
// #include <QMediaPlayer>
// #include <QAudioOutput>
// #include "player.h"
// #include "enemy.h"
// #include "bullet.h"

// QT_BEGIN_NAMESPACE
// namespace Ui {
// class MainWindow;
// }
// QT_END_NAMESPACE

// class MainWindow : public QMainWindow
// {
//     Q_OBJECT

// public:
//     MainWindow(QWidget *parent = nullptr);
//     ~MainWindow();

// protected:
//     void keyPressEvent(QKeyEvent *event) override;
//     void keyReleaseEvent(QKeyEvent *event) override;
//     void resizeEvent(QResizeEvent *event) override; // <-- 添加这一行

// private slots:
//     void updateGame();
//     void spawnEnemy();
//     void shootBullets();
//     void gameOver();
//     void restartGame();
//     void pauseGame();
//     void showControls();
//     void showAbout();

// private:
//     Ui::MainWindow *ui;

//     // 游戏组件
//     QGraphicsScene *gameScene;
//     Player *player;
//     QList<Enemy*> enemies;
//     QList<Bullet*> bullets;

//     // 定时器
//     QTimer *gameTimer;
//     QTimer *enemySpawnTimer;
//     QTimer *shootTimer;
//     QElapsedTimer fpsTimer;

//     // 音频组件
//     QMediaPlayer *backgroundMusic;
//     QAudioOutput *backgroundAudioOutput;
//     QMediaPlayer *shootSound;
//     QAudioOutput *shootAudioOutput;
//     QMediaPlayer *hitSound;
//     QAudioOutput *hitAudioOutput;
//     QMediaPlayer *gameOverSound;
//     QAudioOutput *gameOverAudioOutput;

//     // 游戏状态
//     QSet<int> pressedKeys;
//     int score;
//     int wave;
//     int enemiesKilled;
//     bool gameRunning;
//     bool gamePaused;
//     int frameCount;
//     bool gameInitialized; // 新增：标记游戏是否已初始化

//     void setupGame();
//     void setupAudio();  // 新增音频设置函数
//     void movePlayer();
//     void checkCollisions();
//     void cleanupObjects();
//     void updateWave();
//     void updateUI();
//     void connectActions();

//     // 音频播放函数
//     void playShootSound();
//     void playHitSound();
//     void playGameOverSound();
//     void startBackgroundMusic();
//     void stopBackgroundMusic();
// };

// #endif // MAINWINDOW_H
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// 前向声明
class Menu;
class GameWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showGameScreen();
    void showMenuScreen();
    void exitApplication();

private:
    Ui::MainWindow *ui;
    Menu *menuWidget;
    GameWidget *gameWidget;
};

#endif // MAINWINDOW_H