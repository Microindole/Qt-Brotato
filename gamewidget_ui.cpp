#include "gamewidget.h"
#include "ui_gamewidget.h"
#include <QRandomGenerator> // <-- 新增：修复 'QRandomGenerator' has not been declared
#include <algorithm>        // <-- 新增：为 std::shuffle 提供支持

void GameWidget::updateUI()
{
    if (player) {
        ui->healthBar->setMaximum(player->getMaxHealth());
        ui->healthBar->setValue(player->getHealth());
        ui->healthBar->setFormat(QString("%1 / %2").arg(player->getHealth()).arg(player->getMaxHealth()));
    
        // --- 新增：更新经验条 ---
        ui->expBar->setMaximum(player->getExpToNextLevel());
        ui->expBar->setValue(player->getExperience());
        ui->expBar->setFormat(QString("LV.%1").arg(player->getLevel()));
        // -----------------------
    }
    ui->scoreLabel->setText(QString("🏆 分数: %1").arg(score));
    ui->waveLabel->setText(QString("🌊 波次: %1").arg(wave));
    ui->enemyCountLabel->setText(QString("👾 敌人: %1").arg(enemies.size()));
}

void GameWidget::showPauseMenu()
{
    if (player) {
        pauseWidget->updateStats(player);
    }
    pauseWidget->resize(this->size());
    pauseWidget->move(0, 0);
    pauseWidget->show();
    pauseWidget->raise();
    pauseWidget->setFocusPolicy(Qt::NoFocus);
}

void GameWidget::hidePauseMenu()
{
    pauseWidget->hide();
}

void GameWidget::connectActions()
{
    // This can be used if you add buttons like "pause" to the game widget UI
}

void GameWidget::showControls() { /* Implementation can be added if needed */ }
void GameWidget::showAbout() { /* Implementation can be added if needed */ }

void GameWidget::showUpgradeMenu()
{
    // 随机生成4个升级选项
    QList<UpgradeWidget::UpgradeType> options;
    QList<UpgradeWidget::UpgradeType> allUpgrades = {
        UpgradeWidget::MaxHealth,
        UpgradeWidget::AttackPower,
        UpgradeWidget::Speed,
        UpgradeWidget::HealthRegen
    };

    // 打乱所有可能的升级项
    std::shuffle(allUpgrades.begin(), allUpgrades.end(), *QRandomGenerator::global());
    
    // 取前4个（或者如果总数小于4，则取全部）
    for(int i = 0; i < qMin(4, allUpgrades.size()); ++i) {
        options.append(allUpgrades[i]);
    }

    upgradeWidget->setUpgradeOptions(options);
    upgradeWidget->resize(this->size());
    upgradeWidget->move(0, 0);
    upgradeWidget->show();
    upgradeWidget->raise();
}
