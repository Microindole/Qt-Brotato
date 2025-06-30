#ifndef CHOOSE_H
#define CHOOSE_H

#include <QWidget>
#include <QMap>
#include "player.h"
#include "gamedata.h"


namespace Ui {
class Choose;
}

class Choose : public QWidget
{
    Q_OBJECT

public:
    explicit Choose(QWidget *parent = nullptr);
    ~Choose();

signals:
    void backToMenuRequested();
    void selectionConfirmed();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onConfirmClicked();
    void onCharacterButtonClicked(Player::CharacterType type);
    void onMapButtonClicked(const QString& mapKey);

private:
    void loadAndSetupMaps();
    void setupMapSelectionUI();
    void updateMapDetails(const QString& mapKey);

    Ui::Choose *ui;
    QMap<QString, MapInfo> m_availableMaps;
    QString m_selectedMapKey;
    Player::CharacterType m_selectedCharacter;
};

#endif // CHOOSE_H
