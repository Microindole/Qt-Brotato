#include "menu.h"
#include "ui_menu.h"
#include <QGridLayout>

Menu::Menu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);

    // 连接信号
    connect(ui->startGameButton, &QPushButton::clicked, this, &Menu::startGameRequested);
    connect(ui->exitButton, &QPushButton::clicked, this, &Menu::exitGameRequested);

    // ===================================================================
    // ==          用我们自定义的动画控件替换掉UI里的占位符           ==
    // ===================================================================
    // 1. 创建我们的动画控件实例
    AnimatedCharacterWidget *animatedCharacter = new AnimatedCharacterWidget(this);

    // 2. 从布局中获取并移除旧的、静态的 QLabel
    //    这里的 "mainLayout" 是你在 .ui 文件中给顶层 QGridLayout 设置的 objectName
    QGridLayout *layout = qobject_cast<QGridLayout*>(this->layout());
    if (layout) {
        // 替换操作
        layout->replaceWidget(ui->characterImageLabel, animatedCharacter);
        // 删除旧的控件
        delete ui->characterImageLabel;
        // 将 ui 指针置空，防止悬挂指针问题
        ui->characterImageLabel = nullptr;
    }
    // ===================================================================
}

Menu::~Menu()
{
    delete ui;
}

// 不再需要 showEvent 和 setupCharacterAnimation，可以完全删除它们