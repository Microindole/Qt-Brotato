#include "gamewidget.h"
#include "ui_gamewidget.h"
#include "resourcemanager.h"
#include <QMessageBox>
#include <QRandomGenerator>

void GameWidget::handleWaveEnd()
{
    gamePaused = true;
    gameTimer->stop();
    enemySpawnTimer->stop();
    shootTimer->stop();
    periodicEffectsTimer->stop();
    pauseBackgroundMusic();

    // 清理场上的子弹和敌人
    qDeleteAll(bullets);
    bullets.clear();
    qDeleteAll(enemies);
    enemies.clear();

    if (pendingLevelUps > 0) {
        // 如果有积攒的升级，进入升级界面
        showUpgradeMenu();
    } else {
        // 否则，进入商店
        showShopScreen();
    }
}

void GameWidget::startNextWave()
{
    if (!gameRunning) return;

    wave++;
    waveTimeLeft = 30;

    hidePauseMenu();
    upgradeWidget->hide();
    shopWidget->hide();

    // 恢复游戏
    gamePaused = false;
    gameTimer->start(16);
    enemySpawnTimer->start();
    shootTimer->start();
    periodicEffectsTimer->start(1000);
    waveTimer->start(30000);

    startBackgroundMusic();
    fpsTimer.start();
    setFocus();
}

void GameWidget::onPlayerLevelUp()
{
    // 不再暂停游戏，只记录升级次数
    pendingLevelUps++;
    updateUpgradeIndicators();
}

void GameWidget::onUpgradeSelected(UpgradeWidget::UpgradeType type)
{
    if (player) {
        switch (type) {
        case UpgradeWidget::MaxHealth:   player->increaseMaxHealth(15); break;
        case UpgradeWidget::AttackPower: player->increaseAttackPower(4); break;
        case UpgradeWidget::Speed:       player->increaseSpeed(0.25f); break;
        case UpgradeWidget::HealthRegen: player->increaseHealthRegen(0.8f); break;
        case UpgradeWidget::Armor:       player->increaseArmor(5); break;
        }
    }

    pendingLevelUps--; // 消耗一次升级机会
    updateUpgradeIndicators();

    if (pendingLevelUps > 0) {
        // 如果还有待处理的升级，再次显示升级菜单
        showUpgradeMenu();
    } else {
        // 所有升级都处理完毕，进入商店
        showShopScreen();
    }
}

void GameWidget::pauseGame()
{
    if (!gameRunning) return;


    gamePaused = true;
    gameTimer->stop();
    enemySpawnTimer->stop();
    shootTimer->stop();
    periodicEffectsTimer->stop();
    waveTimer->stop();
    pauseBackgroundMusic();
    showPauseMenu();

}

void GameWidget::onContinueGame()
{
    if (gamePaused) {
        gamePaused = false;
        hidePauseMenu();
        gameTimer->start(16);
        enemySpawnTimer->start();
        shootTimer->start();
        periodicEffectsTimer->start(1000);
        waveTimer->start(waveTimeLeft * 1000);
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
    m_showHealthBars = visible;
    if (player) player->showHealthBar = m_showHealthBars;

    for (Enemy *enemy : std::as_const(enemies)) {
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
