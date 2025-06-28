#include "gamewidget.h"
#include "ui_gamewidget.h"
#include "resourcemanager.h"
#include <QLabel>
#include <QRandomGenerator>
#include <algorithm>

void GameWidget::updateUI()
{
    if (player) {
        ui->healthBar->setMaximum(player->getMaxHealth());
        ui->healthBar->setValue(player->getHealth());
        ui->healthBar->setFormat(QString("%1 / %2").arg(player->getHealth()).arg(player->getMaxHealth()));
    
        // 更新经验条
        ui->expBar->setMaximum(player->getExpToNextLevel());
        ui->expBar->setValue(player->getExperience());
        ui->expBar->setFormat(QString("LV.%1").arg(player->getLevel()));

        ui->coinLabel->setText(QString("💰 金币: %1").arg(player->getCoins()));
        
    }
    ui->scoreLabel->setText(QString("🏆 分数: %1").arg(score));
    ui->waveLabel->setText(QString("🌊 波次: %1").arg(wave));
    ui->enemyCountLabel->setText(QString("👾 敌人: %1").arg(enemies.size()));
    ui->waveTimerLabel->setText(QString("⏳ %1s").arg(waveTimeLeft));
}

void GameWidget::updateUpgradeIndicators()
{
    qDeleteAll(m_upgradeIndicators);
    m_upgradeIndicators.clear();

    if (pendingLevelUps <= 0) return;

    QPixmap icon = ResourceManager::instance().getPixmap(":/images/upgrade.png");
    if (icon.isNull()) return;
    int iconSize = 80; // 图标大小
    int padding = 20;  // 图标之间的间距
    int margin = 20;   // 距离窗口边缘的间距

    for (int i = 0; i < pendingLevelUps; ++i) {
        QLabel* indicator = new QLabel(this); // 将GameWidget设为父对象
        indicator->setPixmap(icon.scaled(iconSize, iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        indicator->setFixedSize(iconSize, iconSize);

        // 计算位置：从右上角开始，水平向左排列
        int x = this->width() - margin - iconSize - (i * (iconSize + padding));
        int y = margin + ui->statsFrame->height(); // 在状态栏下方显示

        indicator->move(x, y);
        indicator->show();
        indicator->raise(); // 确保图标在最上层
        m_upgradeIndicators.append(indicator);
    }
}

void GameWidget::showPauseMenu()
{
    if (player) {
        pauseWidget->updateStats(player);
    }
    pauseWidget->showForPause();
    pauseWidget->resize(this->size());
    pauseWidget->move(0, 0);
    pauseWidget->show();
    pauseWidget->raise();
}

void GameWidget::showWaveCompleteScreen()
{
    if (player) {
        pauseWidget->updateStats(player);
    }
    pauseWidget->showForWaveComplete(wave);
    pauseWidget->resize(this->size());
    pauseWidget->move(0, 0);
    pauseWidget->show();
    pauseWidget->raise();
}

void GameWidget::hidePauseMenu()
{
    pauseWidget->hide();
}


void GameWidget::showUpgradeMenu()
{
    // 随机生成4个升级选项
    QList<UpgradeWidget::UpgradeType> options;
    QList<UpgradeWidget::UpgradeType> allUpgrades = {
        UpgradeWidget::MaxHealth,
        UpgradeWidget::AttackPower,
        UpgradeWidget::Speed,
        UpgradeWidget::HealthRegen,
        UpgradeWidget::Armor
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
