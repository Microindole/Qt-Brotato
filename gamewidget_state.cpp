#include "gamewidget.h"
#include "ui_gamewidget.h"
#include "resourcemanager.h"
#include <QMessageBox>
#include <QRandomGenerator>

void GameWidget::pauseGame()
{
    if (!gameRunning) return;

    if (!gamePaused) {
        gamePaused = true;
        gameTimer->stop();
        enemySpawnTimer->stop();
        shootTimer->stop();
        periodicEffectsTimer->stop();
        pauseBackgroundMusic();
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
        periodicEffectsTimer->start(1000);
        resumeBackgroundMusic();
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
    settingsWidget->setInitialHealthBarVisibility(m_showHealthBars);
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

void GameWidget::onHealthBarVisibilityChanged(bool visible)
{
    // 1. 更新 GameWidget 的全局设置
    m_showHealthBars = visible;

    // 2. 将新设置应用到玩家
    if (player) {
        player->showHealthBar = m_showHealthBars;
    }

    // 3. 遍历当前屏幕上所有的敌人，并应用新设置
    for (Enemy *enemy : enemies) {
        enemy->showHealthBar = m_showHealthBars;
    }
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
    periodicEffectsTimer->stop();
    stopBackgroundMusic();
    ResourceManager::instance().playSound("gameover");

    QMessageBox::information(this, "游戏结束",
        QString("土豆兄弟倒下了！\n\n你的分数: %1\n到达波次: %2").arg(score).arg(wave));

    emit backToMenuRequested();
}

void GameWidget::onPlayerLevelUp()
{
    if (gamePaused) return; // 如果游戏已经暂停（比如在暂停菜单），则不处理升级

    gamePaused = true;
    gameTimer->stop();
    enemySpawnTimer->stop();
    shootTimer->stop();
    periodicEffectsTimer->stop();
    pauseBackgroundMusic(); // 升级时暂停背景音乐

    showUpgradeMenu();
}

void GameWidget::onUpgradeSelected(UpgradeWidget::UpgradeType type)
{
    if (player) {
        switch (type) {
            case UpgradeWidget::MaxHealth:
                player->increaseMaxHealth(20);
                break;
            case UpgradeWidget::AttackPower:
                player->increaseAttackPower(5);
                break;
            case UpgradeWidget::Speed:
                player->increaseSpeed(0.3f);
                break;
            case UpgradeWidget::HealthRegen:
                player->increaseHealthRegen(1.0f);
                break;
        }
    }

    upgradeWidget->hide();

    // 恢复游戏
    gamePaused = false;
    gameTimer->start(16);
    enemySpawnTimer->start(); // 使用它之前的间隔
    shootTimer->start();      // 使用它之前的间隔
    periodicEffectsTimer->start(1000);
    resumeBackgroundMusic();
    fpsTimer.start();
    setFocus();
}
