#include "menu.h"
#include "ui_menu.h"
#include <QGridLayout>

Menu::Menu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);

    // 背景集成代码 
    // 创建动态背景控件的实例
    m_backgroundWidget = new DynamicBackgroundWidget(this);

    // 将其添加到布局的最底层
    auto *layout = qobject_cast<QGridLayout*>(this->layout());
    if (layout) {
        // 添加到 (0, 0) 位置，并让它跨越所有行和列 (-1, -1)
        layout->addWidget(m_backgroundWidget, 0, 0, -1, -1);
    }

    // 确保它被绘制在所有其他UI元素的后面
    m_backgroundWidget->lower();

    // 连接信号
    connect(ui->startGameButton, &QPushButton::clicked, this, &Menu::startGameRequested);
    connect(ui->exitButton, &QPushButton::clicked, this, &Menu::exitGameRequested);

    // 创建我们的动画控件实例
    AnimatedCharacterWidget *animatedCharacter = new AnimatedCharacterWidget(this);

    // 从布局中获取并移除旧的、静态的 QLabel
    // QGridLayout *layout = qobject_cast<QGridLayout*>(this->layout());
    if (layout) {
        // 替换操作
        layout->replaceWidget(ui->characterImageLabel, animatedCharacter);
        // 删除旧的控件
        delete ui->characterImageLabel;
        // 将 ui 指针置空，防止悬挂指针问题
        ui->characterImageLabel = nullptr;
    }
}

Menu::~Menu()
{
    delete ui;
}
