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
    connect(ui->refreshButton, &QPushButton::clicked, this, &Shop::handleRefreshClicked);
}

Shop::~Shop()
{
    delete ui;
}

void Shop::showShop(Player *player)
{
    if (!player) return;
    m_player = player;
    if (m_purchasedUniqueItems.isEmpty()) {
        refreshItemList();
    }
    updateCoinDisplay();
    refreshItemList();
    this->show();
    this->raise();
}

void Shop::setupItems()
{
    // 在这里定义所有可能在商店中出现的物品
    m_allItems.clear();
    m_allItems.append({"heal_potion", "生命药水", "+25 生命值", ":/images/item-heal.png", 15, false});
    m_allItems.append({"glove", "力量手套", "+4 攻击力", ":/images/item-glove.png", 32, false});
    m_allItems.append({"helmet", "钢板护甲", "+5 护甲", ":/images/item-Helmet.png", 30, false});
    m_allItems.append({"wings", "速度之翼", "+10% 移动速度", ":/images/item-Wings.png", 35, false});
    m_allItems.append({"scope", "望远镜", "+50 攻击距离", ":/images/item-Scope.png", 25, false});
    m_allItems.append({"plant", "生命之源", "+0.5 生命再生", ":/images/item-Plant.png", 40, true});
    m_allItems.append({"potato", "神奇土豆", "全属性微量提升", ":/images/item-Potato.png", 45, false});
    m_allItems.append({"glasses", "工程师眼镜", "+20% 攻击距离, -5% 攻击力", ":/images/item-Glasses.png", 20, true});
    m_allItems.append({"scared_sausage", "害怕的香肠", "+20% 移速, -10% 最大生命值", ":/images/item-Scared_Sausage.png", 25, true});
    m_allItems.append({"weird_ghost", "诡异的幽灵", "+15% 经验获取", ":/images/item-Weird_Ghost.png", 35, true});
}

void Shop::refreshItemList()
{
    if (!m_player) return;
    QGridLayout* layout = qobject_cast<QGridLayout*>(ui->itemsGridContainer->layout());
    if (!layout) return;

    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

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
        iconLabel->setPixmap(ResourceManager::instance().getPixmap(shopItem.iconPath).scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
        buyButton->setProperty("isUnique", shopItem.isUnique);

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
    updateButtonsState();
}

void Shop::updateButtonsState()
{
    if (!m_player) return;
    ui->refreshButton->setEnabled(m_player->getCoins() >= 10);
    QList<QPushButton*> buttons = ui->itemsGridContainer->findChildren<QPushButton*>();
    for(QPushButton* btn : std::as_const(buttons)) {
        QString itemId = btn->property("itemId").toString();
        bool isUnique = btn->property("isUnique").toBool();
        if (isUnique && m_purchasedUniqueItems.contains(itemId)) {
            btn->setEnabled(false);
            btn->setText("已拥有");
            continue;
        }

        int itemPrice = btn->property("itemPrice").toInt();
        btn->setEnabled(m_player->getCoins() >= itemPrice);
    }
}

void Shop::updateCoinDisplay()
{
    if (m_player) {
        ui->coinLabel->setText(QString("💰 %1").arg(m_player->getCoins()));
    }
}

void Shop::handleContinueClicked(){ emit continueToNextWave();}
void Shop::handleBackToMenuClicked(){ emit backToMenu();}

void Shop::handleItemPurchaseClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button && m_player) {
        int price = button->property("itemPrice").toInt();
        if (m_player->getCoins() >= price) {
            m_player->addCoins(-price);
            QString itemId = button->property("itemId").toString();
            bool isUnique = button->property("isUnique").toBool();
            if(isUnique) m_purchasedUniqueItems.insert(itemId);
            emit itemPurchased(itemId);

            button->setEnabled(false);
            button->setText("已购买");

            updateCoinDisplay();

            updateButtonsState();
        }
    }
}

void Shop::handleRefreshClicked()
{
    if (m_player && m_player->getCoins() >= 10) {
        m_player->addCoins(-10);
        ResourceManager::instance().playSound("push");
        updateCoinDisplay();
        refreshItemList();
        updateButtonsState();
    }
}

void Shop::clearPurchaseHistory()
{
    m_purchasedUniqueItems.clear();
}
