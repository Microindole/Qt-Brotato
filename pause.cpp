#include "pause.h"
#include "ui_pause.h"
#include "player.h" // 包含Player头文件以获取其定义

Pause::Pause(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Pause)
{
    ui->setupUi(this);
    
    // 设置焦点策略，确保不会抢夺键盘焦点
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
    if (!player) {
        return;
    }

    // 更新UI上的属性值
    ui->levelValueLabel->setText(QString::number(player->getLevel()));
    ui->maxHealthValueLabel->setText(QString::number(player->getMaxHealth()));
    ui->healthRegenValueLabel->setText(QString::number(player->getHealthRegen(), 'f', 1));
    ui->attackPowerValueLabel->setText(QString::number(player->getAttackPower()));
    ui->armorValueLabel->setText(QString::number(player->getArmor()));
    ui->speedValueLabel->setText(QString::number(player->getSpeed(), 'f', 1));
    ui->attackRangeValueLabel->setText(QString::number(player->getAttackRange(), 'f', 1)); // 更新攻击距离
}

void Pause::onContinueClicked()
{
    this->hide();
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
