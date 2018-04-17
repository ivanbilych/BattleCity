#include "board.h"
#include <QGuiApplication>
#include <QWindow>

#include "objectscleaner.h"

const int step = 10;
const int tankWidth = 50;

Board::Board(QObject *parent) : QObject(parent)
{
    qDebug() << "Constructor Board";

    ObjectsCleaner::getInstance(parent, this);

    int height = 750;
    int width = 750;

    // divide the board into cells
    int rowsAmount = height / step;
    if (height % step != 0)
        rowsAmount += 1;

    int columnsAmount = width / step;
    if (width % step != 0)
        columnsAmount += 1;

    cells.resize(rowsAmount);
    for (int row = 0; row < rowsAmount; row++) {
        for(int col = 0; col < columnsAmount; col++) {
            cells[row].append(new Cell(row * step, col * step));
        }
    }

    //create enemy tanks
    int enemiesAmount = 8; // TO DO: move to commmon config

    for (int i = 0; i < enemiesAmount; i++) {
        Enemy* enemy = new Enemy(100, 100, tankWidth * i * 2, 0);
        m_enemies.append(enemy);

        int firstRow = 0;
        int firstCol = tankWidth * i * 2 / step;
        rowsAmount = enemy->getHeight() / step;
        columnsAmount = enemy->getWidth() / step;

        for (int row = 0; row < rowsAmount; row++) {
            for(int col = 0; col < columnsAmount; col++) {
                 cells[firstRow + row][firstCol + col]->setBoardObject(enemy);
            }
        }

        if (i == enemiesAmount - 1)
            continue;

        for(int j = 0; j < 4 + abs(i - 3) * 2 ; j++) {
            Wall* upperWall = new Wall(0, tankWidth * (2 * i + 1), j * 25 + tankWidth);
            m_walls.append(upperWall);

            int firstRow = upperWall->getY() / step;
            int firstCol = upperWall->getX() / step;
            rowsAmount = upperWall->getHeight() / step;
            columnsAmount = upperWall->getWidth() / step;

            for (int row = 0; row < rowsAmount; row++) {
                for(int col = 0; col < columnsAmount; col++) {
                     cells[firstRow + row][firstCol + col]->setBoardObject(upperWall);
                }
            }

            Wall* lowerWall = new Wall(0, tankWidth * (2 * i + 1), height / 2 + (j + 1) * 25 + tankWidth);
            m_walls.append(lowerWall);

            firstRow = lowerWall->getY() / step;
            firstCol = lowerWall->getX() / step;

            for (int row = 0; row < rowsAmount; row++) {
                for(int col = 0; col < columnsAmount; col++) {
                     cells[firstRow + row][firstCol + col]->setBoardObject(lowerWall);
                }
            }
        }
    }

    //create walls
    for(int j = 0; j < 17 ; j++) {
        Wall* wall = new Wall(0, j * 25 + tankWidth * 3, width / 2);
        m_walls.append(wall);

        int firstRow = wall->getY() / step;
        int firstCol = wall->getX() / step;
        rowsAmount = wall->getHeight() / step;
        columnsAmount = wall->getWidth() / step;

        for (int row = 0; row < rowsAmount; row++) {
            for(int col = 0; col < columnsAmount; col++) {
                 cells[firstRow + row][firstCol + col]->setBoardObject(wall);
            }
        }
    }

    //create player's tank
    int playerX = width / 2 - 125;
    int playerY = height - 50;
    player.reset(new Player(100, 100, "PlayerTank", playerX, playerY));

    int firstRow = playerY / step;
    int firstCol = playerX / step;
    rowsAmount = player->getHeight() / step;
    columnsAmount = player->getWidth() / step;

    for (int row = 0; row < rowsAmount; row++) {
        for(int col = 0; col < columnsAmount; col++) {
            cells[firstRow + row][firstCol + col]->setBoardObject(player.data());
        }
    }

    //create the player's base
    base.reset(new PlayersBase(player->getX() + 100, height - 50));

    firstRow = base->getY() / step;
    firstCol = base->getX() / step;
    rowsAmount = base->getHeight() / step;
    columnsAmount = base->getWidth() / step;

    for (int row = 0; row < rowsAmount; row++) {
        for(int col = 0; col < columnsAmount; col++) {
            cells[firstRow + row][firstCol + col]->setBoardObject(base.data());
        }
    }

    //create walls around the player's tank
    for(int j = 0; j < 3 ; j++) {
        Wall* leftWalls = new Wall(0, player->getX() + 50, height - (j + 1) * 25);
        Wall* rightWalls = new Wall(0, player->getX() + 150, height - (j + 1) * 25);

        m_walls.append(leftWalls);
        m_walls.append(rightWalls);

        firstRow = leftWalls->getY() / step;
        firstCol = leftWalls->getX() / step;
        rowsAmount = leftWalls->getHeight() / step;
        columnsAmount = leftWalls->getWidth() / step;

        for (int row = 0; row < rowsAmount; row++) {
            for(int col = 0; col < columnsAmount; col++) {
                 cells[firstRow + row][firstCol + col]->setBoardObject(leftWalls);
                 cells[firstRow + row][firstCol + 10 + col]->setBoardObject(rightWalls);
            }
        }
    }

    Wall* wall = new Wall(0, player->getX() + 100, height - 75);
    m_walls.append(wall);
    firstRow = wall->getY() / step;
    firstCol = wall->getX() / step;
    rowsAmount = wall->getHeight() / step;
    columnsAmount = wall->getWidth() / step;

    for (int row = 0; row < rowsAmount; row++) {
        for(int col = 0; col < columnsAmount; col++) {
             cells[firstRow + row][firstCol + col]->setBoardObject(wall);
        }
    }

}

Board::~Board()
{
    qDeleteAll(m_enemies);
    m_enemies.clear();

    foreach(QVector<Cell*> cell, cells)
        qDeleteAll(cell);
    cells.clear();

    qDeleteAll(m_bullets);
    m_bullets.clear();

    qDeleteAll(m_walls);
    m_walls.clear();
}

bool Board::AreCellsFree(QVector<Cell*> cells)
{
    // check if cells are avaliable
    bool areFree = true;

    for (auto& cell : cells) {
        if (!cell->isCellEmpty()) {
            areFree = false;
            break;
        }
    }

    return areFree;
}

void Board::FreeCells(QVector<Cell*> cells)
{
    for (auto& cell : cells)
        cell->clearBoardObject();
}

QVector<QVector<Cell*>> Board::calcPrevAndNextCells(BoardObject* object)
{
    QVector<Cell*> prevCells, nextCells;
    QVector<QVector<Cell*>> prevNextCells;

    int direction = object->getDirection();
    int firstRow = object->getY() / step;
    int firstCol = object->getX() / step;
    int nextIndex = 0;
    int prevIndex = 0;

    //calculate indexes next and previous cells
    if ( direction == Direction::dir_down) {
        prevIndex = firstRow;
        nextIndex = firstRow + object->getHeight() / step;

    } else if (direction == Direction::dir_up) {
        prevIndex = firstRow + object->getHeight() / step - 1;
        nextIndex = firstRow - 1;
    } else if (direction == Direction::dir_left) {
        prevIndex = firstCol + object->getWidth() / step - 1;
        nextIndex = firstCol - 1;
    } else if (direction == Direction::dir_right) {
        nextIndex = firstCol + object->getWidth() / step;
        prevIndex = firstCol;
    }

    if ( direction == Direction::dir_down || direction == Direction::dir_up) {
        int cellAmount = object->getWidth() / step;
        for (int cell = 0; cell < cellAmount; cell++)
            prevCells.append(cells[prevIndex][firstCol + cell]);

        prevNextCells.append(prevCells);

        //check colision with window borders
        if (nextIndex * step >= qApp->focusWindow()->height() - 101 ||
            nextIndex * step < 0)
            return prevNextCells;

        for (int cell = 0; cell < cellAmount; cell++)
            nextCells.append(cells[nextIndex][firstCol + cell]);
    } else {
        int cellAmount = object->getHeight() / step;
        for (int cell = 0; cell < cellAmount; cell++)
            prevCells.append(cells[firstRow + cell][prevIndex]);

        prevNextCells.append(prevCells);

        //check colision with window borders
        if (nextIndex * step >= qApp->focusWindow()->width() - 111 ||
            nextIndex * step < 0)
            return prevNextCells;

        for (int cell = 0; cell < cellAmount; cell++)
            nextCells.append(cells[firstRow + cell][nextIndex]);
    }

    prevNextCells.append(nextCells);

    return prevNextCells;
}

QPair<Objects, BoardObject*> Board::IdentifyObjectType(QVector<Cell*> objectCells)
{
    QPair<Objects, BoardObject*> pair(Objects::undefined, nullptr);

    for (auto& cell : objectCells) {
        if (!cell->isCellEmpty())
            pair.first = cell->getBoardObject()->getTypeObject();
            pair.second = cell->getBoardObject();
    }

    return pair;
}

QPair<Objects, BoardObject*> Board::move(Tank* tank)
{
    // check if a cell is avaliable
    // if it's not than don't move the tank
    QVector<QVector<Cell*>> prevNextCells = calcPrevAndNextCells(tank);
    if (prevNextCells.size() == 1)
        return qMakePair(Objects::windowBorders, nullptr);

    // if new cells are avaliable the tank make a step
    // otherwise the tank change direction
    if (AreCellsFree(prevNextCells[1])) {
        for (auto& cell : prevNextCells[1])
            cell->setBoardObject(tank);
        FreeCells(prevNextCells[0]);

        return qMakePair(Objects::emptyCell, nullptr);
    }

    return IdentifyObjectType(prevNextCells[1]);
}

QPair<Objects, BoardObject*> Board::move(Bullet* bullet)
{
    QVector<QVector<Cell*>> prevNextCells = calcPrevAndNextCells(bullet);
    // if we couldn't calculate next cells
    // than we have collision with window borders
    if (prevNextCells.size() == 1) {
        FreeCells(prevNextCells[0]);
        return qMakePair(Objects::windowBorders, nullptr);
    }

    if (AreCellsFree(prevNextCells[1])) {
        for (auto& cell : prevNextCells[1])
            cell->setBoardObject(bullet);
        FreeCells(prevNextCells[0]);

        return qMakePair(Objects::emptyCell, nullptr);
    }

    return IdentifyObjectType(prevNextCells[1]);
}

void Board::addBullet(Bullet* bullet)
{
    m_bullets.append(bullet);

    int col = bullet->getX() / step;
    int row = bullet->getY() / step;

    Cell* cell = cells.at(row).at(col);
    cell->setBoardObject(bullet);

    emit bulletsChanged(bullets());
}

void Board::removeObject(BoardObject *object)
{
    Objects objectType = object->getTypeObject();

    if (objectType == Objects::enemy) {
        m_enemies.removeOne(dynamic_cast<Enemy*>(object));
        emit enemiesChanged(enemies());
    } else if (objectType == Objects::bullet) {
        m_bullets.removeOne(dynamic_cast<Bullet*>(object));
        emit bulletsChanged(bullets());
    } else if (objectType == Objects::player) {
        emit playerIsAlive(false);
    } else if (objectType == Objects::wall) {
        m_walls.removeOne(dynamic_cast<Wall*>(object));
        emit wallsChanged(walls());
    } else if (objectType == Objects::playersBase) {
        emit playerIsAlive(false);
    }

    int firstRow = object->getY() / step;
    int firstCol = object->getX() / step;
    int rowsAmount = object->getHeight() / step;
    int columnsAmount = object->getWidth() / step;

    for (int row = 0; row < rowsAmount; row++) {
        for(int col = 0; col < columnsAmount; col++) {
             cells[firstRow + row][firstCol + col]->clearBoardObject();
        }
    }
}

QQmlListProperty<Enemy> Board::enemies()
{
    return QQmlListProperty<Enemy>(this, m_enemies);
}

QList<Enemy*> Board::getEnemies()
{
    return m_enemies;
}

Player* Board::getPlayer() const
{
    return player.data();
}

QQmlListProperty<Bullet> Board::bullets()
{
    return QQmlListProperty<Bullet>(this, m_bullets);
}

QList<Bullet*> Board::getBullets()
{
    return m_bullets;
}

QQmlListProperty<Wall> Board::walls()
{
    return QQmlListProperty<Wall>(this, m_walls);
}

QList<Wall*> Board::getWalls()
{
    return m_walls;
}

PlayersBase* Board::getBase() const
{
    return base.data();
}
