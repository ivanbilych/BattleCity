#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include "boardobject.h"
#include "board.h"
#include "tank.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "mainAI.h"
#include "wall.h"
#include "playersbase.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<Player>("BattleCity", 1, 0, "Player");
    qmlRegisterType<Board>("BattleCity", 1, 0, "Board");
    qmlRegisterType<Enemy>("BattleCity", 1, 0, "Enemy");
    qmlRegisterType<Bullet>("BattleCity", 1, 0, "Bullet");
    qmlRegisterType<Wall>("BattleCity", 1, 0, "Wall");
    qmlRegisterType<PlayersBase>("BattleCity", 1, 0, "PlayersBase");
    qmlRegisterType<Tank>();
    qmlRegisterType<BoardObject>();

    QQmlApplicationEngine engine;

    QScopedPointer<MainAI> comp(new MainAI(&app));

    engine.rootContext()->setContextProperty("computer", comp.data());
    engine.rootContext()->setContextProperty("tankAI", comp->getTankAI());
    engine.rootContext()->setContextProperty("gameBoard", comp->getBoard());
    engine.rootContext()->setContextProperty("playerTank", comp->getBoard()->getPlayer());
    engine.rootContext()->setContextProperty("playersBase", comp->getBoard()->getBase());

    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}

