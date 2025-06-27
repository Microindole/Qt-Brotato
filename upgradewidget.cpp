#include "upgradewidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>

UpgradeWidget::UpgradeWidget(QWidget *parent) :
    QWidget(parent)
{
    // --- UI 设置 ---
    // 设置半透明的黑色背景
    this->setStyleSheet("background-color: rgba(0, 0, 0, 180); color: white;");
    
    // 创建主垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    mainLayout->setSpacing(30);
    mainLayout->setAlignment(Qt::AlignCenter);

    // 标题
    QLabel *titleLabel = new QLabel("等级提升！", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 48px; font-weight: bold; color: #FFD700;");
    mainLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel("选择一项强化", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("font-size: 24px; color: #E0E0E0;");
    mainLayout->addWidget(subtitleLabel);

    // 用于放置升级选项按钮的水平布局
    QHBoxLayout *optionsLayout = new QHBoxLayout();
    optionsLayout->setSpacing(20);
    // 将选项布局添加到主布局中
    // 我们将把按钮添加到这个布局里
    mainLayout->addLayout(optionsLayout);
    
    // 给布局命名，方便在 setUpgradeOptions 中查找
    optionsLayout->setObjectName("optionsLayout");

    this->setLayout(mainLayout);
}

UpgradeWidget::~UpgradeWidget()
{
    // 在C++中，如果父对象被删除，其子对象（包括布局和控件）会自动被删除
    // 所以这里通常不需要手动删除ui指针管理的控件
}

void UpgradeWidget::setUpgradeOptions(const QList<UpgradeType>& options)
{
    // 通过对象名找到之前创建的布局
    QHBoxLayout *layout = this->findChild<QHBoxLayout*>("optionsLayout");
    if (!layout) {
        qWarning() << "Could not find optionsLayout!";
        return;
    }

    // 清空旧的选项
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    // 为每个选项创建一个按钮
    for (UpgradeType type : options) {
        QPushButton *button = new QPushButton(upgradeTypeToString(type), this);
        
        // 设置按钮的样式
        button->setMinimumSize(180, 220); // 设置最小尺寸
        button->setStyleSheet(
            "QPushButton {"
            "  font-size: 18px;"
            "  font-weight: bold;"
            "  background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #555555, stop:1 #333333);"
            "  border: 2px solid #FFD700;"
            "  border-radius: 15px;"
            "  padding: 10px;"
            "}"
            "QPushButton:hover {"
            "  background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #666666, stop:1 #444444);"
            "  border: 2px solid #FFFFFF;"
            "}"
        );
        
        // 将升级类型作为属性存储在按钮上，方便点击时识别
        button->setProperty("upgradeType", QVariant::fromValue(type));
        
        // 连接点击信号到槽函数
        connect(button, &QPushButton::clicked, this, &UpgradeWidget::onOptionClicked);
        
        layout->addWidget(button);
    }
}

void UpgradeWidget::onOptionClicked()
{
    // 获取信号的发送者（即被点击的按钮）
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        // 从按钮的属性中取回我们之前存储的升级类型
        UpgradeType selected = button->property("upgradeType").value<UpgradeType>();
        
        // 发出信号，通知GameWidget玩家的选择
        emit upgradeSelected(selected);
    }
}

QString UpgradeWidget::upgradeTypeToString(UpgradeType type)
{
    switch (type) {
        case MaxHealth:   return "最大生命值 +15";
        case AttackPower: return "攻击力 +4";
        case Speed:       return "移动速度 +0.25";
        case HealthRegen: return "生命恢复 +0.8/s";
        case Armor:       return "护甲 +5(减伤)";
        default:          return "未知升级";
    }
}
