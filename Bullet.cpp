#include "Bullet.h"

Bullet::Bullet(QPointF pos, QPointF dir)
    : m_pos(pos),
    m_dir(dir),
    m_speed(400.0),
    m_radius(5),
    m_alive(true)
{
}

void Bullet::update(double dt)
{
    m_pos += m_dir * m_speed * dt;

    // 超出边界就销毁
    if (m_pos.x() < 0 || m_pos.x() > 960 ||
        m_pos.y() < 0 || m_pos.y() > 640)
    {
        m_alive = false;
    }
}

void Bullet::render(QPainter &painter)
{
    painter.setBrush(Qt::yellow);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(m_pos, m_radius, m_radius);
}

bool Bullet::isAlive() const
{
    return m_alive;
}

QPointF Bullet::getPos() const
{
    return m_pos;
}

double Bullet::getRadius() const
{
    return m_radius;
}

void Bullet::destroy()
{
    m_alive = false;
}