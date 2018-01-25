#ifndef MAINAI_H
#define MAINAI_H

#include <QObject>
#include <QTimer>
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

    void setAnimation(bool value);

    void startTimer();
    void stopTimer();

signals:
    void stopStartAnimation(bool value);

public slots:
    void motionEvents();
    void atackEvents();

private:
    QScopedPointer<QTimer> motionTimer;
    QScopedPointer<QTimer> atackTimer;

    QScopedPointer<Board> board;
    QScopedPointer<TankAI> tankAI;
    QScopedPointer<BulletAI> bulletAI;
    bool animation;
};

#endif // MAINAI_H