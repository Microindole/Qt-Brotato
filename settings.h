#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    // 设置初始状态
    void setInitialVolumes(float musicVolume, float sfxVolume);
    void setInitialHealthBarVisibility(bool visible); // 新增函数

signals:
    // 信号
    void backgroundMusicVolumeChanged(float volume);
    void soundEffectsVolumeChanged(float volume);
    void healthBarVisibilityChanged(bool visible); // 新增信号
    void backClicked();

private slots:
    // 槽函数
    void on_musicSlider_valueChanged(int value);
    void on_sfxSlider_valueChanged(int value);
    void on_healthBarCheckBox_toggled(bool checked); // 新增槽
    void on_backButton_clicked();

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
