#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);
    connect(ui->musicSlider, &QSlider::valueChanged, this, &Settings::on_musicSlider_valueChanged);
    connect(ui->sfxSlider, &QSlider::valueChanged, this, &Settings::on_sfxSlider_valueChanged);
    connect(ui->backButton, &QPushButton::clicked, this, &Settings::on_backButton_clicked);
    // 连接血条复选框的 toggled 信号到新的槽函数
    connect(ui->healthBarCheckBox, &QCheckBox::toggled, this, &Settings::on_healthBarCheckBox_toggled);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::setInitialVolumes(float musicVolume, float sfxVolume)
{
    ui->musicSlider->blockSignals(true);
    ui->sfxSlider->blockSignals(true);

    // 将 0.0-1.0 的音量值转换为 0-100 的滑块值
    ui->musicSlider->setValue(static_cast<int>(musicVolume * 100));
    ui->sfxSlider->setValue(static_cast<int>(sfxVolume * 100)); // <-- 修正了音效音量的计算

    // 更新对应的文本标签
    ui->musicValueLabel->setText(QString("%1%").arg(ui->musicSlider->value()));
    ui->sfxValueLabel->setText(QString("%1%").arg(ui->sfxSlider->value()));

    ui->musicSlider->blockSignals(false);
    ui->sfxSlider->blockSignals(false);
}

void Settings::setInitialHealthBarVisibility(bool visible)
{
    // 设置复选框的初始状态，同样阻止信号触发
    ui->healthBarCheckBox->blockSignals(true);
    ui->healthBarCheckBox->setChecked(visible);
    ui->healthBarCheckBox->blockSignals(false);
}

void Settings::on_musicSlider_valueChanged(int value)
{
    ui->musicValueLabel->setText(QString("%1%").arg(value));
    // 将 0-100 的滑块值转换为 0.0-1.0 的实际音量值
    emit backgroundMusicVolumeChanged(static_cast<float>(value) / 100.0f);
}

void Settings::on_sfxSlider_valueChanged(int value)
{
    // 更新音效音量标签
    ui->sfxValueLabel->setText(QString("%1%").arg(value));
    
    // 修正了音效音量的计算，使其与音乐音量逻辑一致
    emit soundEffectsVolumeChanged(static_cast<float>(value) / 100.0f);
}

void Settings::on_healthBarCheckBox_toggled(bool checked)
{
    // 当复选框状态改变时，发射信号通知其他部分
    emit healthBarVisibilityChanged(checked);
}

void Settings::on_backButton_clicked()
{
    emit backClicked();
}
