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
}

Settings::~Settings()
{
    delete ui;
}

void Settings::setInitialVolumes(float musicVolume, float sfxVolume)
{
    ui->musicSlider->blockSignals(true);
    ui->sfxSlider->blockSignals(true);

    // 音乐音量: 0.0 to 1.0 -> 0 to 100
    ui->musicSlider->setValue(static_cast<int>(musicVolume * 100));
    // 音效音量: 0.0 to 0.02 -> 0 to 100
    ui->sfxSlider->setValue(static_cast<int>((sfxVolume / 0.02f) * 100.0f));
    ui->musicValueLabel->setText(QString("%1%").arg(ui->musicSlider->value()));
    ui->sfxValueLabel->setText(QString("%1%").arg(ui->sfxSlider->value()));

    ui->musicSlider->blockSignals(false);
    ui->sfxSlider->blockSignals(false);
}

void Settings::on_musicSlider_valueChanged(int value)
{
    ui->musicValueLabel->setText(QString("%1%").arg(value));
    // 音乐音量: 0 to 100 -> 0.0 to 1.0
    emit backgroundMusicVolumeChanged(static_cast<float>(value) / 100.0f);
}

void Settings::on_sfxSlider_valueChanged(int value)
{
    ui->musicValueLabel->setText(QString("%1%").arg(value));
    // 音效音量: 0 to 100 -> 0.0 to 0.02
    emit soundEffectsVolumeChanged((static_cast<float>(value) / 100.0f) * 0.02f);
}

void Settings::on_backButton_clicked()
{
    emit backClicked();
}
