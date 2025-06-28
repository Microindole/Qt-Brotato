#ifndef PAUSE_H
#define PAUSE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Pause; }
QT_END_NAMESPACE

class Player;

class Pause : public QWidget
{
    Q_OBJECT

public:
    explicit Pause(QWidget *parent = nullptr);
    ~Pause();

    void updateStats(const Player* player);
    void showForPause();
    void showForWaveComplete(int waveNumber);

private slots:
    void onContinueClicked();
    void onRestartClicked();
    void onEndRunClicked();
    void onSettingsClicked();
    void onBackToMenuClicked();

signals:
    void continueGame();
    void restartGame();
    void endRun();
    void openSettings();
    void backToMenu();

private:
    Ui::Pause *ui;
};

#endif // PAUSE_H
