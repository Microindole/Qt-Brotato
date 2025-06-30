#include "choose.h"
#include "ui_choose.h"
#include "gamedata.h"
#include "resourcemanager.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QShowEvent>

Choose::Choose(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Choose)
    , m_selectedCharacter(Player::AllRounder)
{
    ui->setupUi(this);
    // 注册自定义类型，以便在信号槽中使用
    qRegisterMetaType<MapInfo>("MapInfo");

    // 角色选择
    connect(ui->allrounderButton, &QPushButton::clicked, this, [=](){ onCharacterButtonClicked(Player::AllRounder); });
    connect(ui->fighterButton, &QPushButton::clicked, this, [=](){ onCharacterButtonClicked(Player::Fighter); });
    connect(ui->doctorButton, &QPushButton::clicked, this, [=](){ onCharacterButtonClicked(Player::Doctor); });
    connect(ui->bullButton, &QPushButton::clicked, this, [=](){ onCharacterButtonClicked(Player::Bull); });

    // 连接确认返回按钮的信号
    connect(ui->confirmButton, &QPushButton::clicked, this, &Choose::onConfirmClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &Choose::backToMenuRequested);
}

Choose::~Choose()
{
    delete ui;
}

// 当窗口每次显示时，都重新加载地图，以确保DLC状态是最新的
void Choose::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    loadAndSetupMaps();
}

void Choose::loadAndSetupMaps()
{
    m_availableMaps.clear();
    m_selectedMapKey.clear();
    QFile file(":/configs/maps.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "错误：无法打开 maps.xml";
        return;
    }

    QXmlStreamReader xml(&file);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("错误：无法打开资源文件 ':/configs/maps.xml'。将只使用硬编码的默认地图。");
        // 提供一个备用逻辑，即使没有xml也能玩默认地图
        MapInfo defaultMap;
        defaultMap.key = "default";
        defaultMap.name = "标准地图";
        defaultMap.description = "一片宁静的土地，一切都恰到好处。";
        defaultMap.backgroundPath = ":/images/map.png";
        m_availableMaps.insert(defaultMap.key, defaultMap);
    } else {
        QXmlStreamReader xml(&file);
        while (xml.readNextStartElement()) {
            if (xml.name().toString() == "map") {
                bool isDlcMap = xml.attributes().value("dlc").toString() == "true";
                if (isDlcMap && !GameData::instance().dlc1_active) {
                    xml.skipCurrentElement();
                    continue;
                }
                MapInfo map;
                map.key = xml.attributes().value("key").toString();
                map.name = xml.attributes().value("name").toString();
                map.description = xml.attributes().value("description").toString();
                map.backgroundPath = xml.attributes().value("background").toString();
                QString buffStr = xml.attributes().value("buff").toString();
                if (buffStr == "hp_regen_debuff_0.5") {
                    map.buffType = MapInfo::HpRegenDebuff; map.buffValue = -0.5f;
                } else if (buffStr == "range_debuff_0.2") {
                    map.buffType = MapInfo::AttackRangeDebuff; map.buffValue = 0.8f;
                } else if (buffStr == "speed_debuff_0.2") {
                    map.buffType = MapInfo::SpeedDebuff; map.buffValue = 0.8f;
                }
                m_availableMaps.insert(map.key, map);
            }
        }
        file.close();
    }

    file.close();

    setupMapSelectionUI();

    if (m_availableMaps.isEmpty()) {
        // 如果没有任何地图可选（例如XML文件损坏且没DLC）
        ui->mapPreviewLabel->setText("无可用地图");
        ui->mapDescriptionLabel->setText("错误：找不到任何地图信息！");
        m_selectedMapKey.clear();
    } else if (m_availableMaps.count() == 1) {
        // 如果只有一个地图（通常是默认地图）
        ui->mapSelectionContainer->setVisible(false); // 隐藏地图按钮容器
        // 自动选择并显示这个唯一的地图
        m_selectedMapKey = m_availableMaps.firstKey();
        updateMapDetails(m_selectedMapKey);
    } else {
        // 如果有多个地图
        ui->mapSelectionContainer->setVisible(true); // 显示地图按钮容器
        // 模拟点击第一个地图按钮来触发选择和UI更新
        QLayoutItem* item = ui->mapSelectionContainer->layout()->itemAt(0);
        if(item && item->widget()) {
            static_cast<QPushButton*>(item->widget())->click();
        }
    }
}

void Choose::setupMapSelectionUI()
{
    if (!ui->mapSelectionContainer->layout()) {
        ui->mapSelectionContainer->setLayout(new QVBoxLayout());
    }
    QLayout* layout = ui->mapSelectionContainer->layout();

    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (const auto& mapKey : m_availableMaps.keys()) {
        const MapInfo& map = m_availableMaps.value(mapKey);
        QPushButton* mapButton = new QPushButton(map.name, this);
        mapButton->setCheckable(true);
        mapButton->setAutoExclusive(true);
        connect(mapButton, &QPushButton::clicked, this, [=](){ onMapButtonClicked(mapKey); });
        layout->addWidget(mapButton);
    }
}

void Choose::onCharacterButtonClicked(Player::CharacterType type)
{
    m_selectedCharacter = type;
}

void Choose::onMapButtonClicked(const QString& mapKey)
{
    m_selectedMapKey = mapKey;
    updateMapDetails(mapKey);
}

void Choose::updateMapDetails(const QString& mapKey)
{
    if (!m_availableMaps.contains(mapKey)) return;
    const MapInfo& info = m_availableMaps.value(mapKey);
    ui->mapDescriptionLabel->setText(info.description);
    QPixmap pixmap = ResourceManager::instance().getPixmap(info.backgroundPath);
    if (!pixmap.isNull()) {
        // 2. 将图片缩放到 QLabel 的尺寸，并保持宽高比
        ui->mapPreviewLabel->setPixmap(pixmap.scaled(ui->mapPreviewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        ui->mapPreviewLabel->setText("无法加载预览图");
    }
}

void Choose::onConfirmClicked()
{
    if (!m_selectedMapKey.isEmpty()) {
        GameData::instance().selectedMap = m_availableMaps.value(m_selectedMapKey);
        GameData::instance().selectedCharacter = m_selectedCharacter;
        emit selectionConfirmed();
        this->hide();
    } else {
        QMessageBox::warning(this, "无可用地图", "没有可用的地图。请检查配置或购买DLC。");
    }
}


