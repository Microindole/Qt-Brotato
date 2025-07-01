#include "choose.h"
#include "ui_choose.h"
#include "gamedata.h"
#include "resourcemanager.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QShowEvent>

Choose::Choose(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Choose)
    , m_selectedCharacter(Player::AllRounder) // Default character
{
    ui->setupUi(this);
    qRegisterMetaType<MapInfo>("MapInfo");

    // 这是地图信息的唯一来源
    QFile file(":/configs/maps.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "CRITICAL: Could not open maps.xml! Map selection will not work.";
        return; // 如果文件打不开，直接返回
    }

    qDebug() << "Successfully opened :/configs/maps.xml. Parsing...";
    QXmlStreamReader xml(&file);

    // 使用更稳妥的循环方式来解析XML
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (xml.name().toString() == "map") {
                MapInfo map;
                map.key = xml.attributes().value("key").toString();
                map.name = xml.attributes().value("name").toString();
                map.description = xml.attributes().value("description").toString();
                map.backgroundPath = xml.attributes().value("background").toString();

                QString buffStr = xml.attributes().value("buff").toString();
                if (buffStr.startsWith("hp_regen_debuff")) {
                    map.buffType = MapInfo::HpRegenDebuff;
                    map.buffValue = -0.5f;
                } else if (buffStr.startsWith("range_debuff")) {
                    map.buffType = MapInfo::AttackRangeDebuff;
                    map.buffValue = 0.8f;
                } else if (buffStr.startsWith("speed_debuff")) {
                    map.buffType = MapInfo::SpeedDebuff;
                    map.buffValue = 0.8f;
                }

                m_allMaps.insert(map.key, map);
                // 增加调试信息，确认每张地图都被加载
                qDebug() << "Loaded map from XML with key:" << map.key;
            }
        }
    }

    if (xml.hasError()) {
        qWarning() << "XML parsing error:" << xml.errorString();
    }
    qDebug() << "Finished parsing maps.xml. Total maps loaded:" << m_allMaps.count();
    file.close();



    connect(ui->allrounderButton, &QPushButton::clicked, this, [=](){ onCharacterButtonClicked(Player::AllRounder); });
    connect(ui->fighterButton, &QPushButton::clicked, this, [=](){ onCharacterButtonClicked(Player::Fighter); });
    connect(ui->doctorButton, &QPushButton::clicked, this, [=](){ onCharacterButtonClicked(Player::Doctor); });
    connect(ui->bullButton, &QPushButton::clicked, this, [=](){ onCharacterButtonClicked(Player::Bull); });

    // 将所有地图按钮连接到同一个槽函数，代码更简洁
    connect(ui->mapButton_default, &QPushButton::clicked, this, &Choose::onMapButtonClicked);
    connect(ui->mapButton_lava, &QPushButton::clicked, this, &Choose::onMapButtonClicked);
    connect(ui->mapButton_desert, &QPushButton::clicked, this, &Choose::onMapButtonClicked);
    connect(ui->mapButton_cave, &QPushButton::clicked, this, &Choose::onMapButtonClicked);

    connect(ui->confirmButton, &QPushButton::clicked, this, &Choose::onConfirmClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &Choose::backToMenuRequested);
}

Choose::~Choose()
{
    delete ui;
}

// 每次显示窗口时都会调用此函数
void Choose::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    // 每次都重新设置初始状态，以正确反映DLC的激活情况
    setupInitialState();
}

// 此函数现在负责处理按钮的可见性和默认选择
void Choose::setupInitialState()
{
    bool dlc_is_active = GameData::instance().dlc1_active;

    // 根据DLC激活状态设置按钮的可见性
    ui->mapButton_lava->setVisible(dlc_is_active);
    ui->mapButton_desert->setVisible(dlc_is_active);
    ui->mapButton_cave->setVisible(dlc_is_active);

    // 保证默认地图按钮总是可见的
    ui->mapButton_default->setVisible(true);

    // 这可以确保UI状态和数据模型从一开始就是同步和有效的。
    ui->allrounderButton->click();
    ui->mapButton_default->click();
}

void Choose::onCharacterButtonClicked(Player::CharacterType type)
{
    m_selectedCharacter = type;
}

// 这个槽函数现在处理所有地图按钮的点击事件
void Choose::onMapButtonClicked()
{
    // 判断是哪个按钮发送了信号
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    // 从按钮的 objectName 中提取地图的 key (例如 "mapButton_lava" -> "lava")
    QString mapKey = button->objectName().split('_').last();

    m_selectedMapKey = mapKey;
    updateMapDetails(m_selectedMapKey);
}

void Choose::updateMapDetails(const QString& mapKey)
{
    // 从预加载好的 m_allMaps 中可靠地获取地图信息
    if (!m_allMaps.contains(mapKey)) {
        qWarning() << "Attempted to update details for unknown map key:" << mapKey;
        return;
    }

    const MapInfo& info = m_allMaps.value(mapKey);
    ui->mapDescriptionLabel->setText(info.description);

    QPixmap pixmap = ResourceManager::instance().getPixmap(info.backgroundPath);
    if (!pixmap.isNull()) {
        // 使用 .scaled() 来保持QLabel尺寸固定，避免破坏布局
        ui->mapPreviewLabel->setPixmap(pixmap.scaled(ui->mapPreviewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        ui->mapPreviewLabel->clear();
        ui->mapPreviewLabel->setText("无法加载预览图");
        qWarning() << "ResourceManager failed to provide pixmap for" << info.backgroundPath;
    }
}

void Choose::onConfirmClicked()
{
    if (m_selectedMapKey.isEmpty()) {
        QMessageBox::warning(this, "选择地图", "请先选择一张地图！");
        return;
    }

    // 最终检查，确保所选地图数据存在
    if (!m_allMaps.contains(m_selectedMapKey)) {
        QMessageBox::critical(this, "严重错误", "所选地图数据丢失，将使用默认地图。");
        m_selectedMapKey = "default"; // 回退到默认地图
    }

    // --- 最终数据传递 ---
    // 从 m_allMaps 中获取完整、正确的 MapInfo 对象并传递
    GameData::instance().selectedMap = m_allMaps.value(m_selectedMapKey);
    GameData::instance().selectedCharacter = m_selectedCharacter;

    // 现在信号带着正确的数据被发出
    emit selectionConfirmed();
}
