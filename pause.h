#ifndef PAUSE_H
#define PAUSE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Pause; }
QT_END_NAMESPACE

class Player; // 向前声明Player类

class Pause : public QWidget
{
    Q_OBJECT

public:
    explicit Pause(QWidget *parent = nullptr);
    ~Pause();

    void updateStats(const Player* player); // 新增：更新属性显示

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
