#include "shop.h"
#include "ui_shop.h"
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QRandomGenerator>
#include <algorithm>
#include "resourcemanager.h"

Shop::Shop(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Shop)
    , m_player(nullptr)
{
    ui->setupUi(this);
    setupItems();

    connect(ui->continueButton, &QPushButton::clicked, this, &Shop::handleContinueClicked);
    connect(ui->backToMenuButton, &QPushButton::clicked, this, &Shop::handleBackToMenuClicked);
}

Shop::~Shop()
{
    delete ui;
}

void Shop::showShop(Player *player)
{
    if (!player) {
        qWarning() << "Shop::showShop - Player is null!";
        return;
    }
    m_player = player;
    updateCoinDisplay();
    refreshItemList();
    this->show();
    this->raise();
}

void Shop::setupItems()
{
    // 在这里定义所有可能在商店中出现的物品
    m_allItems.append({"health_potion", "生命药水", "+20 最大生命值", ":/images/upgrade.png", 50});
    m_allItems.append({"attack_boost", "力量手套", "+5 攻击力", ":/images/upgrade.png", 75});
    m_allItems.append({"speed_boots", "速度之靴", "+0.3 移动速度", ":/images/upgrade.png", 60});
    m_allItems.append({"armor_plate", "钢板护甲", "+5 护甲", ":/images/upgrade.png", 65});
    // 您可以继续在这里添加更多有趣的道具
}

void Shop::refreshItemList()
{
    if (!m_player) return;

    // 清理旧的商品项
    QGridLayout* layout = qobject_cast<QGridLayout*>(ui->itemsGridContainer->layout());
    if (!layout) return;

    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // 随机选择4个商品进行展示
    std::shuffle(m_allItems.begin(), m_allItems.end(), *QRandomGenerator::global());
    int itemsToShow = qMin(4, m_allItems.size());

    for (int i = 0; i < itemsToShow; ++i) {
        const auto& shopItem = m_allItems[i];

        QWidget* itemWidget = new QWidget(this);
        itemWidget->setObjectName("itemCard");
        QVBoxLayout* itemLayout = new QVBoxLayout(itemWidget);
        itemLayout->setSpacing(10);
        itemLayout->setAlignment(Qt::AlignCenter);

        QLabel* iconLabel = new QLabel(this);
        iconLabel->setPixmap(ResourceManager::instance().getPixmap(shopItem.iconPath).scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        iconLabel->setAlignment(Qt::AlignCenter);
        itemLayout->addWidget(iconLabel);

        QLabel* nameLabel = new QLabel(shopItem.name, this);
        nameLabel->setObjectName("itemName");
        nameLabel->setAlignment(Qt::AlignCenter);
        itemLayout->addWidget(nameLabel);

        QLabel* descLabel = new QLabel(shopItem.description, this);
        descLabel->setObjectName("itemDesc");
        descLabel->setAlignment(Qt::AlignCenter);
        descLabel->setWordWrap(true);
        itemLayout->addStretch();
        itemLayout->addWidget(descLabel);
        itemLayout->addStretch();

        QPushButton* buyButton = new QPushButton(QString("购买 (%1)").arg(shopItem.price), this);
        buyButton->setProperty("itemId", shopItem.id);
        buyButton->setProperty("itemPrice", shopItem.price);

        // 如果金币不足，则禁用按钮
        if (m_player->getCoins() < shopItem.price) {
            buyButton->setEnabled(false);
        }

        connect(buyButton, &QPushButton::clicked, this, &Shop::handleItemPurchaseClicked);
        itemLayout->addWidget(buyButton);

        int row = i / 2;
        int col = i % 2;
        layout->addWidget(itemWidget, row, col);
    }
}

void Shop::updateCoinDisplay()
{
    if (m_player) {
        ui->coinLabel->setText(QString("💰 %1").arg(m_player->getCoins()));
    }
}

void Shop::handleContinueClicked()
{
    emit continueToNextWave();
}

void Shop::handleBackToMenuClicked()
{
    emit backToMenu();
}

void Shop::handleItemPurchaseClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button && m_player) {
        int price = button->property("itemPrice").toInt();
        if (m_player->getCoins() >= price) {
            m_player->addCoins(-price);
            QString itemId = button->property("itemId").toString();
            emit itemPurchased(itemId);

            button->setEnabled(false);
            button->setText("已购买");

            updateCoinDisplay();

            QList<QPushButton*> buttons = ui->itemsGridContainer->findChildren<QPushButton*>();
            for(QPushButton* btn : std::as_const(buttons)) {
                if(btn->isEnabled()) {
                    int itemPrice = btn->property("itemPrice").toInt();
                    if(m_player->getCoins() < itemPrice) {
                        btn->setEnabled(false);
                    }
                }
            }
        }
    }
}
