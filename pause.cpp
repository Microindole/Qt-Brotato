#include "pause.h"
#include "ui_pause.h"
#include "player.h"

Pause::Pause(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Pause)
{
    ui->setupUi(this);
    setFocusPolicy(Qt::NoFocus);
    
    // 连接信号和槽
    connect(ui->continueButton, &QPushButton::clicked, this, &Pause::onContinueClicked);
    connect(ui->restartButton, &QPushButton::clicked, this, &Pause::onRestartClicked);
    connect(ui->endRunButton, &QPushButton::clicked, this, &Pause::onEndRunClicked);
    connect(ui->settingsButton, &QPushButton::clicked, this, &Pause::onSettingsClicked);
    connect(ui->backToMenuButton, &QPushButton::clicked, this, &Pause::onBackToMenuClicked);
}

Pause::~Pause()
{
    delete ui;
}

void Pause::updateStats(const Player *player)
{
    if (!player) return;
    // 更新UI上的属性值
    ui->levelValueLabel->setText(QString::number(player->getLevel()));
    ui->maxHealthValueLabel->setText(QString::number(player->getMaxHealth()));
    ui->healthRegenValueLabel->setText(QString::number(player->getHealthRegen(), 'f', 1));
    ui->attackPowerValueLabel->setText(QString::number(player->getAttackPower()));
    ui->armorValueLabel->setText(QString::number(player->getArmor()));
    ui->speedValueLabel->setText(QString::number(player->getSpeed(), 'f', 1));
    ui->attackRangeValueLabel->setText(QString::number(player->getAttackRange(), 'f', 1));
}

void Pause::showForPause()
{
    ui->titleLabel->setText("游戏暂停");
    ui->continueButton->setText("继续");
    ui->restartButton->show();
    ui->endRunButton->show();
    ui->settingsButton->show();
    ui->backToMenuButton->show();
}

void Pause::showForWaveComplete(int waveNumber)
{
    ui->titleLabel->setText("波次 " + QString::number(waveNumber) + " 完成!");
    ui->continueButton->setText("开始下一波");
    ui->restartButton->hide();
    ui->endRunButton->hide();
    ui->settingsButton->hide();
    ui->backToMenuButton->hide();
}

void Pause::onContinueClicked()
{
    emit continueGame();
}

void Pause::onRestartClicked()
{
    emit restartGame();
}

void Pause::onEndRunClicked()
{
    emit endRun();
}

void Pause::onSettingsClicked()
{
    emit openSettings();
}

void Pause::onBackToMenuClicked()
{
    emit backToMenu();
}
