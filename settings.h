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

    void setInitialVolumes(float musicVolume, float sfxVolume);

signals:
    void backgroundMusicVolumeChanged(float volume);
    void soundEffectsVolumeChanged(float volume);
    void backClicked();

private slots:
    void on_musicSlider_valueChanged(int value);
    void on_sfxSlider_valueChanged(int value);
    void on_backButton_clicked();

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
