#include "menu.h"
#include "ui_menu.h"
#include "payment_manager.h"
#include "gamedata.h"
#include <QGridLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QCheckBox>

Menu::Menu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);

    // 背景集成代码
    m_backgroundWidget = new DynamicBackgroundWidget(this);
    auto *layout = qobject_cast<QGridLayout*>(this->layout());
    if (layout) {
        layout->addWidget(m_backgroundWidget, 0, 0, -1, -1);
    }
    m_backgroundWidget->lower();

    // 连接信号
    connect(ui->startGameButton, &QPushButton::clicked, this, &Menu::startGameRequested);
    connect(ui->exitButton, &QPushButton::clicked, this, &Menu::exitGameRequested);


    AnimatedCharacterWidget *animatedCharacter = new AnimatedCharacterWidget(this);

    if (layout) {
        layout->replaceWidget(ui->characterImageLabel, animatedCharacter);
        delete ui->characterImageLabel;
        ui->characterImageLabel = nullptr;
    }


    // 支付
    m_paymentManager = new PaymentManager(this);
    auto* leftLayout = ui->leftButtonsContainer;

    m_dlcButton = new QPushButton("地图DLC", this);
    if (leftLayout) {leftLayout->insertWidget(2, m_dlcButton);}

    // “地图拓展包”开关
    m_dlcSwitch = new QCheckBox("地图拓展包", this);
    m_dlcSwitch->setChecked(false);
    m_dlcSwitch->setVisible(false); // 初始时隐藏
    if(leftLayout) { leftLayout->insertWidget(3, m_dlcSwitch); }

    connect(m_dlcButton, &QPushButton::clicked, this, &Menu::onPurchaseDlcClicked);
    connect(m_paymentManager, &PaymentManager::purchaseSuccess, this, &Menu::onPurchaseSuccess);
    connect(m_paymentManager, &PaymentManager::purchaseFailed, this, &Menu::onPurchaseFailed);
    connect(m_dlcSwitch, &QCheckBox::toggled, this, &Menu::onDlcSwitchToggled);
}

Menu::~Menu()
{
    delete ui;
}

void Menu::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updateDlcUi();
}

void Menu::updateDlcUi()
{
    if (GameData::instance().dlc1_purchased) {
        m_dlcButton->setVisible(false);
        m_dlcSwitch->setVisible(true);
        m_dlcSwitch->setChecked(GameData::instance().dlc1_active);
    } else {
        m_dlcButton->setVisible(true);
        m_dlcSwitch->setVisible(false);
    }
}

void Menu::onPurchaseDlcClicked()
{
    m_paymentManager->purchaseDlc("dlc_map_pack_1", 0.01, "熔岩与沙漠地图包");
}

void Menu::onPurchaseSuccess(const QString& dlcId)
{
    QMessageBox::information(this, "支付成功", "您已成功购买地图拓展包！感谢您的支持！");
    // 更新全局状态，标记DLC已购买并激活
    GameData::instance().dlc1_purchased = true;
    GameData::instance().dlc1_active = true;
    updateDlcUi();
}

void Menu::onPurchaseFailed(const QString& reason)
{
    QMessageBox::warning(this, "支付失败", "购买失败: " + reason);
}

void Menu::onDlcSwitchToggled(bool checked)
{
    // 当玩家点击开关时，更新全局的DLC激活状态
    GameData::instance().dlc1_active = checked;
    qDebug() << "地图拓展包状态: " << (checked ? "已启用" : "已禁用");
}
