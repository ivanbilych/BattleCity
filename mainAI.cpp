#include "mainAI.h"

MainAI::MainAI(QObject *parent) : QObject(parent)
{
    qDebug() << "Constructor main artificial intelligence";

    board.reset(new Board(parent));
    animation = false;

    tankTimer.reset(new QTimer(this));
    bulletTimer.reset(new QTimer(this));

    tankAI.reset(new TankAI);
    bulletAI.reset(new BulletAI);

    connect(tankTimer.data(), SIGNAL(timeout()), this, SLOT(tankEvents()));
    connect(bulletTimer.data(), SIGNAL(timeout()), this, SLOT(bulletEvents()));

    connect(board.data(), SIGNAL(playerIsAlive(bool)), this, SLOT(setAnimation(bool)));
}

MainAI::~MainAI()
{

}

void MainAI::tankEvents()
{
    if (!(rand() % 10)) {
        tankAI->sendAtackSignal(board.data());
    } else {
        tankAI->sendPressSignal(board.data());
    }
}

void MainAI::bulletEvents()
{
    bulletAI->sendMoveSignal(board.data());
}

void atackEvents();

Board* MainAI::getBoard() const
{
    return board.data();
}

bool MainAI::getAnimation() const
{
    return animation;
}

TankAI* MainAI::getTankAI() const
{
    return tankAI.data();
}

void MainAI::setAnimation(bool value)
{
    if (animation != value) {
        animation = value;

        if (animation)
            startTimer();
        else
            stopTimer();

        emit stopStartAnimation(animation);
    }
}

void MainAI::startTimer()
{
    if (!tankTimer->isActive() && !bulletTimer->isActive()) {
        tankTimer->start(100);
        bulletTimer->start(50);
    }
}

void MainAI::stopTimer()
{
    if (tankTimer->isActive() && bulletTimer->isActive()) {
        tankTimer->stop();
        bulletTimer->stop();
    }
}
