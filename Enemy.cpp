#include "Enemy.h"

Enemy::Enemy(double x, double y, double size, int hp)
    : m_rect(x, y, size, size),
    m_hp(hp),
    m_alive(true),
    m_speed(100.0)
{
}

void Enemy::moveBy(double dx, double dy)
{
    if (!m_alive) return;

    m_rect.translate(dx, dy);
}

void Enemy::render(QPainter &painter)
{
    if (!m_alive) return;

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::red);
    painter.drawRect(m_rect);

    painter.setBrush(QColor(60, 60, 60));
    painter.drawRect(QRectF(m_rect.x(), m_rect.y() - 10, m_rect.width(), 6));

    double hpRatio = m_hp / 100.0;
    if (hpRatio < 0) hpRatio = 0;

    painter.setBrush(Qt::green);
    painter.drawRect(QRectF(m_rect.x(), m_rect.y() - 10, m_rect.width() * hpRatio, 6));
}

void Enemy::takeDamage(int damage)
{
    if (!m_alive) return;

    m_hp -= damage;
    if (m_hp <= 0)
    {
        m_hp = 0;
        m_alive = false;
    }
}

bool Enemy::isAlive() const
{
    return m_alive;
}

QRectF Enemy::getRect() const
{
    return m_rect;
}

QPointF Enemy::getCenter() const
{
    return QPointF(
        m_rect.x() + m_rect.width() / 2.0,
        m_rect.y() + m_rect.height() / 2.0
        );
}