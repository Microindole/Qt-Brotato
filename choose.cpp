#include "choose.h"
#include "ui_choose.h"
#include "gamedata.h"
#include "resourcemanager.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QVBoxLayout>
#include <QHBoxLayout>
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
    qRegisterMetaType<MapInfo>("MapInfo");

    // 连接角色选择按钮
    connect(ui->allrounderButton, &QPushButton::clicked, this, [=](){ onCharacterButtonClicked(Player::AllRounder); });
    connect(ui->fighterButton, &QPushButton::clicked, this, [=](){ onCharacterButtonClicked(Player::Fighter); });
    connect(ui->doctorButton, &QPushButton::clicked, this, [=](){ onCharacterButtonClicked(Player::Doctor); });
    connect(ui->bullButton, &QPushButton::clicked, this, [=](){ onCharacterButtonClicked(Player::Bull); });

    connect(ui->mapButton_default, &QPushButton::clicked, this, [=](){ onMapButtonClicked("default"); });
    connect(ui->mapButton_lava, &QPushButton::clicked, this, [=](){ onMapButtonClicked("lava"); });
    connect(ui->mapButton_desert, &QPushButton::clicked, this, [=](){ onMapButtonClicked("desert"); });
    connect(ui->mapButton_cave, &QPushButton::clicked, this, [=](){ onMapButtonClicked("cave"); });

    // 连接确认和返回按钮
    connect(ui->confirmButton, &QPushButton::clicked, this, &Choose::onConfirmClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &Choose::backToMenuRequested);

    QFile file(":/configs/maps.xml");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QXmlStreamReader xml(&file);
        while (xml.readNextStartElement()) {
            if (xml.name().toString() == "map") {
                MapInfo map;
                map.key = xml.attributes().value("key").toString();
                map.name = xml.attributes().value("name").toString();
                map.description = xml.attributes().value("description").toString();
                map.backgroundPath = xml.attributes().value("background").toString();

                // Simplified buff parsing
                QString buffStr = xml.attributes().value("buff").toString();
                if (buffStr.startsWith("hp_regen_debuff")) map.buffType = MapInfo::HpRegenDebuff, map.buffValue = -0.5f;
                else if (buffStr.startsWith("range_debuff")) map.buffType = MapInfo::AttackRangeDebuff, map.buffValue = 0.8f;
                else if (buffStr.startsWith("speed_debuff")) map.buffType = MapInfo::SpeedDebuff, map.buffValue = 0.8f;

                m_allMaps.insert(map.key, map);
            }
        }
        file.close();
    } else {
        qWarning() << "CRITICAL: Could not open maps.xml!";
    }
}

Choose::~Choose()
{
    delete ui;
}

void Choose::showEvent(QShowEvent *event)
{
    qDebug() << "[Choose] showEvent: dlc1_active is" << GameData::instance().dlc1_active;
    // 首先调用基类的 showEvent，这是个好习惯
    QWidget::showEvent(event);
    setupMapButtons();
}

void Choose::setupMapButtons()
{
    bool dlc_is_active = GameData::instance().dlc1_active;

    // Iterate through all known maps and set visibility
    for (const auto& mapKey : m_allMaps.keys()) {
        QPushButton* button = findChild<QPushButton*>("mapButton_" + mapKey);
        if (!button) {
            qWarning() << "Could not find button for map:" << mapKey;
            continue;
        }

        bool isDlcMap = (mapKey != "default"); // A simple check if the map is DLC

        // The core logic: show if it's not a DLC map, OR if DLC is active
        button->setVisible(!isDlcMap || dlc_is_active);
    }

    // Automatically click the first visible button to show a default selection
    if (ui->mapButton_default->isVisible()) {
        ui->mapButton_default->click();
    } else if (ui->mapButton_lava->isVisible()) {
        // If default is hidden but DLC is active, click the first DLC map
        ui->mapButton_lava->click();
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
    if (!m_allMaps.contains(mapKey)) return;

    const MapInfo& info = m_allMaps.value(mapKey);
    ui->mapDescriptionLabel->setText(info.description);

    QPixmap pixmap = ResourceManager::instance().getPixmap(info.backgroundPath);
    if (!pixmap.isNull()) {
        ui->mapPreviewLabel->setPixmap(pixmap.scaled(ui->mapPreviewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        ui->mapPreviewLabel->setText("无法加载预览图");
    }
}

void Choose::onConfirmClicked()
{
    if (m_selectedMapKey.isEmpty()) {
        QMessageBox::warning(this, "选择地图", "请先选择一张地图！");
        return;
    }

    bool isDlcMap = (m_selectedMapKey != "default");

    // 如果是DLC地图但DLC未激活，则显示警告并阻止继续
    if (isDlcMap && !GameData::instance().dlc1_active) {
        QMessageBox::warning(this, "需要DLC", "选择该地图需要购买并启用“地图拓展包”！");
        return;
    }

    GameData::instance().selectedMap = m_allMaps.value(m_selectedMapKey);
    GameData::instance().selectedCharacter = m_selectedCharacter;
    emit selectionConfirmed();
}
