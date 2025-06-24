#include "gamewidget.h"
#include "ui_gamewidget.h"
#include <QMessageBox>

void GameWidget::pauseGame()
{
    if (!gameRunning) return;

    if (!gamePaused) {
        gamePaused = true;
        gameTimer->stop();
        enemySpawnTimer->stop();
        shootTimer->stop();
        stopBackgroundMusic();
        showPauseMenu();
    }
}

void GameWidget::onContinueGame()
{
    if (gamePaused) {
        gamePaused = false;
        hidePauseMenu();
        gameTimer->start(16);
        enemySpawnTimer->start(2000);
        shootTimer->start(600);
        startBackgroundMusic();
        fpsTimer.start();
        setFocus();
    }
}

void GameWidget::onRestartFromPause()
{
    hidePauseMenu();
    restartGame();
    setFocus();
}

void GameWidget::onEndRun()
{
    hidePauseMenu();
    gameOver();
}

void GameWidget::onOpenSettings()
{
    // 隐藏暂停菜单，显示设置菜单
    pauseWidget->hide();
    
    settingsWidget->setInitialVolumes(currentMusicVolume, currentSfxVolume);
    settingsWidget->resize(this->size());
    settingsWidget->move(0, 0);
    settingsWidget->show();
    settingsWidget->raise();
}

// 添加这些新的槽函数
void GameWidget::onMusicVolumeChanged(float volume)
{
    setMusicVolume(volume);
}

void GameWidget::onSfxVolumeChanged(float volume)
{
    setSfxVolume(volume);
}

void GameWidget::onSettingsClosed()
{
    // 隐藏设置菜单，重新显示暂停菜单
    settingsWidget->hide();
    pauseWidget->show();
    pauseWidget->raise();
}

void GameWidget::onBackToMenu()
{
    hidePauseMenu();
    gameRunning = false;
    gameTimer->stop();
    enemySpawnTimer->stop();
    shootTimer->stop();
    stopBackgroundMusic();
    emit backToMenuRequested();
}

void GameWidget::gameOver()
{
    gameRunning = false;
    gameTimer->stop();
    enemySpawnTimer->stop();
    shootTimer->stop();
    stopBackgroundMusic();
    playGameOverSound();

    QMessageBox::information(this, "游戏结束",
        QString("土豆兄弟倒下了！\n\n你的分数: %1\n到达波次: %2").arg(score).arg(wave));

    emit backToMenuRequested();
}