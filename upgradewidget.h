#ifndef UPGRADEWIDGET_H
#define UPGRADEWIDGET_H

#include <QWidget>
#include <QList>

// 前向声明，避免在头文件中包含完整的QPushButton头文件
QT_BEGIN_NAMESPACE
namespace Ui { class UpgradeWidget; }
QT_END_NAMESPACE

class QPushButton;

class UpgradeWidget : public QWidget
{
    Q_OBJECT

public:
    // 定义一个枚举来清晰地表示不同的升级选项
    enum UpgradeType {
        MaxHealth,
        AttackPower,
        Speed,
        HealthRegen,
        Armor
    };
    Q_ENUM(UpgradeType) // 注册枚举，使其在Qt元对象系统中可用

    explicit UpgradeWidget(QWidget *parent = nullptr);
    ~UpgradeWidget();

    // 一个公共方法，用于在显示窗口前设置要提供的升级选项
    void setUpgradeOptions(const QList<UpgradeType>& options);

signals:
    // 当玩家选择一个升级时发出此信号
    void upgradeSelected(UpgradeType type);

private slots:
    // 槽函数，用于处理按钮点击事件
    void onOptionClicked();

private:
    Ui::UpgradeWidget *ui;

    // 一个辅助函数，用于将枚举转换为用户可读的字符串
    QString upgradeTypeToString(UpgradeType type);
};

#endif // UPGRADEWIDGET_H
