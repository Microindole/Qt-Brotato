#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QKeyEvent>
#include <QLabel>
#include <QProgressBar>
#include <QSet>
#include <QList>
#include <QElapsedTimer>
#include "player.h"
#include "enemy.h"
#include "bullet.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void updateGame();
    void spawnEnemy();
    void shootBullets();
    void gameOver();
    void restartGame();
    void pauseGame();
    void showControls();
    void showAbout();
    
    // 新增的界面切换槽函数
    void startGame();
    void backToMenu();
    void showHelp();
    void exitGame();

private:
    Ui::MainWindow *ui;
    
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
    
    // 游戏状态
    QSet<int> pressedKeys;
    int score;
    int wave;
    int enemiesKilled;
    bool gameRunning;
    bool gamePaused;
    int frameCount;
    bool gameInitialized; // 新增：标记游戏是否已初始化
    
    void setupGame();
    void movePlayer();
    void checkCollisions();
    void cleanupObjects();
    void updateWave();
    void updateUI();
    void connectActions();
    void showStartScreen(); // 新增：显示开始界面
    void showGameScreen();  // 新增：显示游戏界面
};

#endif // MAINWINDOW_H
