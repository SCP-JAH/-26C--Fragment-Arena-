#ifndef BULLET_H
#define BULLET_H

#include <QPointF>
#include <QPainter>

class Bullet
{
public:
    Bullet(QPointF pos, QPointF dir);

    void update(double dt);
    void render(QPainter& painter);

    bool isAlive() const;
    QPointF getPos() const;
    double getRadius() const;
    void destroy();

private:
    QPointF m_pos;
    QPointF m_dir;

    double m_speed;
    double m_radius;

    bool m_alive;
};

#endif // BULLET_H