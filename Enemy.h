#ifndef ENEMY_H
#define ENEMY_H

#include <QRectF>
#include <QPainter>
#include <QPointF>

class Enemy
{
public:
    Enemy(double x, double y, double size, int hp);

    void moveBy(double dx, double dy);
    void render(QPainter& painter);
    void takeDamage(int damage);

    bool isAlive() const;
    QRectF getRect() const;
    QPointF getCenter() const;

private:
    QRectF m_rect;
    int m_hp;
    bool m_alive;
    double m_speed;
};

#endif // ENEMY_H