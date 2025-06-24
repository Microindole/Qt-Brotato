#include "gamewidget.h"
#include "ui_gamewidget.h"

void GameWidget::updateUI()
{
    if (player) {
        ui->healthBar->setMaximum(player->getMaxHealth());
        ui->healthBar->setValue(player->getHealth());
        ui->healthBar->setFormat(QString("%1 / %2").arg(player->getHealth()).arg(player->getMaxHealth()));
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