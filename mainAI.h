#ifndef MAINAI_H
#define MAINAI_H

#include <QObject>
#include <QTimer>
#include "objectscleaner.h"
#include "tankAI.h"
#include "bulletAI.h"

class MainAI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool animation READ getAnimation WRITE setAnimation NOTIFY stopStartAnimation)

public:
    explicit MainAI(QObject *parent = 0);
    ~MainAI();

    Board* getBoard() const; // use this method only for setContextProperty
    bool getAnimation() const;
    TankAI* getTankAI() const;

    void startTimer();
    void stopTimer();

signals:
    void stopStartAnimation(bool value);

public slots:
    void tankEvents();
    void bulletEvents();
    void setAnimation(bool value);

private:
    QScopedPointer<QTimer> tankTimer;
    QScopedPointer<QTimer> bulletTimer;

    QScopedPointer<Board> board;
    QScopedPointer<TankAI> tankAI;
    QScopedPointer<BulletAI> bulletAI;
    bool animation;
};

#endif // MAINAI_H
