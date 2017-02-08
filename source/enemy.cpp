#include "enemy.h"

Enemy::Enemy(float hitPointsLimit, float damage, QQuickItem* parent) :
    Tank(parent) {
    qDebug() << "Constructor: Enemy";

    this->hitPointsLimit = hitPointsLimit;
    this->damage = damage;
    hitPoints = hitPointsLimit;
    direction = Direction::dir_down;
    coordinate_x = 300;
    coordinate_y = 20;
}

Enemy::Enemy(QQuickItem* parent) :
    Tank(parent) {
}

Enemy::~Enemy() {
    qDebug() << "Destructor: Enemy";
}

void Enemy::move() {
    try {
        doStep();
    } catch ( EndOfBoardException e) {
        updateDirection();
    }

}

void Enemy::doStep() {
    if ( direction == Direction::dir_down ) {
        if ( coordinate_y >= 600 ) {
            throw EndOfBoardException();
        }

        coordinate_y += 10;
        emit YChanged(coordinate_y);
    } else if ( direction == Direction::dir_up ) {
        if ( coordinate_y <= 0 ) {
            throw EndOfBoardException();
        }

        coordinate_y -= 10;
        emit YChanged(coordinate_y);
    } else if ( direction == Direction::dir_left ) {
        if ( coordinate_x <= 0 ) {
            throw EndOfBoardException();
        }

        coordinate_x -= 10;
        emit XChanged(coordinate_x);
    } else if ( direction == Direction::dir_right ) {
        if ( coordinate_x >= 600 ) {
            throw EndOfBoardException();
        }

        coordinate_x += 10;
        emit XChanged(coordinate_x);
    }
}
