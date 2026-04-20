#include "GameWindow.h"
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <cmath>

GameWindow::GameWindow(QWidget *parent)
    : QWidget(parent),
    m_timer(new QTimer(this)),
    m_lastTime(0),
    m_playerX(100),
    m_playerY(100),
    m_playerSize(40),
    m_playerSpeed(220.0),
    m_playerHp(100),
    m_playerMaxHp(100),
    m_playerDamageTimer(0.0),
    m_moveUp(false),
    m_moveDown(false),
    m_moveLeft(false),
    m_moveRight(false),
    m_gameOver(false),
    m_killCount(0),
    m_mousePos(0, 0)
{
    setFixedSize(960, 640);
    setWindowTitle("Lite Arena");
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    m_elapsedTimer.start();
    m_lastTime = m_elapsedTimer.elapsed();

    connect(m_timer, &QTimer::timeout, this, &GameWindow::gameLoop);
    m_timer->start(16);

    resetGame();
}

GameWindow::~GameWindow()
{
}

void GameWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(), QColor(30, 30, 30));

    painter.setPen(Qt::white);
    painter.drawText(20, 30, "WASD Move / Left Click Shoot");

    painter.setPen(Qt::white);
    painter.drawText(20, 55, QString("HP: %1 / %2").arg(m_playerHp).arg(m_playerMaxHp));

    // 玩家血条背景
    painter.setBrush(QColor(60, 60, 60));
    painter.setPen(Qt::NoPen);
    painter.drawRect(QRectF(20, 65, 200, 16));

    // 玩家血条前景
    double hpRatio = static_cast<double>(m_playerHp) / m_playerMaxHp;
    painter.setBrush(Qt::green);
    painter.drawRect(QRectF(20, 65, 200 * hpRatio, 16));

    painter.setPen(Qt::white);
    painter.drawText(20, 100, QString("Kills: %1").arg(m_killCount));

    painter.setBrush(Qt::blue);
    painter.setPen(Qt::NoPen);
    painter.drawRect(QRectF(m_playerX, m_playerY, m_playerSize, m_playerSize));

    for (auto &bullet : m_bullets)
    {
        bullet.render(painter);
    }
    for (auto &enemy : m_enemies)
    {
        enemy.render(painter);
    }

    if (m_gameOver)
    {
        painter.setPen(Qt::red);
        QFont font = painter.font();
        font.setPointSize(24);
        font.setBold(true);
        painter.setFont(font);

        painter.drawText(rect(), Qt::AlignCenter, "GAME OVER\nPress R to Restart");
    }
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    switch (event->key())
    {
    case Qt::Key_W:
        m_moveUp = true;
        break;
    case Qt::Key_S:
        m_moveDown = true;
        break;
    case Qt::Key_A:
        m_moveLeft = true;
        break;
    case Qt::Key_D:
        m_moveRight = true;
        break;
    default:
        break;
    case Qt::Key_R:
        if (m_gameOver)
        {
            resetGame();
        }
        break;
    }
}

void GameWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) return;

    switch (event->key())
    {
    case Qt::Key_W:
        m_moveUp = false;
        break;
    case Qt::Key_S:
        m_moveDown = false;
        break;
    case Qt::Key_A:
        m_moveLeft = false;
        break;
    case Qt::Key_D:
        m_moveRight = false;
        break;
    default:
        break;
    }
}

void GameWindow::mouseMoveEvent(QMouseEvent *event)
{
    m_mousePos = event->pos();
}

void GameWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPointF playerCenter(
            m_playerX + m_playerSize / 2.0,
            m_playerY + m_playerSize / 2.0
            );

        QPointF dir = m_mousePos - playerCenter;

        double length = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());
        if (length == 0) return;

        dir /= length;

        m_bullets.append(Bullet(playerCenter, dir));
    }
}

void GameWindow::gameLoop()
{
    qint64 currentTime = m_elapsedTimer.elapsed();
    double dt = (currentTime - m_lastTime) / 1000.0;
    m_lastTime = currentTime;

    updateGame(dt);
    update();
}

void GameWindow::updateGame(double dt)
{
    if (m_gameOver)
    {
        return;
    }

    double dx = 0.0;
    double dy = 0.0;

    if (m_moveUp) dy -= 1.0;
    if (m_moveDown) dy += 1.0;
    if (m_moveLeft) dx -= 1.0;
    if (m_moveRight) dx += 1.0;

    m_playerX += dx * m_playerSpeed * dt;
    m_playerY += dy * m_playerSpeed * dt;

    if (m_playerX < 0) m_playerX = 0;
    if (m_playerY < 0) m_playerY = 0;
    if (m_playerX + m_playerSize > width()) m_playerX = width() - m_playerSize;
    if (m_playerY + m_playerSize > height()) m_playerY = height() - m_playerSize;

    QPointF playerCenter(
        m_playerX + m_playerSize / 2.0,
        m_playerY + m_playerSize / 2.0
        );

    // 更新敌人
    for (int i = 0; i < m_enemies.size(); ++i)
    {
        if (!m_enemies[i].isAlive()) continue;

        QPointF enemyCenter = m_enemies[i].getCenter();

        // ===== 1. 追踪玩家方向 =====
        QPointF chaseDir = playerCenter - enemyCenter;

        double chaseLen = std::sqrt(chaseDir.x() * chaseDir.x() + chaseDir.y() * chaseDir.y());
        if (chaseLen > 1e-6)
        {
            chaseDir /= chaseLen;
        }
        else
        {
            chaseDir = QPointF(0, 0);
        }

        // ===== 2. 分离（避免重叠） =====
        QPointF separateDir(0, 0);

        for (int j = 0; j < m_enemies.size(); ++j)
        {
            if (i == j) continue;
            if (!m_enemies[j].isAlive()) continue;

            QPointF otherCenter = m_enemies[j].getCenter();
            QPointF diff = enemyCenter - otherCenter;

            double dist = std::sqrt(diff.x() * diff.x() + diff.y() * diff.y());

            double minDist = 60.0; // 安全距离

            if (dist < minDist && dist > 1e-6)
            {
                diff /= dist;
                double weight = (minDist - dist) / minDist;
                separateDir += diff * weight;
            }
        }

        // ===== 3. 合成方向 =====
        QPointF finalDir = chaseDir + separateDir * 1.2;

        double finalLen = std::sqrt(finalDir.x() * finalDir.x() + finalDir.y() * finalDir.y());
        if (finalLen > 1e-6)
        {
            finalDir /= finalLen;
        }
        else
        {
            finalDir = QPointF(0, 0);
        }

        double enemySpeed = 100.0;

        m_enemies[i].moveBy(
            finalDir.x() * enemySpeed * dt,
            finalDir.y() * enemySpeed * dt
            );
    }

    // 玩家受伤冷却
    if (m_playerDamageTimer > 0)
    {
        m_playerDamageTimer -= dt;
    }

    // 敌人碰撞玩家
    QRectF playerRect(m_playerX, m_playerY, m_playerSize, m_playerSize);
    for (int i = 0; i < m_enemies.size(); ++i)
    {
        if (!m_enemies[i].isAlive()) continue;

        if (m_enemies[i].getRect().intersects(playerRect))
        {
            if (m_playerDamageTimer <= 0.0)
            {
                m_playerHp -= 10;
                if (m_playerHp < 0) m_playerHp = 0;
                m_playerDamageTimer = 0.5;

                if (m_playerHp <= 0)
                {
                    m_gameOver = true;
                }
            }
        }
    }

    // 更新子弹
    for (int i = 0; i < m_bullets.size(); ++i)
    {
        m_bullets[i].update(dt);
    }

    // 子弹打敌人
    for (int i = 0; i < m_bullets.size(); ++i)
    {
        if (!m_bullets[i].isAlive()) continue;

        QPointF bulletPos = m_bullets[i].getPos();

        for (int j = 0; j < m_enemies.size(); ++j)
        {
            if (!m_enemies[j].isAlive()) continue;

            if (m_enemies[j].getRect().contains(bulletPos))
            {
                bool wasAlive = m_enemies[j].isAlive();

                m_enemies[j].takeDamage(25);
                m_bullets[i].destroy();

                if (wasAlive && !m_enemies[j].isAlive())
                {
                    m_killCount++;
                }

                break;
            }
        }
    }

    // 清理死亡子弹
    for (int i = m_bullets.size() - 1; i >= 0; --i)
    {
        if (!m_bullets[i].isAlive())
        {
            m_bullets.remove(i);
        }
    }
}

void GameWindow::resetGame()
{
    m_playerX = 100;
    m_playerY = 100;
    m_playerHp = m_playerMaxHp;
    m_playerDamageTimer = 0.0;

    m_moveUp = false;
    m_moveDown = false;
    m_moveLeft = false;
    m_moveRight = false;

    m_gameOver = false;
    m_killCount = 0;

    m_bullets.clear();
    m_enemies.clear();

    m_enemies.append(Enemy(700, 120, 50, 100));
    m_enemies.append(Enemy(760, 300, 50, 100));
    m_enemies.append(Enemy(650, 500, 50, 100));
}
